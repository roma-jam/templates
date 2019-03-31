PLATFORM 				= nrf

OPTIMIZATION            = s
CURRENT_DATE			= $(shell date +"%D %T")

#----------------------------------------------------------
CROSS                      = arm-none-eabi-

GCC                        = $(CROSS)gcc
AS                         = $(CROSS)as
SIZE                       = $(CROSS)size
OBJCOPY                    = $(CROSS)objcopy
OBJDUMP                    = $(CROSS)objdump
NM                         = $(CROSS)nm

#----------------------------------------------------------
MCU                         = NRF51822QFAA

LIB_FOLDER 					= NRF51

$(info MCU: $(MCU))

TARGET_NAME                 = template_$(MCU)
#----------------------------------------------------------
BUILD_DIR                   = build
OUTPUT_DIR                  = output
CORE_DIR					= core
BOARD_DIR					= board
LDS_SCRIPT                  = $(CORE_DIR)/script.ld.S

#----------------------------------------------------------
INCLUDE_FOLDERS 			= ./ ../ $(CORE_DIR) $(BOARD_DIR)
INCLUDE_FOLDERS 			+= $(CORE_DIR)/$(PLATFORM) $(BOARD_DIR)/$(PLATFORM)
INCLUDE_FOLDERS 			+= $(CORE_DIR)/$(PLATFORM)/$(LIB_FOLDER)

INCLUDES                    = $(INCLUDE_FOLDERS:%=-I%)
VPATH                      += $(INCLUDE_FOLDERS)
#----------------------------------------------------------
#core-dependent part
SRC_AS                      = startup_cortexm.S cortexm.S

SRC_C						= nrf_power.c nrf_pin.c nrf_uart.c nrf_radio.c

SRC_C						+= board.c system.c

SRC_C                       += main.c
SRC_C 						+= dbg.c printf.c 

OBJ                         = $(SRC_AS:%.S=%.o) $(SRC_C:%.c=%.o)
#----------------------------------------------------------
DEFINES                     = -D$(MCU) -D__CORTEX -D'__MCU="$(MCU)"' -D'__BUILD_TIME="$(CURRENT_DATE)"'

MCU_FLAGS                   = -mcpu=cortex-m0 -mthumb -D__CORTEX_M0 -mtune=cortex-m0 -std=gnu99

NO_DEFAULTS                 = -fdata-sections -ffunction-sections -fno-hosted -fno-builtin  -nostdlib -nodefaultlibs
FLAGS_CC                    = $(INCLUDES) $(DEFINES) -I. -O$(OPTIMIZATION) -c -fmessage-length=0 $(MCU_FLAGS) $(NO_DEFAULTS)
FLAGS_LD                    = -Xlinker --gc-sections $(MCU_FLAGS)
#----------------------------------------------------------
all: $(TARGET_NAME).elf

%.elf: $(OBJ) $(LDS_SCRIPT)
	@$(GCC) $(INCLUDES) -I. $(DEFINES) -DLDS -E $(LDS_SCRIPT) -o $(BUILD_DIR)/script.ld.hash
	@awk '!/^(\ )*#/ {print $0}' $(BUILD_DIR)/script.ld.hash > $(BUILD_DIR)/script.ld
	@echo LD: $(OBJ)
	@$(GCC) $(FLAGS_LD) -T $(BUILD_DIR)/script.ld -o $(BUILD_DIR)/$@ $(OBJ:%.o=$(BUILD_DIR)/%.o)
	@echo '-----------------------------------------------------------'
	@$(SIZE) $(BUILD_DIR)/$(TARGET_NAME).elf
	@$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET_NAME).elf $(BUILD_DIR)/$(TARGET_NAME).bin
	@$(OBJDUMP) -h -S -z $(BUILD_DIR)/$(TARGET_NAME).elf > $(BUILD_DIR)/$(TARGET_NAME).lss
	@$(NM) -n $(BUILD_DIR)/$(TARGET_NAME).elf > $(BUILD_DIR)/$(TARGET_NAME).sym
	@mkdir -p $(OUTPUT_DIR)
	@mv $(BUILD_DIR)/$(TARGET_NAME).bin $(OUTPUT_DIR)/$(TARGET_NAME).bin
	
.c.o:
	@-mkdir -p $(BUILD_DIR)
	@echo CC: $<
	@$(GCC) $(FLAGS_CC) -c ./$< -o $(BUILD_DIR)/$@

.S.o:
	@-mkdir -p $(BUILD_DIR)
	@echo AS_C: $<
	@$(GCC) $(INCLUDES) -I. $(DEFINES) -c -x assembler-with-cpp ./$< -o $(BUILD_DIR)/$@

program:
#	@openocd -f nrf51822.cfg -c "init" -c "flash probe 0" -c "exit"
	@openocd -f nrf51822.cfg -c "program $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x00000000" -c "reset" -c "exit 0x0"
	
reset:
	@openocd -f nrf51822.cfg -c "init" -c "reset" -c "exit 0x0"

clean:
	@echo '-----------------------------------------------------------'
	@rm -f build/*.*

test:
	@echo $(VPATH)

.PHONY : all clean program reset
