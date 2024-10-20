LD=/usr/local/i386elfgcc/bin/i386-elf-ld
GCC=/usr/local/i386elfgcc/bin/i386-elf-gcc -m32 -ffreestanding -Wall -Wextra -fno-pie -nostdlib

BOOT_DIR=boot
BOOT_STAGE_DIR=$(BOOT_DIR)/stage
BUILD_DIR=build
PROGRAMS_DIR=programs

# Kernel Configuration
KERNEL_DIR=kernel
BUILD_KERNEL_DIR=build/kernel
BUILD_X86_LIB=build/x86_lib
X86_LIB_DIR=x86_lib

STAGE_C_SOURCE=$(shell find $(BOOT_STAGE_DIR) -name '*.c')
STAGE_C_OBJECTS=$(patsubst $(BOOT_STAGE_DIR)/%.c,$(BUILD_DIR)/%.C.o,$(STAGE_C_SOURCE))

X86_C_SOURCE=$(shell find $(X86_LIB_DIR) -name '*.c')
X86_ASM_SOURCE=$(shell find $(X86_LIB_DIR) -name '*.asm')

X86_LIB_DIR_MAKE=$(shell find $(X86_LIB_DIR) -type d)

X86_C_OBJECTS=$(patsubst $(X86_LIB_DIR)/%.c,$(BUILD_X86_LIB)/%.C.o,$(X86_C_SOURCE))
X86_ASM_OBJECTS=$(patsubst $(X86_LIB_DIR)/%.asm,$(BUILD_X86_LIB)/%.ASM.o,$(X86_ASM_SOURCE))
X86_LIB_DIR_MAKER=$(patsubst %, $(BUILD_DIR)/%, $(X86_LIB_DIR_MAKE))

build: mkdir $(BUILD_DIR)/BOOT.BIN $(BUILD_DIR)/OS.O $(BUILD_DIR)/DISK.O $(BUILD_DIR)/IOS.O $(STAGE_C_OBJECTS) build_kernel
	make build_program
	echo "Build OS Stage"
	$(LD) -m elf_i386 -T $(BOOT_STAGE_DIR)/linker.ld -o $(BUILD_DIR)/OSSTAGE.BIN $(BUILD_DIR)/OS.O $(BUILD_DIR)/IOS.O $(STAGE_C_OBJECTS) --oformat binary
	echo "Building MYOS.IMG, MYOS.disk"
	dd if=/dev/zero of=MYOS.IMG bs=512 count=102400
	mkfs.fat --offset=16 -F 32 -n "MYOS  DRIVE" MYOS.IMG
	dd if=$(BUILD_DIR)/BOOT.BIN of=MYOS.IMG conv=notrunc
	dd if=$(BUILD_DIR)/OSSTAGE.BIN of=MYOS.IMG bs=512 seek=2 conv=notrunc
	mcopy -i MYOS.IMG@@8192 $(BUILD_DIR)/KERNEL.BIN "::kernel.bin"
	mmd -i MYOS.IMG@@8192 "::/home"
	mcopy -i MYOS.IMG@@8192 file.txt "::file1.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file2.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file3.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file4.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file5.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file6.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file7.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file8.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file9.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file10.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file12.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::file13.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::abcdefghijklmnopqrstuvwxyz.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::abcdefhelloworld.txt"
	mcopy -i MYOS.IMG@@8192 file.txt "::/home/file14.txt"
	mcopy -i MYOS.IMG@@8192 $(BUILD_DIR)/program.bin "::prog.bin"
#	dd if=MYOS.DISK of=MYOS.IMG bs=512 seek=16 conv=notrunc

$(BUILD_DIR)/BOOT.BIN: $(BOOT_DIR)/boot.asm
	nasm -f bin -o $@ $^

$(BUILD_DIR)/OS.O: $(BOOT_STAGE_DIR)/os.asm
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/IOS.O: $(BOOT_STAGE_DIR)/ios.asm
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/DISK.O: $(BOOT_STAGE_DIR)/disk.asm
	nasm -f elf32 -o $@ $^

$(BUILD_DIR)/%.C.o: $(BOOT_STAGE_DIR)/%.c
	$(GCC) -o $@ -c $<

run:
	echo "Start Emulator"
# if you use hyprland and not using workspace 2
#	hyprctl dispatch workspace 2
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
	$(LD) -m elf_i386 -T $(KERNEL_DIR)/linker.ld -o $(BUILD_DIR)/KERNEL.BIN $(BUILD_KERNEL_DIR)/ENTRY_KERNEL.o $(BUILD_KERNEL_DIR)/KERNEL.o $(X86_ASM_OBJECTS) $(X86_C_OBJECTS) --oformat binary

KERNEL_BUILD: $(KERNEL_DIR)/kernel.c
	$(GCC) -o $(BUILD_KERNEL_DIR)/KERNEL.o -c $< -I"x86_lib"
	
ENTRY_KERNEL_BUILD: $(KERNEL_DIR)/entry_kernel.asm
	nasm -f elf32 -o $(BUILD_KERNEL_DIR)/ENTRY_KERNEL.o $<

$(BUILD_X86_LIB)/%.C.o: $(X86_LIB_DIR)/%.c
	$(GCC) -o $@ -c $< -I"x86_lib"

$(BUILD_X86_LIB)/%.ASM.o: $(X86_LIB_DIR)/%.asm
	nasm -f elf32 -o $@ $<

build_program:
	nasm -f elf32 $(PROGRAMS_DIR)/main.asm -o $(BUILD_DIR)/program_asm.o
	$(GCC) -c $(PROGRAMS_DIR)/main.c -o $(BUILD_DIR)/program_c.o -I"x86_lib"
	$(LD) -o $(BUILD_DIR)/program.bin $(BUILD_DIR)/program_c.o $(BUILD_DIR)/program_asm.o --oformat elf32-i386
	
