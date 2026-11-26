#include <sg_xpt.h>
#include "chaostgkinder/network_layout.h"

#ifdef NONMATCHING
#include <mdm_lifecycle.h>
#include <string.h>

/* the bounds of the game code's BSS and the SDK code's, which
 * config/chaostgkinder/bss.ld sets */
extern Uint8 kinder_bss_area[], kinder_bss_end[];
extern Uint8 kinder_sg_bss_area[];

static void clear_bss(Uint8 *start, const Uint8 *end)
{
    memset(start, 0, (unsigned long)end - (unsigned long)start);
}

void kinder_network_init_sections(void)
{
    clear_bss(kinder_bss_area, kinder_bss_end);
    clear_bss(kinder_sg_bss_area, kinder_sg_bss_end);

    /* the five entries of the SDK's constructor table that hold one, in the
     * table's order */
    init_dteapicc();
    init_dspdbcc();
    init_pacingcc();
    init_targetcc();
    init_RamDownlcc();
}

void kinder_network_fini_sections(void)
{
    end_dteapicc();
    end_pacingcc();
}
#else
/* the SDK's constructor and destructor tables, sections DSG_INIT_ and
 * DSG_END_. the game's own two tables are empty. both point at
 * kinder_empty_init_entries, the start of section PSG, the SDK's code.
 * kinder_bss_area is section B, and kinder_sg_bss_area section BSG */
extern network_initializer kinder_sg_init_entries[10];
extern network_initializer kinder_sg_fini_entries[4];
extern network_initializer kinder_empty_init_entries[];
extern Uint8 kinder_bss_area[0x4f6b8], kinder_sg_bss_area[0x23acc];

#pragma section ATA
network_initializer *kinder_sg_constructors = kinder_sg_init_entries;
Sint32 kinder_sg_ctor_bytes = sizeof(kinder_sg_init_entries);
network_initializer *kinder_sg_destructors = kinder_sg_fini_entries;
Sint32 kinder_sg_dtor_bytes = sizeof(kinder_sg_fini_entries);
network_initializer *kinder_constructors = kinder_empty_init_entries;
Sint32 kinder_ctor_bytes = 0;
network_initializer *kinder_destructors = kinder_empty_init_entries;
Sint32 kinder_dtor_bytes = 0;
void *kinder_bss = kinder_bss_area;
Sint32 kinder_bss_bytes = sizeof(kinder_bss_area);
void *kinder_sg_bss = kinder_sg_bss_area;
Sint32 kinder_sg_bss_bytes = sizeof(kinder_sg_bss_area);
#endif
