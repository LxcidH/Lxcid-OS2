#ifndef TERMINAL_H
#define TERMINAL_H

#include "../src/multiboot.h"
#include <stdint.h>

// --- Global Variable Declarations ---
// By using 'extern', we tell the compiler that these variables exist,
// but they are DEFINED in another file (like kmain.c). This prevents
// "multiple definition" errors during linking.
extern volatile uint32_t* framebuffer;
extern uint32_t screen_width;
extern uint32_t screen_height;
extern uint32_t screen_pitch;

extern uint32_t white;
extern uint32_t red;

// --- Function Prototypes ---
// All of your graphics and terminal functions should be declared here.
void init_graphics(multiboot_info_t *mbi);
void puts(int x, int y, char *str, uint32_t color);
void putchar(int x, int y, char c, uint32_t color);
void put_pixel(int x, int y, uint32_t color);
// Add any other drawing functions you have (e.g., draw_line) here.

#endif // TERMINAL_H
