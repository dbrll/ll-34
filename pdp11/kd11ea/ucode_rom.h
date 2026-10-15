/* Assemble one microinstruction from the twelve ROMs. */

#ifndef UCODE_ROM_H
#define UCODE_ROM_H

#include <stdint.h>
#include "roms/rom_images.h"

/* E98 is already in the right order. */
static inline uint8_t ucode_reverse_nibble(uint8_t value) {
    return (uint8_t)(((value & 0x01) << 3) |
                     ((value & 0x02) << 1) |
                     ((value & 0x04) >> 1) |
                     ((value & 0x08) >> 3));
}

static inline uint64_t ucode_read(uint16_t mpc) {
    uint64_t word = 0;

    word |= (uint64_t)ucode_reverse_nibble(m8266_e110_ucode_rom[mpc]) << 0;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e109_ucode_rom[mpc]) << 4;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e108_ucode_rom[mpc]) << 8;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e107_ucode_rom[mpc]) << 12;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e106_ucode_rom[mpc]) << 16;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e99_ucode_rom[mpc]) << 20;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e97_ucode_rom[mpc]) << 24;
    word |= (uint64_t)m8266_e98_ucode_rom[mpc] << 28;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e100_ucode_rom[mpc]) << 32;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e104_ucode_rom[mpc]) << 36;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e103_ucode_rom[mpc]) << 40;
    word |= (uint64_t)ucode_reverse_nibble(m8266_e105_ucode_rom[mpc]) << 44;

    return word;
}

#endif
