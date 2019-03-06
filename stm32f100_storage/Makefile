PLATFORM				= stm32
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
#MCU							= STM32L151C8
#MCU							= STM32F072RB
MCU							= STM32F100RB

$(info MCU: $(MCU))

#LIB_FOLDER					= STM32L1xx
#LIB_FOLDER					= STM32F0xx
LIB_FOLDER					= STM32F10x/Include

TARGET_NAME                 = storage_$(MCU)
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
SRC_AS                      = startup_cortexm.S

SRC_C						= system.c
SRC_C						+= $(PLATFORM)_power.c $(PLATFORM)_uart.c $(PLATFORM)_pin.c
SRC_C						+= board.c crc.c
SRC_C                       += main.c 
SRC_C 						+= dbg.c printf.c 

SRC_C						+= flash.c storage.c flash_update.c

OBJ                         = $(SRC_AS:%.S=%.o) $(SRC_C:%.c=%.o)
#----------------------------------------------------------
DEFINES                     = -D$(MCU) -D__CORTEX -D'__MCU="$(MCU)"' -D'__BUILD_TIME="$(CURRENT_DATE)"'
ifeq ($(LIB_FOLDER), STM32F0xx)
MCU_FLAGS                   = -mcpu=cortex-m0 -mthumb -D__CORTEX_M0 -mtune=cortex-m0 -std=gnu99
else 
MCU_FLAGS                   = -mcpu=cortex-m3 -mthumb -D__CORTEX_M3 -D__thumb2__=1 -mtune=cortex-m3 -msoft-float -mapcs-frame -std=gnu99
endif
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
	@$(OBJCOPY) -O ihex $(BUILD_DIR)/$(TARGET_NAME).elf $(BUILD_DIR)/$(TARGET_NAME).hex
	@$(OBJDUMP) -h -S -z $(BUILD_DIR)/$(TARGET_NAME).elf > $(BUILD_DIR)/$(TARGET_NAME).lss
	@$(NM) -n $(BUILD_DIR)/$(TARGET_NAME).elf > $(BUILD_DIR)/$(TARGET_NAME).sym
	@mkdir -p $(OUTPUT_DIR)
	@mv $(BUILD_DIR)/$(TARGET_NAME).bin $(OUTPUT_DIR)/$(TARGET_NAME).bin
	@mv $(BUILD_DIR)/$(TARGET_NAME).hex $(OUTPUT_DIR)/$(TARGET_NAME).hex

.c.o:
	@-mkdir -p $(BUILD_DIR)
	@echo CC: $<
	@$(GCC) $(FLAGS_CC) -c ./$< -o $(BUILD_DIR)/$@

.S.o:
	@-mkdir -p $(BUILD_DIR)
	@echo AS_C: $<
	@$(GCC) $(INCLUDES) -I. $(DEFINES) -c -x assembler-with-cpp ./$< -o $(BUILD_DIR)/$@

program:
	ST-LINK_CLI -c SWD -P $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x08000000 -Rst

clean:
	@echo '-----------------------------------------------------------'
	@rm -f build/*.*

test:
	@echo $(VPATH)

.PHONY : all clean program flash
