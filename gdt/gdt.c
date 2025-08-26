#include "gdt.h"

// By declaring the GDT and its pointer as 'static volatile', we give the
// compiler two instructions:
// 1. 'static': Limit the visibility of these variables to this file only.
// 2. 'volatile': This is the crucial part. It tells the compiler that these
//    variables can be changed by means outside the program's control (like
//    the CPU itself). This forces the compiler to write every change to
//    memory immediately and prevents aggressive optimizations that could
//    corrupt our setup.
static volatile gdt_entry_t gdt_entries[GDT_ENTRIES];
static volatile gdt_ptr_t   gdt_ptr;

// Function to set up a single GDT entry.
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// The main function to initialize the GDT.
void gdt_init() {
    // Set up the GDT pointer.
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    // 1. The mandatory NULL descriptor.
    gdt_set_gate(0, 0, 0, 0, 0);

    // 2. The Code Segment descriptor.
    // Base address: 0, Limit: 4 GBytes, Access: Ring 0, Granularity: 4 KByte blocks, 32-bit protected mode.
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    // 3. The Data Segment descriptor.
    // Same as code segment, but for data.
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Load the GDT into the CPU.
    gdt_flush((uint32_t)&gdt_ptr);
}
