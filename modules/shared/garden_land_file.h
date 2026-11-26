/* the GCC build's checks that a garden's land file holds what it points to.
 * chaostglobby/lobby.c and chaostgrace/scene.c load their land files through
 * P1. each file holds P0 pointers into its own bytes */
#ifndef SHARED_GARDEN_LAND_FILE_H
#define SHARED_GARDEN_LAND_FILE_H

#ifdef NONMATCHING
/* this side's checks of a land file. they test that a span lies inside the
 * file, and that the land's table and entries do */
#include <shinobi.h>
#include <stdint.h>
#include "collision/world.h"

/* whether data[0..bytes) lies within file[0..size), with data on a 4-byte
 * boundary. where file lies in P1, data below PHYSICAL_LIMIT counts as its P1
 * alias: the address ORed with P1_BASE */
static Bool garden_land_contains(const void *file, Uint32 size,
                                 const void *data, Uint32 bytes)
{
    uintptr_t base = (uintptr_t)file;
    uintptr_t address = (uintptr_t)data;

    if (address < PHYSICAL_LIMIT && base >= P1_BASE && base < P2_BASE) {
        address |= (uintptr_t)P1_BASE;
    }
    return address % sizeof(Uint32) == 0 && address >= base && bytes <= size &&
           address - base <= size - bytes;
}

/* whether land and its entries lie within file[0..size), as
 * garden_land_contains tells. it is FALSE too for a negative size, a count
 * below 1, or drawn above count */
static Bool garden_land_ready(const void *file, Sint32 size,
                              const struct land_table *land)
{
    if (size < 0 ||
        !garden_land_contains(file, (Uint32)size, land, sizeof(*land))) {
        return FALSE;
    }
    return land->count > 0 && land->drawn <= land->count &&
           garden_land_contains(file, (Uint32)size, land->entries,
                                (Uint32)land->count * sizeof(*land->entries));
}
#endif

#endif
