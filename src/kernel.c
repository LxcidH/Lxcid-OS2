#include "multiboot.h"
#include <stdint.h>
#include "../font/font.h"
volatile uint32_t* framebuffer;
uint32_t screen_width;
uint32_t screen_height;
uint32_t screen_pitch;

int abs(int n) {
    return (n > 0) ? n : -n;
}

/*
 * Initializes the global graphics variables from the multiboot structure.
 * This should be called once at the beginning of kmain before any drawing.
 */
void init_graphics(multiboot_info_t *mbi) {
    // Cast the 64-bit address to a 32-bit pointer for easy access.
    framebuffer = (volatile uint32_t*)((uintptr_t)mbi->framebuffer_addr);
    screen_width = mbi->framebuffer_width;
    screen_height = mbi->framebuffer_height;
    // Pitch is the number of bytes per row. We divide by 4 because we are
    // using a 32-bit (4-byte) pointer.
    screen_pitch = mbi->framebuffer_pitch / 4;
}

/*
 * Puts a single pixel on the screen at the specified coordinates.
 * This is the core drawing function for our kernel.
 */
void put_pixel(int x, int y, uint32_t color) {
    // Add a boundary check to prevent writing outside the screen's memory.
    // This is a crucial safety measure to prevent crashes.
    if (x >= 0 && x < screen_width && y >= 0 && y < screen_height) {
        // Calculate the linear memory offset for the pixel.
        // The formula is: y * (pixels per row) + x
        uint32_t offset = y * screen_pitch + x;
        framebuffer[offset] = color;
    }
}

void draw_filled_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            put_pixel(start_x + x, start_y + y, color);
        }
    }
}

/*
 * Draws a line from (x0, y0) to (x1, y1) using Bresenham's line algorithm.
 * This implementation is optimized to use only integer arithmetic.
 */
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    // Calculate the differences in x and y coordinates.
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);

    // Determine the direction of the line for both x and y.
    // sx will be 1 if the line goes right, -1 if it goes left.
    // sy will be 1 if the line goes down, -1 if it goes up.
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    // This is the core of the algorithm: the error accumulator.
    // It starts as the sum of the deltas.
    int err = dx + dy;
    int e2; // Temporary error variable

    // Loop indefinitely until we reach the end point.
    while (1) {
        // Draw the current pixel.
        put_pixel(x0, y0, color);

        // Check if we've reached the destination.
        if (x0 == x1 && y0 == y1) {
            break;
        }

        // e2 is the error multiplied by 2.
        e2 = 2 * err;

        // Check if we need to move in the x direction.
        if (e2 >= dy) {
            err += dy; // Update the error.
            x0 += sx;  // Move one step in the x direction.
        }

        // Check if we need to move in the y direction.
        if (e2 <= dx) {
            err += dx; // Update the error.
            y0 += sy;  // Move one step in the y direction.
        }
    }
}

/*
 * Draws a single character, using its specific vertical offset for perfect alignment.
 */
void putchar(int x, int y, char c, uint32_t color) {
    unsigned char uc = (unsigned char)c;
    unsigned char* glyph = font_data[uc];

    // Get the character's unique vertical offset from the baseline.
    int y_offset = font_y_offsets[uc];

    // The final drawing position for the top of the character's bitmap.
    int draw_y = y + y_offset;

    for (int cy = 0; cy < FONT_CHAR_HEIGHT; cy++) {
        for (int cx = 0; cx < FONT_CHAR_WIDTH; cx++) {
            int byte_index = cx / 8;
            int bit_index = 7 - (cx % 8);
            unsigned char byte = glyph[cy * FONT_BYTES_PER_ROW + byte_index];

            if ((byte >> bit_index) & 1) {
                // Draw the pixel relative to the character's specific top-left corner.
                put_pixel(x + cx, draw_y + cy, color);
            }
        }
    }
}

void puts(int x, int y, char *str, uint32_t color) {
    int cx = x;

    for (int i = 0; str[i] != '\0'; i++) {
        putchar(cx, y, str[i], color);
        cx += FONT_CHAR_WIDTH;
    }
}

void kmain(multiboot_info_t *mbi) {
    // First, check if the bootloader provided valid framebuffer information.
    if (!(mbi->flags & (1 << 12))) {
        // If not, we can't draw, so halt.
        while(1) { asm volatile("hlt"); }
    }

    // Set up our global graphics variables.
    init_graphics(mbi);

    // Define the color white (0x00RRGGBB).
    uint32_t white = 0x00FFFFFF;
    uint32_t red = 0x00FF0000;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            put_pixel(i, j, red);
        }
    }

    draw_filled_rect(100, 100, 150, 50, red);
    draw_line(0, 0, 100, 100, red);

    puts(400, 400, "Hello, World!", white);
    while(1) {
        asm volatile("hlt");
    }
}
