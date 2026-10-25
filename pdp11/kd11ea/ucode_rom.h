/* Assemble one microinstruction from the twelve ROMs. */

#ifndef UCODE_ROM_H
#define UCODE_ROM_H

#include <stdint.h>
#include "roms/rom_images.h"

/* Control store, twelve 82S137 (K2-8). A0-A9: MPC 06 04 07 03 02 01 00 08
 * 05 09 (MPC 09 is 0). Micro-word bits n..n+3 are O4..O1, except on E98
 * (O1..O4). */

static inline uint16_t ucode_addr(uint16_t mpc) {
    return (uint16_t)((((mpc >> 6) & 1) << 0) | (((mpc >> 4) & 1) << 1) |
                      (((mpc >> 7) & 1) << 2) | (((mpc >> 3) & 1) << 3) |
                      (((mpc >> 2) & 1) << 4) | (((mpc >> 1) & 1) << 5) |
                      (((mpc >> 0) & 1) << 6) | (((mpc >> 8) & 1) << 7) |
                      (((mpc >> 5) & 1) << 8));
}

/* O4..O1 -> micro-word bits n..n+3 */
static inline uint8_t ucode_reverse_nibble(uint8_t value) {
    return (uint8_t)(((value & 0x01) << 3) |
                     ((value & 0x02) << 1) |
                     ((value & 0x04) >> 1) |
                     ((value & 0x08) >> 3));
}

static inline uint64_t ucode_read(uint16_t mpc) {
    uint16_t a = ucode_addr(mpc & 0x1FF);
    uint64_t word = 0;

    word |= (uint64_t)ucode_reverse_nibble(m8266_e110_ucode_rom[a]) << 0;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e109_ucode_rom[a]) << 4;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e108_ucode_rom[a]) << 8;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e107_ucode_rom[a]) << 12;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e106_ucode_rom[a]) << 16;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e99_ucode_rom[a]) << 20;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e97_ucode_rom[a]) << 24;
    word |= (uint64_t)m8266_e98_ucode_rom[a] << 28;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e100_ucode_rom[a]) << 32;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e104_ucode_rom[a]) << 36;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e103_ucode_rom[a]) << 40;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e105_ucode_rom[a]) << 44;

    return word;
}

#endif
