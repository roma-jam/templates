#PLATFORM				= kinetis
PLATFORM				= stm32
#PLATFORM				= lpc

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
ifeq ($(PLATFORM), kinetis)
MCU                         = MK22FN256VLH12
else 
ifeq ($(PLATFORM), stm32)
#MCU							= STM32L151C8
#MCU							= STM32F072RB
MCU							= STM32F100RB

#LIB_FOLDER					= STM32L1xx
#LIB_FOLDER					= STM32F0xx
LIB_FOLDER					= STM32F10x/Include

else 
MCU							= LPC1837JET100
endif
endif

$(info MCU: $(MCU))

TARGET_NAME                 = puf_$(MCU)
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
ifeq ($(PLATFORM), kinetis)
SRC_AS                      += startup_kinetis.S
endif	

SRC_C						= system.c
SRC_C						+= $(PLATFORM)_power.c $(PLATFORM)_uart.c $(PLATFORM)_pin.c
ifeq ($(PLATFORM), kinetis)
#SRC_C                      	+= $(PLATFORM)_otg.c $(PLATFORM)_resident.c 
else
ifeq ($(PLATFORM), lpc)
#SRC_C						+= $(PLATFORM)_otg.c lpc_iap.c
else 
#SRC_C						+= $(PLATFORM)_usb.c
endif	 
endif
SRC_C						+= board.c crc.c
SRC_C                       += main.c
SRC_C 						+= dbg.c printf.c 

SRC_C						+= galois.c rs.c


OBJ                         = $(SRC_AS:%.S=%.o) $(SRC_C:%.c=%.o)
#----------------------------------------------------------
DEFINES                     = -D$(MCU) -D__CORTEX -D'__MCU="$(MCU)"' -D'__BUILD_TIME="$(CURRENT_DATE)"'
ifeq ($(PLATFORM), kinetis)
MCU_FLAGS                   = -mcpu=cortex-m4 -mthumb -D__CORTEX_M4 -D__thumb2__=1 -mtune=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mapcs-frame -std=gnu99
MCU_FLAGS                   +=-mno-unaligned-access
else 
ifeq ($(LIB_FOLDER), STM32F0xx)
MCU_FLAGS                   = -mcpu=cortex-m0 -mthumb -D__CORTEX_M0 -mtune=cortex-m0 -std=gnu99
else 
MCU_FLAGS                   = -mcpu=cortex-m3 -mthumb -D__CORTEX_M3 -D__thumb2__=1 -mtune=cortex-m3 -msoft-float -mapcs-frame -std=gnu99
endif
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
ifeq ($(PLATFORM), lpc)
	python checksum_calc.py $(OUTPUT_DIR)/$(TARGET_NAME).bin
endif

.c.o:
	@-mkdir -p $(BUILD_DIR)
	@echo CC: $<
	@$(GCC) $(FLAGS_CC) -c ./$< -o $(BUILD_DIR)/$@

.S.o:
	@-mkdir -p $(BUILD_DIR)
	@echo AS_C: $<
	@$(GCC) $(INCLUDES) -I. $(DEFINES) -c -x assembler-with-cpp ./$< -o $(BUILD_DIR)/$@

program:
ifeq ($(PLATFORM), kinetis)
	JLink -device MK22FN256xxx12 -CommanderScript cmd_$(PLATFORM).jlink
else 
ifeq ($(PLATFORM), stm32)
	ST-LINK_CLI -c SWD -P $(OUTPUT_DIR)/$(TARGET_NAME).bin 0x08000000 -Rst
else 
	JLink -device LPC1837 -CommanderScript cmd_$(PLATFORM).jlink
endif
endif

clean:
	@echo '-----------------------------------------------------------'
	@rm -f build/*.*

test:
	@echo $(VPATH)

.PHONY : all clean program flash
