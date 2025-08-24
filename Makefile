# The C compiler
CC = i686-elf-gcc

# The assembler
AS = nasm

# Directories
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
# The -p flag ensures that mkdir doesn't complain if the directory already exists.
$(shell mkdir -p $(BUILD_DIR) $(ISO_DIR))

# C compiler flags
# We add -I$(SRC_DIR) so the compiler can find any header files in your src directory.
CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -O2 -Wall -Wextra -I$(SRC_DIR)

# Assembler flags
ASFLAGS = -felf32

# Linker flags
LDFLAGS = -m elf_i386

# Source files
# Use wildcard to automatically find all .c and .asm files in the src directory.
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES = $(wildcard $(SRC_DIR)/*.asm)

# Object files
# This transforms paths like 'src/kernel.c' into 'build/kernel.o'.
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# The final kernel binary and ISO file paths
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(ISO_DIR)/os.iso

# Default target
all: $(KERNEL)

# Link the kernel
# The dependencies are the object files and the linker script.
$(KERNEL): $(ASM_OBJECTS) $(C_OBJECTS) linker.ld
	ld $(LDFLAGS) -T linker.ld -o $(KERNEL) $(ASM_OBJECTS) $(C_OBJECTS)

# Compile C sources
# This rule tells make how to build a .o file in build/ from a .c file in src/.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly sources
# This rule tells make how to build a .o file in build/ from a .asm file in src/.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	$(AS) $(ASFLAGS) $< -o $@

# Clean up build files by removing the build and iso directories
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

# Create a bootable ISO
iso: $(KERNEL) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	# THIS IS THE UPDATED COMMAND to create a hybrid ISO for USB booting
	grub-mkrescue -o $(ISO) isodir
	rm -r isodir

# Run with QEMU, using the ISO from the iso/ directory
run: iso
	qemu-system-i386 -cdrom $(ISO)
