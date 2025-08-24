#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// --- Configuration ---
#define FONT_CHAR_WIDTH  21
#define FONT_CHAR_HEIGHT 40
#define FONT_BYTES_PER_ROW 3
#define FONT_FIRST_CHAR 0
#define FONT_NUM_CHARS 256

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <font.otf>\n", argv[0]);
        return 1;
    }

    FILE *fontFile = fopen(argv[1], "rb");
    if (!fontFile) { /* error */ return 1; }
    fseek(fontFile, 0, SEEK_END);
    long size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    unsigned char *otf_buffer = malloc(size);
    fread(otf_buffer, 1, size, fontFile);
    fclose(fontFile);

    stbtt_fontinfo font;
    if (!stbtt_InitFont(&font, otf_buffer, stbtt_GetFontOffsetForIndex(otf_buffer, 0))) { /* error */ return 1; }

    float scale = stbtt_ScaleForPixelHeight(&font, FONT_CHAR_HEIGHT);

    // --- Generate the C header file ---
    printf("#ifndef FONT_H\n#define FONT_H\n\n");
    printf("// Font data for '%s', rendered at %dx%d\n", argv[1], FONT_CHAR_WIDTH, FONT_CHAR_HEIGHT);
    printf("#define FONT_CHAR_WIDTH  %d\n", FONT_CHAR_WIDTH);
    printf("#define FONT_CHAR_HEIGHT %d\n", FONT_CHAR_HEIGHT);
    printf("#define FONT_BYTES_PER_ROW %d\n", FONT_BYTES_PER_ROW);
    printf("#define FONT_BYTES_PER_CHAR (%d * %d)\n", FONT_CHAR_HEIGHT, FONT_BYTES_PER_ROW);
    printf("#define FONT_NUM_CHARS %d\n\n", FONT_NUM_CHARS);

    // This array will hold the specific vertical offset for each character.
    int y_offsets[FONT_NUM_CHARS] = {0};

    // --- Generate the main font data array ---
    printf("unsigned char font_data[FONT_NUM_CHARS][FONT_BYTES_PER_CHAR] = {\n");
    for (int c = FONT_FIRST_CHAR; c < FONT_NUM_CHARS; c++) {
        printf("    /* Char %d */ {\n        ", c);

        unsigned char *bitmap = calloc(FONT_CHAR_WIDTH * FONT_CHAR_HEIGHT, 1);
        int glyph_index = stbtt_FindGlyphIndex(&font, c);

        if (glyph_index > 0) {
            // NEW: Get the glyph's specific bounding box.
            // iy0 is the vertical offset from the baseline to the top of the bitmap.
            int ix0, iy0, ix1, iy1;
            stbtt_GetGlyphBitmapBox(&font, glyph_index, scale, scale, &ix0, &iy0, &ix1, &iy1);
            y_offsets[c] = iy0; // Store the offset

            // Render the glyph into a temporary buffer at its original size
            int w = ix1 - ix0;
            int h = iy1 - iy0;
            unsigned char *glyph_bitmap = malloc(w * h);
            stbtt_MakeGlyphBitmap(&font, glyph_bitmap, w, h, w, scale, scale, glyph_index);

            // Copy the temporary bitmap onto our fixed-size 21x40 canvas
            for(int y = 0; y < h; y++) {
                for(int x = 0; x < w; x++) {
                    if (x < FONT_CHAR_WIDTH && y < FONT_CHAR_HEIGHT) {
                         bitmap[y * FONT_CHAR_WIDTH + x] = glyph_bitmap[y * w + x];
                    }
                }
            }
            free(glyph_bitmap);
        }

        // Convert the bitmap to our packed format (same as before)
        int byte_count = 0;
        for (int y = 0; y < FONT_CHAR_HEIGHT; y++) {
            unsigned char row_bytes[FONT_BYTES_PER_ROW] = {0};
            for (int x = 0; x < FONT_CHAR_WIDTH; x++) {
                if (bitmap[y * FONT_CHAR_WIDTH + x] > 127) {
                    int byte_index = x / 8;
                    int bit_index = 7 - (x % 8);
                    row_bytes[byte_index] |= (1 << bit_index);
                }
            }
            for (int i = 0; i < FONT_BYTES_PER_ROW; i++) {
                printf("0x%02x, ", row_bytes[i]);
                byte_count++;
                if (byte_count % 12 == 0 && byte_count < (FONT_CHAR_HEIGHT * FONT_BYTES_PER_ROW)) {
                    printf("\n        ");
                }
            }
        }
        free(bitmap);
        printf("\n    },\n");
    }
    printf("};\n\n");

    // --- Generate the vertical offsets array ---
    printf("int font_y_offsets[FONT_NUM_CHARS] = {\n    ");
    for(int i = 0; i < FONT_NUM_CHARS; i++) {
        printf("%d, ", y_offsets[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n    ");
        }
    }
    printf("\n};\n\n");

    printf("#endif // FONT_H\n");
    free(otf_buffer);
    return 0;
}
