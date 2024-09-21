LD=/usr/local/i386elfgcc/bin/i386-elf-ld
GCC=/usr/local/i386elfgcc/bin/i386-elf-gcc -m32 -ffreestanding -Wall -Wextra -fno-pie -nostdlib

SRC_DIR=source
BUILD_DIR=build

# Kernel Configuration
KERNEL_DIR=kernel
BUILD_KERNEL_DIR=build/kernel
BUILD_X86_LIB=build/x86_lib
X86_LIB_DIR=x86_lib

STAGE_C_SOURCE=$(shell find $(SRC_DIR) -name '*.c')
STAGE_C_OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.C.o,$(STAGE_C_SOURCE))

X86_C_SOURCE=$(shell find $(X86_LIB_DIR) -name '*.c')
X86_ASM_SOURCE=$(shell find $(X86_LIB_DIR) -name '*.asm')

X86_LIB_DIR_MAKE=$(shell find $(X86_LIB_DIR) -type d)

X86_C_OBJECTS=$(patsubst $(X86_LIB_DIR)/%.c,$(BUILD_X86_LIB)/%.C.o,$(X86_C_SOURCE))
X86_ASM_OBJECTS=$(patsubst $(X86_LIB_DIR)/%.asm,$(BUILD_X86_LIB)/%.ASM.o,$(X86_ASM_SOURCE))
X86_LIB_DIR_MAKER=$(patsubst %, $(BUILD_DIR)/%, $(X86_LIB_DIR_MAKE))

build: mkdir $(BUILD_DIR)/BOOT.BIN $(BUILD_DIR)/OS.O $(BUILD_DIR)/DISK.O $(BUILD_DIR)/IOS.O $(STAGE_C_OBJECTS) build_kernel
	echo "Build OS Stage"
	$(LD) -m elf_i386 -T $(SRC_DIR)/LINKER.LD -o $(BUILD_DIR)/OSSTAGE.BIN $(BUILD_DIR)/OS.O $(BUILD_DIR)/IOS.O $(STAGE_C_OBJECTS) --oformat binary
	echo "Building MYOS.IMG, MYOS.disk"
	dd if=/dev/zero of=MYOS.IMG bs=512 count=102400
	dd if=/dev/zero of=MYOS.DISK bs=512 count=101730
	mkfs.fat -F 16 -n "MYOS  DRIVE" MYOS.DISK
	dd if=$(BUILD_DIR)/BOOT.BIN of=MYOS.IMG conv=notrunc
	dd if=$(BUILD_DIR)/OSSTAGE.BIN of=MYOS.IMG bs=512 seek=2 conv=notrunc
	mcopy -i MYOS.DISK $(BUILD_DIR)/KERNEL.BIN "::kernel.bin"
	dd if=MYOS.DISK of=MYOS.IMG bs=512 seek=30 conv=notrunc
	echo "Run MYOS"
#	dd if=/dev/zero of=OSDOS.DISK bs=512 count=101730
#	dd if=$(BUILD_DIR)/KERNEL.BIN of=OSDOS.IMG bs=512 seek=30 conv=notrunc

$(BUILD_DIR)/BOOT.BIN: $(SRC_DIR)/BOOT.ASM
	nasm -f bin -o $@ $^

$(BUILD_DIR)/OS.O: $(SRC_DIR)/OS.ASM
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/IOS.O: $(SRC_DIR)/IOS.ASM
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/DISK.O: $(SRC_DIR)/DISK.ASM
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/%.C.o: $(SRC_DIR)/%.c
	$(GCC) -o $@ -c $<

run:
	echo "Start Emulator"
	qemu-system-i386 -hda MYOS.IMG

mkdir: clean $(X86_LIB_DIR_MAKER)
	mkdir -p build/bin
	mkdir -p build/kernel

$(BUILD_DIR)/%: $(X86_LIB_DIR_MAKE)
	mkdir -p $@

clean:
	echo "Cleaning all build file"
	rm -rf MYOS.IMG
	rm -rf MYOS.DISK
	rm -rf build

build_kernel: ENTRY_KERNEL_BUILD KERNEL_BUILD $(X86_ASM_OBJECTS) $(X86_C_OBJECTS)
	echo "Build Kernel"
	$(LD) -m elf_i386 -T $(KERNEL_DIR)/KERNEL_LINKER.LD -o $(BUILD_DIR)/KERNEL.BIN $(BUILD_KERNEL_DIR)/ENTRY_KERNEL.o $(BUILD_KERNEL_DIR)/KERNEL.o $(X86_ASM_OBJECTS) $(X86_C_OBJECTS) --oformat binary

KERNEL_BUILD: $(KERNEL_DIR)/kernel.c
	$(GCC) -o $(BUILD_KERNEL_DIR)/KERNEL.o -c $< -I"x86_lib"
	
ENTRY_KERNEL_BUILD: $(KERNEL_DIR)/entry_kernel.asm
	nasm -f elf32 -o $(BUILD_KERNEL_DIR)/ENTRY_KERNEL.o $<

$(BUILD_X86_LIB)/%.C.o: $(X86_LIB_DIR)/%.c
	$(GCC) -o $@ -c $< -I"x86_lib"

$(BUILD_X86_LIB)/%.ASM.o: $(X86_LIB_DIR)/%.asm
	nasm -f elf32 -o $@ $<