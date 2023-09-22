include environment.mk

# Directory to keep generated built binaries in
BUILD_DIR=build

# Options passed to GCC that specify the architecture of the target, in this case a cortex-m0+ running in Thumb Mode
OPTIONS_ARCH=-mthumb\
				-mcpu=cortex-m4

# Options passed to GCC that specify the optimization level of the program, in this case, optimize for size while including as much debug information
OPTIONS_OPT=-Os\
			-g3\

# Other options passed to GCC
#! if the nano.specs option is set, then the symbols for printf and scanf to print floating point numbers are not included by default
#! if the rdimon.specs option is set, then semihosting is enabled (comment out this option to disable semihosting)
OPTIONS_OTHER=-Wall\
				-Werror\
				-ffunction-sections\
				-fdata-sections\
				--specs=nano.specs\
				--specs=nosys.specs\
				--specs=rdimon.specs

# Other directories for GCC to search for header files
HEADER_SEARCH_DIRS=-I"CMSIS/Include"\
					-I"CMSIS/Device/ST/STM32F4xx/Include"\
					-I"Inc"

# Preprocesor macro definitions for GCC
# for its header file to get included
PREPROCESSOR_MACROS=-DSTM32F401xC

# Linker options passed to GCC for linking the program
#! if the -lrdimo option is set, then semihosting is enabled (comment out this option to disable semihosting)
OPTIONS_LINK=-Wl,--gc-sections,-lgcc,-lc,-lnosys,-lrdimon,-Map=$(BUILD_DIR)/main.map

# Linker script to include while building the project
LINKER_SCRIPT=-Tstm32f4xx.ld

# Startup code (in assembly)
STARTUP=CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f401xc.s

# Source code files to be compiled (in C)
SRCS=$(filter-out Src/main.c, $(wildcard Src/*.c))

.PHONY: build clean flash

# First compile the driver file into an object file
# Then compile the demo file along with the startup, system and driver to get an ELF (linking done at this step)
# Convert the ELF file into the final binaries (BIN and HEX) and get map and list files
build:
	$(GCC)\
		$(OPTIONS_ARCH)\
		$(OPTIONS_OPT)\
		$(OPTIONS_OTHER)\
		$(HEADER_SEARCH_DIRS)\
		$(PREPROCESSOR_MACROS)\
		$(SRCS) \
		-c -o $(BUILD_DIR)/gpio.o
	$(GCC)\
		$(OPTIONS_ARCH)\
		$(OPTIONS_OPT)\
		$(OPTIONS_OTHER)\
		$(HEADER_SEARCH_DIRS)\
		$(PREPROCESSOR_MACROS)\
		$(OPTIONS_LINK)\
		$(LINKER_SEARCH_DIRS)\
		$(LINKER_SCRIPT)\
		Src/main.c $(STARTUP) CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c $(BUILD_DIR)/gpio.o \
		-o $(BUILD_DIR)/main.elf
	$(OBJDUMP) -S $(BUILD_DIR)/gpio.o > $(BUILD_DIR)/gpio.list
	$(OBJDUMP) -S $(BUILD_DIR)/main.elf > $(BUILD_DIR)/main.list
	$(SIZE) $(BUILD_DIR)/main.elf
	$(OBJCOPY) --output-target=binary $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.bin
	$(OBJCOPY) --output-target=ihex $(BUILD_DIR)/main.elf $(BUILD_DIR)/main.hex


clean:
	rm -rf build


flash:
	$(OPENOCD) \
		-f $(OPENOCD_SCRIPT_PATH)/interface/stlink.cfg \
		-f $(OPENOCD_SCRIPT_PATH)/target/stm32f4x.cfg \
		-c "program $(BUILD_DIR)/main.hex reset exit"
