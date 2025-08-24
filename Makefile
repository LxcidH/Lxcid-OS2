# The C compiler (cross-compiler)
CC = i686-elf-gcc

# The assembler
AS = nasm

# --- CONFIGURATION ---
# List all directories containing source code here.
SRC_DIRS = src gdt

# --- DIRECTORIES ---
BUILD_DIR = build
ISO_DIR = iso
# The -p flag ensures that mkdir doesn't complain if the directory already exists.
$(shell mkdir -p $(BUILD_DIR) $(ISO_DIR))

# VPATH tells make where to look for source files listed below.
VPATH = $(SRC_DIRS)

# --- COMPILER AND LINKER FLAGS ---
# Use a foreach loop to add an include flag for each source directory.
CFLAGS = -m32 -ffreestanding -nostdlib -fno-pie -O2 -Wall -Wextra $(foreach dir,$(SRC_DIRS),-I$(dir))
ASFLAGS = -felf32
LDFLAGS = -m elf_i386

# --- SOURCE AND OBJECT FILES ---
# List the base names of your source files here. Make will find them using VPATH.
C_SOURCES = kernel.c gdt.c
ASM_SOURCES = boot.asm gdt_asm.asm

# Automatically generate the object file names by replacing the extension.
C_OBJECTS = $(C_SOURCES:.c=.o)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)

# Prepend the build directory path to all object file names.
OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_OBJECTS) $(ASM_OBJECTS))

# --- TARGETS ---
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = $(ISO_DIR)/os.iso

# Default target
all: $(KERNEL)

# Link the kernel
$(KERNEL): $(OBJECTS) linker.ld
	ld $(LDFLAGS) -T linker.ld -o $(KERNEL) $(OBJECTS)

# Generic rule to compile C files from any directory in VPATH into the build directory.
$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Generic rule to assemble assembly files from any directory in VPATH into the build directory.
$(BUILD_DIR)/%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR)

# Create a bootable ISO
iso: $(KERNEL) grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL) isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir
	rm -r isodir

# Run with QEMU
run: iso
	qemu-system-i386 -cdrom $(ISO)
