/* the overlay's two BSS sections and its tables of static constructors and
 * destructors. the tables are the SDK code's and the game code's.
 * kinder_network_start
 * clears both sections and runs each constructor table, and
 * kinder_network_stop runs each destructor table. under NONMATCHING,
 * kinder_network_init_sections and kinder_network_fini_sections do that
 * work. the matching build gives each table and section by its address and
 * its size in bytes */
#ifndef CHAOSTGKINDER_NETWORK_LAYOUT_H
#define CHAOSTGKINDER_NETWORK_LAYOUT_H
#include <sg_xpt.h>

#ifdef NONMATCHING
/* the end of the SDK code's BSS */
extern Uint8 kinder_sg_bss_end[];
void kinder_network_init_sections(void);
void kinder_network_fini_sections(void);
#else
typedef void (*network_initializer)(void);
extern network_initializer *kinder_sg_constructors, *kinder_sg_destructors;
extern network_initializer *kinder_constructors, *kinder_destructors;
extern Sint32 kinder_sg_ctor_bytes, kinder_sg_dtor_bytes;
extern Sint32 kinder_ctor_bytes, kinder_dtor_bytes;
extern void *kinder_bss, *kinder_sg_bss;
extern Sint32 kinder_bss_bytes, kinder_sg_bss_bytes;
#endif
#endif
