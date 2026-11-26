/* the rings' lamps, in the order a pulse lights them. core_eggman.c lights each
 * list's lamps in turn, eggman_lamp_delay steps apart. eggman_outer_lamps_c
 * ends with outer_lamp_b_12, as eggman_outer_lamps_b does */
#include <ninja.h>

#include "stg35/machinery.h"
#include "stg35/ring_lamps.h"

NJS_CNK_OBJECT *eggman_inner_lamps_a[6] = {
#include "stg35/ring_sequences/eggman_inner_lamps_a.inc"
};

NJS_CNK_OBJECT *eggman_inner_lamps_b[6] = {
#include "stg35/ring_sequences/eggman_inner_lamps_b.inc"
};

NJS_CNK_OBJECT *eggman_outer_lamps_a[15] = {
#include "stg35/ring_sequences/eggman_outer_lamps_a.inc"
};

NJS_CNK_OBJECT *eggman_outer_lamps_b[12] = {
#include "stg35/ring_sequences/eggman_outer_lamps_b.inc"
};

NJS_CNK_OBJECT *eggman_outer_lamps_c[12] = {
#include "stg35/ring_sequences/eggman_outer_lamps_c.inc"
};

NJS_CNK_OBJECT *eggman_outer_lamps_d[15] = {
#include "stg35/ring_sequences/eggman_outer_lamps_d.inc"
};
