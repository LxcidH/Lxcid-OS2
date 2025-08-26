#include "../drivers/terminal.h"
#include "idt.h"
__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
    puts(100, 100, "exception handled!", red);
    __asm__ volatile ("cli; hlt");

}

__attribute__((noreturn))
void div_by_zero() {
    puts(500, 500, "div by 0 fired", red);
    for (;;);
}

void test_handler() {
    puts(500, 500, "test handler fired", red);
    __asm__ volatile ("cli; hlt");
}

// --- Unified Interrupt Dispatcher ---
// This is the single C function called by the assembly stub. It decides whether
// the interrupt is a fault or an IRQ and calls the appropriate handler.
void c_interrupt_handler(struct registers* regs) {
    if (regs->int_no < 32) {
        // It's a CPU exception.
        // fault_handler(regs);
    } else if (regs->int_no == 128) {
        // It's a system call, so pass the registers to the syscall handler
        // syscall_handler(regs);
    } else {
/*         // It's a hardware interrupt (IRQ).
        irq_handler(regs);
        // We MUST send an End-of-Interrupt (EOI) to the PICs for IRQs.
        pic_send_eoi(regs->int_no - 32) */;
    }
}
