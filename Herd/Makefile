# Phil@uberlab.org on 17/09/2014
# Updated to fit into a more standard Visual Studio 2013 Solution - Phil@uberlab.org on 10/01/2015
# Inspired from https://github.com/apmorton/teensy-template/blob/master/Makefile
# Built for NMake in the context of a Microsoft Visual Studio 2013 environment

# The name of your project (used to name the compiled .hex file)
TARGET = swarmAlpha

# The path to the project base directory
PROJECT_PATH = $(MAKEDIR)

# The path to the source directory
SOURCESPATH = $(PROJECT_PATH)\Sources

# The path to the objects (namely the teensy-coreALL.o)
OBJECTSPATH = $(PROJECT_PATH)\Objects

# The path to the numerous executables produced
BINSPATH = $(PROJECT_PATH)\Bin

# Base path location for all utilities and absolute path for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH = $(PROJECT_PATH)\Tools

# Absolute path to base directory for all header files
INCLUDEPATH = $(PROJECT_PATH)\Headers

# Path location for the arm-none-eabi compiler
COMPILERPATH = $(TOOLSPATH)\arm-none-eabi\bin

# The device used
DEVICE = MK20DX256

# The linker script used, should be the same as the device
LDSCRIPT = $(INCLUDEPATH)\ldscripts\mk20dx256.ld

# The frequency of the MCU clock
CLOCK_FREQUENCY = 96000000

# Needed options
OPTIONS = -DF_CPU=$(CLOCK_FREQUENCY) -DUSB_SERIAL -DLAYOUT_US_ENGLISH -D__$(DEVICE)__ -DTIME_T=1404214698

# Facultative options needed by many Arduino libraries to configure for Teensy 3.1
# Comment out if not needed
OPTIONS = $(OPTIONS) -DARDUINO=105 -DTEENSYDUINO=118

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS = -w -g -Os -Wall -ffunction-sections -fdata-sections -mcpu=cortex-m4 -mthumb -MMD $(OPTIONS)

# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti -fpermissive

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -T$(LDSCRIPT)

# additional libraries to link
LIBS = -lm


# names for the compiler programs
CC 		= $(COMPILERPATH)\arm-none-eabi-gcc.exe
CXX 	= $(COMPILERPATH)\arm-none-eabi-g++.exe
AS 		= $(COMPILERPATH)\arm-none-eabi-as.exe
OBJCOPY = $(COMPILERPATH)\arm-none-eabi-objcopy.exe
SIZE 	= $(COMPILERPATH)\arm-none-eabi-size.exe

# If you dont want to include every .c .cpp .h files, explicitly define source files.
# The list is not directory recursive! (GMake > NMake)
C_FILES      = *.c
CPP_FILES    = *.cpp
HEADER_FILES = *.h
OBJECT_FILES = *.o
OBJS = $(SOURCESPATH)\$(C_FILES) $(SOURCESPATH)\$(CPP_FILES) $(OBJECTSPATH)\$(OBJECT_FILES)
# $(SOURCESPATH)\$(CPP_FILES) $(SOURCESPATH)\$(HEADER_FILES)

all: $(TARGET).hex

$(TARGET).elf:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -I $(INCLUDEPATH) -o $(BINSPATH)\$@ $(OBJS) $(LIBS)

$(TARGET).hex:$(TARGET).elf
	$(SIZE) $(BINSPATH)\$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $(BINSPATH)\$(TARGET).elf $(BINSPATH)\$(TARGET).hex

clean:
	@echo Deleting *.o,*.d,*.elf,*.hex...
	@del /f /q $(BINSPATH)\*.o $(BINSPATH)\*.d $(BINSPATH)\*.elf $(BINSPATH)\*.hex

post_compile:
	@echo Telling the Teensy loader that we found a job...
	$(TOOLSPATH)\teensy_post_compile.exe -file=$(TARGET) -path=$(BINSPATH) -tools=$(TOOLSPATH)

reboot:
	@echo Rebooting Teensy board...
	$(TOOLSPATH)\teensy_reboot.exe

rebuild: clean all post_compile reboot

build: clean all

upload: post_compile reboot