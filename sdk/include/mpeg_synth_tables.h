/* the MPEG layer II synthesis filter's coefficient tables, which
 * mpeg_synth_init moves in place */
#ifndef MPEG_SYNTH_TABLES_H
#define MPEG_SYNTH_TABLES_H
#include <mpeg_alloc.h>
#include <sg_xpt.h>

#define MPEG_SYNTH_PHASES 16
#define MPEG_SYNTH_OUTPUTS 64

/* mpeg_synth_init moves each table's coefficients up to their next 32-byte
 * boundary. spare is the room they move into */
struct mpeg_matrix_table {
    Float coefficients[MPEG_SYNTH_OUTPUTS][SBLIMIT];
    Uint8 spare[128];
};
struct mpeg_window_table {
    Float coefficients[SBLIMIT][MPEG_SYNTH_PHASES];
    Uint8 spare[128];
};
extern struct mpeg_matrix_table mpeg_matrix_store;
extern struct mpeg_window_table mpeg_window_store;

#endif
