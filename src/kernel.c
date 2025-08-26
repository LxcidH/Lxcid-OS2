#include "multiboot.h"
#include "../drivers/terminal.h"
#include "../gdt/gdt.h"
#include "../idt/idt.h"
#include "../drivers/pic.h"

// Define global variables
volatile uint32_t* framebuffer;
uint32_t screen_width;
uint32_t screen_height;
uint32_t screen_pitch;
uint32_t white = 0x00FFFFFF;
uint32_t red   = 0x00FF0000;

void kmain(multiboot_info_t *mbi) {
    // Initialize core systems
    gdt_init();
    idt_init();
    pic_remap();
    init_graphics(mbi);



    volatile int a = 5;
    volatile int b = 0;
    volatile int c = a / b;
    puts(50, 50, "Hello, World!", white);

    // We no longer need a halt loop here, as the timer interrupt
    // will keep the system alive. An idle loop is better.
    for(;;) {
        asm volatile("hlt"); // Wait for the next interrupt
    }
}
