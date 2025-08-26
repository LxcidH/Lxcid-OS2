#include "idt.h"
#include "isr.h"
#include <stdbool.h>
#include <stddef.h>

#define IDT_MAX_DESCRIPTORS 256

extern void default_handler(void);
extern void divide_by_zero_handler(void);

__attribute__((aligned(0x10)))
static idt_entry_t idt[256];
static idtr_t idtr;

static bool vectors[IDT_MAX_DESCRIPTORS];
extern void* isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_init(void);

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low         = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs       = 0x08;
    descriptor->attributes      = flags;
    descriptor->isr_high        = (uint32_t)isr >> 16;
    descriptor->reserved        = 0;
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint16_t vector = 0; vector < 256; vector++) {
        idt_set_descriptor(vector, (void*)default_handler, 0x8E);
        vectors[vector] = true;
    }

    idt_set_descriptor(0, (void*)div_by_zero, 0x8E);
    idt_set_descriptor(0x30, (void*)test_handler, 0x8E);

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // Load the new IDT
}

