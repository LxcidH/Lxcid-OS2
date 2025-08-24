; Set the magic number and flags for the Multiboot header.
MBALIGN   equ  1 << 0
MEMINFO   equ  1 << 1
VMODE     equ  1 << 2
MBFLAGS   equ  MBALIGN | MEMINFO | VMODE
MAGIC     equ  0x1BADB002
CHECKSUM  equ  -(MAGIC + MBFLAGS)

; This is the Multiboot header. GRUB looks for this to know how to load the kernel.
section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM
    ; The following fields are set to 0 to tell GRUB to use the settings
    ; from its own config file (grub.cfg), which is the modern approach.
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB stack
stack_top:

section .text
global _start
extern kmain

_start:
    ; Set up the stack pointer.
    mov esp, stack_top

    ; CRITICAL FIX: The C calling convention (cdecl) expects arguments
    ; to be passed on the stack. GRUB provides the Multiboot info
    ; pointer in the EBX register. We must push it onto the stack
    ; before calling our C kernel's main function.
    push ebx

    ; Now call our C function.
    call kmain

    ; If kmain returns, halt the CPU.
    cli
.hang:
    hlt
    jmp .hang
