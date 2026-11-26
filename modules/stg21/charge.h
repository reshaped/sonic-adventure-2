/* DYNAMITE in Sand Ocean's object list: dynamite.h's charge, which
 * stg21/charge.c builds again under names of its own */
#ifndef STG21_CHARGE_H
#define STG21_CHARGE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

extern NJS_TEXLIST *ocean_charge_texlists[];
extern Sint32 ocean_charge_blasts;
/* as dynamite_build does */
Uint8 *ocean_charge_build(Uint8 *buf);
/* as dynamite_init, but it doesn't call blast_load_texture. a whole charge
 * whose placement has been stopped scores nothing as it ends */
void ocean_charge_init(struct task *task);
/* as dynamite_count does, from Sand Ocean's own table */
int ocean_charge_count(int id);

#endif
