/* Check the Metal Harbor head's advertised SET kinds through the resident
 * lookup. */
#include <assert.h>
#include <stddef.h>

#include "system/loader.h"
/* The host lookup uses the directory by name and needs no ELF boot section. */
#undef HEAD_SECTION
#define HEAD_SECTION
#include "stg10/head.c"

struct object_kind metal_harbor_kinds[METAL_HARBOR_KINDS];

int main(void)
{
    struct set_entry entry = {0};
    int i;

    assert(metal_harbor_objects.kinds == metal_harbor_kinds);
    assert(metal_harbor_objects.count == EXPECTED_COUNT);
    for (i = 0; i <= METAL_HARBOR_KINDS; i++) {
        struct object_kind *expected =
            i < EXPECTED_COUNT ? &metal_harbor_kinds[i] : NULL;
        entry.id = (Uint16)i;
        assert(set_entry_kind(&metal_harbor_objects, &entry) == expected);
    }
    return 0;
}
