/* The entry point is the _start label in boot.asm */
ENTRY(_start)

SECTIONS
{
    /* Begin code at 1MB. */
    . = 1M;

    .text ALIGN(4K) :
    {
        *(.multiboot)
        *(.text)
    }

    .rodata ALIGN(4K) :
    {
        *(.rodata)
    }

    .data ALIGN(4K) :
    {
        *(.data)
    }

    .bss ALIGN(4K) :
    {
        *(.bss)
    }
}
