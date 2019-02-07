/*
 * main.c
 *
 *  Created on: 5 θώνÿ 2017 γ.
 *      Author: RLeonov
 */

#include <string.h>
#include "board.h"
#include "system.h"

#if (DFU_DEBUG)
#include "dbg.h"
#endif // DFU_DEBUG

#include "core/stm32/STM32F10x/Include/stm32f10x.h"


#define RAM_WORD(addr)              (*(uint32_t*)(addr))
#define RAM_SECTOR_SIZE_BYTES       24
#define RAM_STACK_RESERVED          2048
#define BIT_AVERAGE                 2

#define UID_SIZE_BYTES              12
#define KEY_SIZE_BYTES              32

bool is_address_valid(volatile const uint32_t *address)
{
    uint32_t value = 0;
    bool is_valid = true;
    /* Cortex-M3, Cortex-M4, Cortex-M4F, Cortex-M7 are supported */
    static const uint32_t BFARVALID_MASK = (0x80 << SCB_CFSR_BUSFAULTSR_Pos);

    /* Clear BusFault (BFARVALID) flag by writing 1 to it */
    SCB->CFSR |= BFARVALID_MASK;

    /* Ignore BusFault by enabling BFHFNMIGN and disabling interrupts */
    uint32_t mask = __get_FAULTMASK();

    __disable_fault_irq();

    SCB->CCR |= SCB_CCR_BFHFNMIGN_Msk;

    /* probe address */
    value = *address;

    /* Check BFARVALID flag */
    if ((SCB->CFSR & BFARVALID_MASK) != 0)
    {
        /* Bus Fault occured reading the address */
        is_valid = false;
    }

    /* Reenable BusFault by clearing  BFHFNMIGN */
    SCB->CCR &= ~SCB_CCR_BFHFNMIGN_Msk;
    __set_FAULTMASK(mask);

    return is_valid;
}




void get_ram_key(uint8_t out_key[32])
{
    /* count stat foo */
    uint32_t amount = 0;
    uint16_t sectors = 0;
    uint32_t avg = 0;
    printf("Stat buf size: %d\n", (SRAM_SIZE - RAM_STACK_RESERVED) / RAM_SECTOR_SIZE_BYTES);
    uint8_t stat_buf[(SRAM_SIZE - RAM_STACK_RESERVED) / RAM_SECTOR_SIZE_BYTES] = { 0 };

    /* measure statistics for every 32-bit in ram */
    for(uint32_t ram_addr = SRAM_BASE; ram_addr < SRAM_BASE + SRAM_SIZE - RAM_STACK_RESERVED; ram_addr += sizeof(uint32_t))
    {
        /* count 1 in every 32 bit */
        for(uint8_t bit = 0; bit < 32; bit++)
        {
            if((RAM_WORD(ram_addr) & (1 << bit)) != 0)
                amount++;
        }

        /* first word doesn't count */
        if(ram_addr == SRAM_BASE)
            continue;

        /* every sector size print statistic */
        if((ram_addr % RAM_SECTOR_SIZE_BYTES) == 0)
        {
//            printf("addr %#X, amount %d\n", ram_addr, amount);
//            printf("%d\n", amount);
            delay_us(9);
            avg += amount;
            amount = 0;
            sectors++;
        }
    }

    avg = avg / sectors;
    printf("Total sectors (size %d): %d\n", RAM_SECTOR_SIZE_BYTES, sectors);
    printf("Average: %d\n", avg);

    /* fill stat buf */
    sectors = 0;
    for(uint32_t ram_addr = SRAM_BASE; ram_addr < SRAM_BASE + SRAM_SIZE - RAM_STACK_RESERVED; ram_addr += sizeof(uint32_t))
    {
        /* count 1 in every 32 bit */
        for(uint8_t bit = 0; bit < 32; bit++)
        {
            if((RAM_WORD(ram_addr) & (1 << bit)) != 0)
                amount++;
        }

        /* first word doesn't count */
        if(ram_addr == SRAM_BASE)
            continue;

        /* every 2 KB print statistic */
        if((ram_addr % RAM_SECTOR_SIZE_BYTES) == 0)
        {
            if(amount > avg)
                stat_buf[sectors] = 1;
            else
                stat_buf[sectors] = 0;

//            printf("%d", stat_buf[sectors]);
            delay_us(9);
            amount = 0;
            sectors++;
        }
    }

//    printf("\n");

    /* prepare data buffer */
    uint8_t cnt = 0;
    uint8_t bit = 0;
    while(cnt < KEY_SIZE_BYTES)
    {
        if((cnt * 8) + bit > sectors)
            break;
//        printf("cnt: %d\n", cnt);
//        printf("stat_buf bit1 value: %02X\n", stat_buf[(cnt * 8) + bit]);
//        printf("bit: %d\n", bit);
        out_key[cnt] <<= 1;

        out_key[cnt] |= stat_buf[(cnt * 8) + bit];
        bit++;

        if(bit > 7)
        {
//            printf("Key byte[%d] = %02X\n", cnt, out_key[cnt]);
            cnt++;
            bit = 0;
        }
    }
}

