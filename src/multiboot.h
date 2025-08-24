#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

/**
 * The Multiboot Information structure.
 * The bootloader provides a pointer to this structure in the EBX register
 * when it calls the kernel's entry point.
 *
 * The '__attribute__((packed))' is crucial. It tells the compiler not to
 * add any padding between the members, ensuring the C struct's memory
 * layout exactly matches the structure defined in the Multiboot spec.
 */
typedef struct multiboot_info {
    // Indicates which fields below are valid.
    uint32_t flags;

    // --- Valid if bit 0 of 'flags' is set ---
    uint32_t mem_lower;
    uint32_t mem_upper;

    // --- Valid if bit 1 of 'flags' is set ---
    uint32_t boot_device;

    // --- Valid if bit 2 of 'flags' is set ---
    uint32_t cmdline;

    // --- Valid if bit 3 of 'flags' is set ---
    uint32_t mods_count;
    uint32_t mods_addr;

    // --- Valid if bit 4 or 5 of 'flags' is set ---
    uint32_t syms[4];

    // --- Valid if bit 6 of 'flags' is set ---
    uint32_t mmap_length;
    uint32_t mmap_addr;

    // --- Valid if bit 7 of 'flags' is set ---
    uint32_t drives_length;
    uint32_t drives_addr;

    // --- Valid if bit 8 of 'flags' is set ---
    uint32_t config_table;

    // --- Valid if bit 9 of 'flags' is set ---
    uint32_t boot_loader_name;

    // --- Valid if bit 10 of 'flags' is set ---
    uint32_t apm_table;

    // --- Valid if bit 11 of 'flags' is set ---
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    // --- Valid if bit 12 of 'flags' is set ---
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type; // 0 for indexed, 1 for RGB
    uint8_t  color_info[6];

} __attribute__((packed)) multiboot_info_t;

#endif // MULTIBOOT_H
