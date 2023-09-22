# STM32F401CCU6-Peripheral-Drivers

This repository contains **lightweight** and **minimal** drivers for peripherals on the STM32F401CCU6. This microcontroller can be found on the popular [Black Pill Board V1.2](https://stm32-base.org/boards/STM32F401CCU6-WeAct-Black-Pill-V1.2.html), and is based on an ARM Cortex M4. It runs at 16 MHz, and contains a _Floating Point Unit (FPU)_, making it a very powerful microcontroller.

In ARM Cortex microcontrollers, peripherals are hardware components present within the microcontroller that provide necessary functionality to the main processor and memory, such as GPIO pins, USART, I2C etc. Each peripheral can be configured and controlled completely using a set of _Memory-Mapped Registers (MMRs)_.

Normally, managing MMRs directly in your code is quite tedious and reduces readability, which is why it is recommended to use  _Hardware Abstraction Layers (HALs)_ to ease the development process. The HAL needs to be written only once and can be re-used in multiple projects.

A HAL is divided into various drivers, each responsible for managing a single peripheral. A driver hides the details of the registers it is modifying, and provides artifacts that correspond to the results that must be achieved with the peripheral. The drivers contained in this repository are written with a focus on being minimal and easily understandable.

## Organization Of the Repository

The repository is organized as a set of directories, each containing a standalone project, complete with source files, header files, CMSIS, startup files, linker scripts and a working build-system (Makefiles have been used to maintain minimality). Unless specified within the project's readme, no outside dependencies have to be installed to build the project.

Each directory is responsible for a single driver, that can be used for your own project. The driver contains -

- ```Src/``` directory that contains ```.c``` source files (a ```<driver>.c``` file and a ```main.c``` file, unless specified otherwise).
- ```Inc/``` directory that contains ```.h``` header files (this must be added to the include-path of the consuming project).
- ```CMSIS/``` directory that contains the CMSIS header files for Peripheral Register Access and Startup files. _Note that most of the non-essential features from the CMSIS directory has been deleted to maintain brevity, specifically leaving out only the peripheral register aliases and startup file._
- ```stm32fdxx.ld``` GNU linker script that defines how the generated ELF should be organized internally to match the microcontroller's memory map.
- ```Makefile``` makefile that contains the ```build```, ```clean``` and ```flash``` targets for building the driver, removing the generated binaries and flashing the library-demo onto the microcontroller respectively.

The latest version of CMSIS can be downloaded either using the [CubeMX software](https://www.st.com/en/development-tools/stm32cubemx.html) from ST, or from [this location on the ST website](https://www.st.com/en/embedded-software/stm32cubef4.html). This downloads a _software pack_ for the microcontroller, which contains the CMSIS directory of interest.

## Building and Consuming the Drivers

### Dependencies

|Program|Installation|Remarks|
|-------|------------|-------|
|ARM GCC toolchain|Can be installed on most Linux distros using the builtin package manager, or from the [ARM Developer website](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) for Linux, Windows or MacOS|Used to cross-compile the program for the thumbv7 architecture found on the ARM Cortex M4.|
|GNU Make|Comes pre-installed on most Linux distros, can be installed from [gnuwin32](https://gnuwin32.sourceforge.net/packages/make.htm) for Windows|Used for build automation|
|OpenOCD **(optional)**|Guide for installing on your platform can be found on the [OpenOCD Website](https://openocd.org/pages/getting-openocd.html)|Used for flashing/uploading the library demo program to the microcontroller, and for debugging as a server for GDB|
|GDB multiarch **(optional)**|Can be installed on most Linux distros using the builtin package manager, on MacOS using Homebrew, and on Windows using MSYS2 |Used for debugging programs|

After installing the dependencies, it is important to create an ```environment.mk``` file within each driver's directory, next to the ```Makefile```. This contains paths to the various programs used for building and flashing, and is automatically imported by the makefile. The file must be populated by the following variables -

```make
GCC=<path to arm gcc>
LD=<path to arm ld (usually present in the same directory as gcc)>
OBJDUMP=<path to arm object dump (usually present in the same directory as gcc)>
SIZE=<path to arm size (usually present in the same directory as gcc)>
OBJCOPY=<path to arm objectcopy (usually present in the same directory as gcc)>

LINKER_SEARCH_DIRS=<additional directories for linker scripts>

OPENOCD=<path to openocd>
OPENOCD_SCRIPT_PATH=<path to openocd script files>
GDB=<path to gdb>
```

For example, if the host machine was running Ubuntu and the binaries were all added to the path, the file would look as follows -

```make
GCC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size
OBJCOPY=arm-none-eabi-objcopy

LINKER_SEARCH_DIRS=

OPENOCD=openocd
OPENOCD_SCRIPT_PATH=/usr/share/openocd/scripts
GDB=GDB-multiarch
```

**Creating the ```environment.mk``` file is a necessary step that can not be skipped.**
### Steps To Build And Consume

As mentioned, each driver is a standalone project that can be built and used seperately from all other drivers. Start by creating a ```build/``` directory within the driver's root directory. Then, simply run the following command in the root directory of the driver -

```bash
make build
```

This will build the following binaries of interest -

- ```<driver>.o``` contains the definitions of the library's functions
- ```<driver>.list``` contains the human-readable assembly code generated from the library code
- ```main.bin```, ```main.elf```, ```main.hex``` files that contain a program to demonstrate the usage of the library, which can be flashed onto the microcontroller
- ```main.map``` contains symbol and section information about the library

Declarations of the functions can be found in the ```Inc/<driver>.h``` file, and definitions can be found in the ```build/<driver>.o``` file.

To upload the demo program into your microcontroller, simply connect it to your PC via and STLink and run the following command in the root directory of the driver -

```bash
make flash
```

To change the debugger from an STLink to a JLink, simply change the ```stlink.cfg``` to ```jlink.cfg``` under the ```flash``` target in the Makefile -

```make
flash:
	$(OPENOCD) \
		-f $(OPENOCD_SCRIPT_PATH)/interface/stlink.cfg \ # replace stlink with jlink
		-f $(OPENOCD_SCRIPT_PATH)/target/stm32f4x.cfg \
		-c "program $(BUILD_DIR)/main.hex reset exit"
```

## Documentation

The header files contain inline documentation for all structs, functions and enums within it. This repository uses the [Doxygen](https://www.doxygen.nl/) standard for inline-documentation.

## Further Resources

These are resources that were used during the development of these drivers.

- [ARM Cortex M4 Generic User Guide](https://developer.arm.com/documentation/dui0553/latest/)
- [Complete STM32F4 PDF Documentation](https://www.st.com/en/microcontrollers-microprocessors/stm32f401/documentation.html)
- [STM32F401xB/C Reference Manual Direct Download](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwia5OWa3L2BAxXMbmwGHdWVAgMQFnoECA8QAQ&url=https%3A%2F%2Fwww.st.com%2Fresource%2Fen%2Freference_manual%2Frm0368-stm32f401xbc-and-stm32f401xde-advanced-armbased-32bit-mcus-stmicroelectronics.pdf&usg=AOvVaw1a620YmZcu5nmOonec9iNV&opi=89978449)
- [STM32F401CCU6 Datasheet Direct Download](https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=&ved=2ahUKEwjc-bqV3b2BAxVtTWwGHVSmDWEQFnoECBIQAQ&url=https%3A%2F%2Fwww.st.com%2Fresource%2Fen%2Fdatasheet%2Fstm32f401cb.pdf&usg=AOvVaw0KSJRsQnGQF1xrXwfeEy5u&opi=89978449)
- [Black Pill V1.2 Guide](https://stm32-base.org/boards/STM32F401CCU6-WeAct-Black-Pill-V1.2.html)