void get_uid(uint8_t uid[UID_SIZE_BYTES], uint8_t size_words)
{
    uint32_t addr = 0x1FFFF7E8;
    for(uint8_t i = 0; i < size_words; i++)
        *((uint32_t*)(uid + i*sizeof(uint32_t))) = *((uint32_t*)(addr + i*sizeof(uint32_t)));

}

void pfu_test()
{
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
    printf("PUF Test, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
//    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

    printf("SRAM base: %#X, size: %d KB\n", SRAM_BASE, SRAM_SIZE >> 10);

    /* check RAM access */
    uint32_t addr = SRAM_BASE;
    uint32_t total_sram = 0;
    do
    {
        total_sram += sizeof(uint32_t);
        addr += sizeof(uint32_t);
    }
    while (is_address_valid((const uint32_t*)addr));

    printf("Actual SRAM: %d Bytes\n", total_sram);

    uint8_t uid[UID_SIZE_BYTES] = { 0 };
    uint8_t key[KEY_SIZE_BYTES] = { 0 };

    get_ram_key(key);

    printf("UID:");
    for(int i = 0; i < 16; i++)
        printf(" %02X", uid[i]);
    printf("\n");

    printf("Key:");
    for(int i = 0; i < 32; i++)
        printf(" %02X", key[i]);
    printf("\n");

    printf("======================================\n");
    delay_ms(10);
#if (DFU_DEBUG)
    printf("HALT.\n");
#endif // DFU_DEBUG
    for(;;) { }
}


#include "rs.h"
struct gf_poly temp_poly;
uint8_t temp_buffer[NELE];
uint8_t msg_buffer[NELE];
uint8_t expect[NELE];

void test_case(char *msg, int len, char *err, int len_err, int ret_exp)
{
    int i;
    int ret;

    struct gf_poly msg_in;
    struct gf_poly err_in;

    memcpy(msg_buffer, msg, len);

    msg_in.dat = (uint8_t *)msg_buffer;
    msg_in.len = len;

//    gf_print_data("IN ", &msg_in);

    rs_encode_msg(&msg_in);

//    gf_print_data("ENC", &msg_in);

    memcpy(expect, msg_in.dat, msg_in.len);

    err_in.dat = (uint8_t *)err;
    err_in.len = len_err;

//    gf_print_data("ERR", &err_in);

    gf_poly_add(&temp_poly, &msg_in, &err_in);
    gf_poly_copy(&msg_in, &temp_poly);

//    gf_print_data("ADD", &msg_in);

    ret = rs_decode_msg(&msg_in);

//    gf_print_data("DEC", &msg_in);

    if(ret == REED_SOLOMON_ERROR_CORRECT || ret == REED_SOLOMON_NO_ERROR)
    {
        for(i = 0; i < msg_in.len; i++)
        {
            if(msg_in.dat[i] != expect[i])
            {
                printf("i: %d, expect: %02X, result: %02X\n", i, expect[i], msg_in.dat[i]);
            }
        }
    }

    if(ret == ret_exp)
    {
        printf("PASS\n");
        return;
    }
    else
    {
        printf("ret: %d, ret_exp: %d\n", ret, ret_exp);
    }
}

void rs_test()
{
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
    printf("Reed Solomon Test, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

    temp_poly.dat = temp_buffer;
    temp_poly.len = 0;

    // initial reed_solomon
    rs_init();

    test_case("01234567", 8, "q\0\0\0\0c\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("ABCDEF10", 8, "q\0\0\0\0c\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("01234567", 8, "q\0\0\0\0c\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("01234567", 8, "qb\0\0\0\0\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("basdfeqd", 8, "q\0\0\0\0\0\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("1234kcvj", 8, "q\0\0\0a\0\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("akjviu23", 8, "q\0\0\0\0\0\0e\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("ajk38723", 8, "q\0\0\0b\0\0\0\0\0\0\0", 12, REED_SOLOMON_ERROR_CORRECT);
    test_case("ajk38723", 8, "q\0\0\0b\0\0\0\0\0e\0", 12, REED_SOLOMON_TOO_MUCH_ERROR);
    test_case("ajk38723", 8, "q\0\0\0b\0a\0\0\0e\0", 12, REED_SOLOMON_TOO_MUCH_ERROR);
    test_case("ajk38723", 8, "\0\0\0\0\0\0\0\0\0\0\0\0", 12, REED_SOLOMON_NO_ERROR);

    printf("HALT.\n");
    while(1);
}


/* divercify key */
void key_prepare_divercify()
{
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
    printf("Divercify Key, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG

    uint8_t KeyData[UID_SIZE_BYTES + KEY_SIZE_BYTES + NSYM] = { 0 };
    /* get KeyData: UID[16] || SRAM_NOT_ZERO_AVG_CNT[32] */
    /* UID */
    get_uid(KeyData, UID_SIZE_BYTES / sizeof(uint32_t));

    /* get SRAM value */
    get_ram_key(KeyData + UID_SIZE_BYTES);

    printf("Key:");
    for(int i = 0; i < KEY_SIZE_BYTES; i++)
        printf(" %02X", KeyData[UID_SIZE_BYTES + i]);
    printf("\n");

    // init reed_solomon
    rs_init();
    /* count BB sequence */
    struct gf_poly msg_in;
    msg_in.dat = (uint8_t *)KeyData;
    msg_in.len = UID_SIZE_BYTES + KEY_SIZE_BYTES;

    gf_print_data("IN ", &msg_in);

    rs_encode_msg(&msg_in);

    gf_print_data("ENC ", &msg_in);

    printf("HALT.\n");
    while(1);
}


/*
 * UID  39 2D D9 05 47 59 37 32 15 1F 08 43
 * KEY  B3 64 E4 8C A6 E3 98 1E 3A 78 8D 8B 0E 0B 7F EF BF 16 76 6C BA 0A 13 33 A8 3B 5A C8 82 A1 B7 FC
 * BB   85 9B 49 2A 30 81 84 74 38 B8 F8 93 AF 77 A3 CE 80 62 58 E1 64 3C 21 C3 93 B9 90 FC A9 33 42 D1 60 4B D0 F0 E6 D3 95 21 DE 82 CB A4
 *
 */

static const uint8_t KK[KEY_SIZE_BYTES] =
{
        0xB3, 0x64, 0xE4, 0x8C, 0xA6, 0xE3, 0x98, 0x1E,
        0x3A, 0x78, 0x8D, 0x8B, 0x0E, 0x0B, 0x7F, 0xEF,
        0xBF, 0x16, 0x76, 0x6C, 0xBA, 0x0A, 0x13, 0x33,
        0xA8, 0x3B, 0x5A, 0xC8, 0x82, 0xA1, 0xB7, 0xFC
};

static const uint8_t BB[NSYM] =
{
        0x85, 0x9B, 0x49, 0x2A, 0x30, 0x81, 0x84, 0x74,
        0x38, 0xB8, 0xF8, 0x93, 0xAF, 0x77, 0xA3, 0xCE,
        0x80, 0x62, 0x58, 0xE1, 0x64, 0x3C, 0x21, 0xC3,
        0x93, 0xB9, 0x90, 0xFC, 0xA9, 0x33, 0x42, 0xD1,
        0x60, 0x4B, 0xD0, 0xF0, 0xE6, 0xD3, 0x95, 0x21,
        0xDE, 0x82, 0xCB, 0xA4
};


#define HI_OCT(x)   ((x) & 0xF0)
#define LO_OCT(x)   ((x) & 0x0F)

void key_divercify()
{
    int corr = 0;
    int corr_octets = 0;
    board_init();

#if (DFU_DEBUG)
    board_dbg_init();
    printf("Divercify Key, v%d.%d, %s\n", VERSION >> 8, VERSION & 0xff, __BUILD_TIME);
    printf("%s core, %d MHz\n", __MCU, power_get_core_clock() / 1000000);
#endif // DFU_DEBUG
    /* read data from OTG */

    uint8_t KeyData[UID_SIZE_BYTES + KEY_SIZE_BYTES + NSYM] = { 0 };
    /* get KeyData: UID[16] || SRAM_NOT_ZERO_AVG_CNT[32] */
    /* UID */
    get_uid(KeyData, UID_SIZE_BYTES / sizeof(uint32_t));
    /* get SRAM value */
    get_ram_key(KeyData + UID_SIZE_BYTES);

    printf("Key:");
    for(int i = 0; i < KEY_SIZE_BYTES; i++)
    {
        printf(" %02X", KeyData[UID_SIZE_BYTES + i]);

        /* also read corr coef */
        if(KK[i] == KeyData[UID_SIZE_BYTES + i])
            corr++;
    }
    printf("\n");


    for(int i = 0; i < KEY_SIZE_BYTES; i++)
    {
        if(HI_OCT(KK[i]) == HI_OCT(KeyData[UID_SIZE_BYTES + i]))
            corr_octets++;
        if(LO_OCT(KK[i]) == LO_OCT(KeyData[UID_SIZE_BYTES + i]))
            corr_octets++;
    }

    printf("Equal bytes: %d/32 (%d%%)\n", corr, 100 - ((KEY_SIZE_BYTES - corr) * 100)/KEY_SIZE_BYTES);
    printf("Equal octets: %d/64 (%d%%)\n", corr_octets, 100 - ((64 - corr_octets) * 100)/64);

    /* put BB seq */
    memcpy(KeyData + UID_SIZE_BYTES + KEY_SIZE_BYTES, BB, NSYM);

    /* init reed solomon */
    rs_init();
    /* count BB sequence */
    struct gf_poly msg_in;
    msg_in.dat = (uint8_t *)KeyData;
    msg_in.len = UID_SIZE_BYTES + KEY_SIZE_BYTES + NSYM;

//    gf_print_data("IN ", &msg_in);

    printf("rs decode status: %d\n", rs_decode_msg(&msg_in));

    printf("FIX:");
    for(int i = 0; i < KEY_SIZE_BYTES; i++)
        printf(" %02X", KeyData[UID_SIZE_BYTES + i]);
    printf("\n");
    printf("KEY:");
    for(int i = 0; i < KEY_SIZE_BYTES; i++)
        printf(" %02X", KK[i]);
    printf("\n");

    if(0 == memcmp(KK, KeyData + UID_SIZE_BYTES, KEY_SIZE_BYTES))
        printf("KEY HAS BEEN RESTORED.\n");
    else
        printf("KEY HAS NOT BEEN RESTORED.\n");

    printf("HALT.\n");
    while(1);
}

