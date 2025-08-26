# The C compiler (cross-compiler)
CC = i686-elf-gcc

# The assembler
AS = nasm

# --- CONFIGURATION ---
# List all directories containing source code here.
SRC_DIRS = src gdt drivers idt io flanterm flanterm/flanterm_backends lib

# --- DIRECTORIES ---
BUILD_DIR = build
ISO_DIR = iso
$(shell mkdir -p $(BUILD_DIR) $(ISO_DIR))

# --- COMPILER AND LINKER FLAGS ---
# Use a foreach loop to add an include flag (-I) for each source directory.
CFLAGS = -g -m32 -ffreestanding -nostdlib -fno-pie -O2 -Wall -Wextra $(foreach dir,$(SRC_DIRS),-I$(dir))
ASFLAGS = -felf32
# Linker flags are now passed to the compiler during the linking stage.
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

# --- AUTOMATIC SOURCE FILE DISCOVERY ---
# Use a foreach loop with the wildcard function to find all .c and .asm files
# in all of the directories listed in SRC_DIRS.
C_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.asm))

# Automatically generate the object file paths by replacing the source directory
# and file extension with the build directory and .o extension.
C_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(C_SOURCES)))
ASM_OBJECTS = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(notdir $(ASM_SOURCES)))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# VPATH tells make where to look for the source files when it sees a target like 'build/kmain.o'.
VPATH = $(SRC_DIRS)

# --- TARGETS ---
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(ISO_DIR)/os.iso

all: $(KERNEL)

# Link the kernel using the C compiler as the front-end.
# This ensures that the correct version of libgcc is found and linked.
$(KERNEL): $(OBJECTS) linker.ld
	$(CC) -m32 $(LDFLAGS) -o $(KERNEL) $(OBJECTS) -lgcc

# Generic rule to compile any .c file found via VPATH into the build directory.
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Generic rule to assemble any .asm file found via VPATH into the build directory.
$(BUILD_DIR)/%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

iso: $(KERNEL) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir
	rm -r isodir

run: iso
	qemu-system-i386 -cdrom $(ISO)
