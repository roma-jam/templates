OPTIMIZATION            = s

#----------------------------------------------------------
#PATH must be set to CodeSourcery/bin
CROSS                      = arm-none-eabi-

GCC                        = $(CROSS)gcc
AS                         = $(CROSS)as
SIZE                       = $(CROSS)size
OBJCOPY                    = $(CROSS)objcopy
OBJDUMP                    = $(CROSS)objdump
NM                         = $(CROSS)nm

#----------------------------------------------------------
#MCU                         = NRF51822QFAA
MCU							= NRF52832QFAA
TARGET_NAME                 = nrf_template
#----------------------------------------------------------
BUILD_DIR                   = build
OUTPUT_DIR                  = output
REXOS                       = ../../rexos
KERNEL                      = $(REXOS)/kernel
USERSPACE                   = $(REXOS)/userspace
LIB                         = $(REXOS)/lib
LDS_SCRIPT                  = $(KERNEL)/arm.ld.S
#----------------------------------------------------------
CMSIS_DIR                   = $(REXOS)/CMSIS
#CMSIS_DEVICE_DIR            = $(CMSIS_DIR)/Device/ST/STM32L0xx
#CMSIS_DEVICE_DIR            = $(CMSIS_DIR)/Device/ST/STM32F10x
#CMSIS_DEVICE_DIR           = $(CMSIS_DIR)/Device/ST/STM32F2xx
#CMSIS_DEVICE_DIR           = $(CMSIS_DIR)/Device/ST/STM32F4xx
#CMSIS_DEVICE_DIR            = $(CMSIS_DIR)/Device/NXP/LPC11Uxx
#CMSIS_DEVICE_DIR            = $(CMSIS_DIR)/Device/NORDIC/nRF51
CMSIS_DEVICE_DIR            = $(CMSIS_DIR)/Device/NORDIC/nRF52
#----------------------------------------------------------
#not used in kernel
INCLUDE_FOLDERS             = $(CMSIS_DIR)/Include $(CMSIS_DEVICE_DIR)
#kernel
INCLUDE_FOLDERS            += $(KERNEL) $(KERNEL)/core
#lib
INCLUDE_FOLDERS            += $(LIB)
#userspace
INCLUDE_FOLDERS            += $(USERSPACE) $(USERSPACE)/core $(USERSPACE)/nrf
#sys
INCLUDE_FOLDERS            += $(REXOS)/kernel/drv $(REXOS)/kernel/nrf $(REXOS)/midware $(REXOS)/midware/rf
INCLUDE_FOLDERS            += $(REXOS)/midware/fs

INCLUDES                    = $(INCLUDE_FOLDERS:%=-I%)
VPATH                      += $(INCLUDE_FOLDERS)
#----------------------------------------------------------
#core-dependent part
SRC_C                       = kcortexm.c
SRC_AS                      = startup_cortexm.S cortexm.S
#kernel
SRC_C						+= kernel.c dbg.c kstdlib.c karray.c kso.c kirq.c kprocess.c ksystime.c kipc.c kstream.c kobject.c kio.c kerror.c kexo.c
#lib
SRC_C                      += lib_lib.c lib_systime.c pool.c printf.c lib_std.c lib_stdio.c lib_array.c lib_so.c
#drv
SRC_C						+= nrf_pin.c nrf_gpio.c nrf_power.c nrf_timer.c nrf_exo.c nrf_uart.c nrf_rtc.c nrf_rf.c 
#nrf_flash.c nrf_rng.c nrf_temp.c
#SRC_C						+= nrf_wdt.c nrf_adc.c
#SRC_C                      += stm32_pin.c stm32_gpio.c stm32_power.c stm32_timer.c stm32_rtc.c stm32_exo.c stm32_uart.c stm32_otg.c stm32_eth.c
#userspace lib
SRC_C						+= ipc.c io.c process.c stdio.c stdlib.c systime.c time.c uart.c power.c stream.c pin.c flash.c wdt.c
#SRC_C                      += ipc.c io.c process.c stdio.c stdlib.c systime.c time.c uart.c usb.c power.c stream.c pin.c
#SRC_C                      += eth.c tcpip.c mac.c icmp.c ip.c arp.c tcp.c
SRC_C 						+= nrf_radio.c 
SRC_C						+= nrf_ble.c

#SRC_C						+= storage.c vfs.c utf.c
#midware
#SRC_C						+= fat16.c vfss.c ber.c
#SRC_C						+= pinboard.c rfd.c 
#SRC_C                      += usbd.c cdc_acmd.c eth_phy.c tcpips.c macs.c routes.c arps.c ips.c icmps.c tcps.c
#userspace lib
SRC_C                      += app.c 
#SRC_C						+= button.c fs.c
#SRC_C						+= lcd.c lcd_format.c

OBJ                         = $(SRC_AS:%.S=%.o) $(SRC_C:%.c=%.o)
#----------------------------------------------------------
DEFINES                     = -D$(MCU) -D'__MCU="$(MCU)"'
MCU_FLAGS                   = -mcpu=cortex-m0 -mthumb -D__CORTEX_M0 -mtune=cortex-m0 -g3 -std=gnu99
#MCU_FLAGS                   = -mcpu=cortex-m4 -mthumb -D__CORTEX_M4 -mtune=cortex-m4 -g3 -std=gnu99
NO_DEFAULTS                 = -fdata-sections -ffunction-sections -fno-hosted -fno-builtin  -nostdlib -nodefaultlibs
FLAGS_CC                    = $(INCLUDES) $(DEFINES) -I. -O$(OPTIMIZATION) -Wall -c -fmessage-length=0 $(MCU_FLAGS) $(NO_DEFAULTS)
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
	@$(OBJCOPY) -O ihex $(BUILD_DIR)/$(TARGET_NAME).elf $(BUILD_DIR)/$(TARGET_NAME).hex
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
	@$(GCC) $(INCLUDES) -I. $(DEFINES) -c -x assembler-with-cpp -g3 ./$< -o $(BUILD_DIR)/$@

program:
#	@st-flash write $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x8000000
#	@openocd -f stm32f1.cfg -c "program $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x08000000 reset exit"
	@openocd -f nrf51822.cfg -c "program $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x00000000" -c "reset" -c "exit 0x0"

clean:
	@echo '-----------------------------------------------------------'
	@rm -f build/*.*

test:
	@echo $(VPATH)

.PHONY : all clean program flash
