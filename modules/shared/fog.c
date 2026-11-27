/* fog: a stage's fog settings, and the two tasks that apply them to Ninja's
 * fog at each step */
#include <ninja.h>
#include <sg_xpt.h>
#include <string.h>

#include "effect/particle.h"
#include "shared/fog.h"
#include "system/areas.h"
#include "system/game_state.h"
#include "system/loader.h"
#include "system/task.h"

/* the far depth fog_clear sets. njGenerateFogDensity gives every depth
 * above 65536 ninja.h's FOG_DENSITY_65536 */
#define CLEAR_FAR 100000.0F
/* the task levels of the fog task and of its manager. task_step_all leaves
 * level 6 out, and task_step_level_6 steps it */
#define FOG_TASK_LEVEL 6
#define MANAGER_LEVEL 1
/* the highest scroll position, in entries, just short of FOG_TABLE_SIZE */
#define SCROLL_END 127.99F
/* a mix above MIX_TO takes to's table as it is. one of MIX_FROM or less takes
 * from's, where from's speed is 0 */
#define MIX_TO 0.999F
#define MIX_FROM 0.001F

static NJS_FOG_TABLE fog_scroll_table;
static NJS_FOG_TABLE fog_blend_table;

/* nothing calls these two, and no header declares them. they are external
 * only because SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* set Ninja's fog color and density from setting, as fog_apply does */
void fog_apply_color_density(struct fog_setting *setting);
/* the fog task, or null */
struct task *fog_get_task(void);
/* NOLINTEND(misc-use-internal-linkage) */

/* the fog's state. its initializers place it in the data section, where the
 * image holds it, and not in bss */
static struct task *fog_task = NULL;
/* fog_from is the setting the fog task applies, or null for none. where
 * fog_to isn't null too, the task applies the blend of the two by fog_mix */
static struct fog_setting *fog_from = NULL;
static struct fog_setting *fog_to = NULL;
static Float fog_mix = 0.0F;
/* TRUE while a call or a task of the fog's reads or changes its state. a
 * step of the fog task that finds it TRUE applies nothing */
static Bool fog_busy = FALSE;
static struct task *fog_manager = NULL;

void fog_clear(struct fog_setting *setting)
{
    memset(setting, 0, sizeof(struct fog_setting));
    setting->far = CLEAR_FAR;
}

Bool fog_read(const char *name, struct fog_setting *setting)
{
#ifdef NONMATCHING
    /* Read through the resident scratch area because the GD-ROM transfers
     * whole sectors and a failed transfer can leave part of a sector there. */
    Sint32 bytes =
        loader_read_file_checked(name, WORK_AREA, WORK_AREA_END - WORK_AREA);
    if (bytes < (Sint32)sizeof(*setting)) {
        return FALSE;
    }
    memcpy(setting, WORK_AREA, sizeof(*setting));
    return TRUE;
#else
    if (loader_read_file(name, WORK_AREA) == 0) {
        memcpy(setting, WORK_AREA, sizeof(struct fog_setting));
        return TRUE;
    }
    return FALSE;
#endif
}

void fog_apply_color_density(struct fog_setting *setting)
{
    njSetFogColor(setting->color);
    njSetFogDensity(njGenerateFogDensity(-setting->far));
}

/* apply setting's color, density and table to Ninja's fog, with the table
 * scrolled as struct fog_setting says */
static void fog_apply(struct fog_setting *setting)
{
    /* @hack spelling: applied, a copy of setting, where setting read in
     * place would do */
    struct fog_setting *applied = setting;
    NJS_FOG_TABLE slopes;
    Float speed;
    Float pos;
    Float value;
    Float prev;
    Float cur;
    Float sum;
    Float *table;
    Int i;
    Int entry;
    Int next;
    Int shift;
    Int phase;

    /* @hack spelling: the color and density set in each branch, where once
     * before the test would do */
    if (applied->speed == 0.0F) {
        njSetFogColor(applied->color);
        njSetFogDensity(njGenerateFogDensity(-applied->far));
        njSetFogTable(applied->table);
    } else {
        njSetFogColor(applied->color);
        njSetFogDensity(njGenerateFogDensity(-applied->far));
        speed = applied->speed;
        if (speed < 0.0F) {
            speed = -speed;
        }
#ifdef NONMATCHING
        /* the original's runtime gives a remainder by 0 as 0, where GCC's
         * faults. STG21's fog has speed 2000 */
        phase = (Int)(FOG_TABLE_SIZE / speed);
        phase = phase != 0 ? game_state.steps % phase : 0;
#else
        phase = game_state.steps % (Int)(FOG_TABLE_SIZE / speed);
#endif
        if (applied->speed > 0.0F) {
            pos = phase * speed;
        } else {
            pos = (FOG_TABLE_SIZE - 1) - (phase * speed);
        }
        if (pos < 0.0F) {
            pos = 0.0F;
        }
        if (pos > SCROLL_END) {
            pos = SCROLL_END;
        }
        /* shift takes pos's whole part, and pos keeps the fraction */
        shift = njRoundOff(pos);
        pos -= shift;
        if (pos < 0.0F) {
            pos = 0.0F;
        }
        if (pos >= 1.0F) {
            pos = 1.0F;
        }
        /* @hack redundant: a test of applied that never runs */
        if (0) {
            if (applied) {
            }
        }
        if (applied->flags & FOG_SCROLL_ENTRIES) {
            /* @hack spelling: table, a local for applied's table, where
             * applied->table indexed would do */
            table = applied->table;
            for (i = 0; i < FOG_TABLE_SIZE; i++) {
                entry = (shift + i) % FOG_TABLE_SIZE;
                next = (shift + i + 1) % FOG_TABLE_SIZE;
                /* @bug logic: swaps the two entries' weights, giving
                 * table[entry] pos and table[next] 1 - pos (FMUL FR15,FR2 and
                 * FMAC FR0,FR3,FR2 at 0x1e2 and 0x1e4 of the object's code).
                 * as pos runs from 0 to 1, each value drifts one entry back,
                 * against the scroll. as shift then steps, the value jumps
                 * two entries ahead */
                value = (table[entry] * pos) + ((1.0F - pos) * table[next]);
                if (value > 1.0F) {
                    value = 1.0F;
                }
                /* @hack spelling: the lower bound as a conditional
                 * expression, where an if as for the upper would do */
                value = value < -1.0F ? -1.0F : value;
                /* @hack redundant: a test of value that never runs, with an
                 * empty else */
                if (0) {
                    if (value) {
                    } else {
                    }
                }
                fog_scroll_table[i] = value;
            }
        } else {
            prev = 0.0F;
            for (i = 0; i < FOG_TABLE_SIZE; i++) {
                cur = applied->table[FOG_TABLE_SIZE - 1 - i];
                slopes[i] = cur - prev;
                prev = cur;
            }
            sum = 0.0F;
            /* @hack spelling: i = i + 1, where i++ would do */
            for (i = 0; i < FOG_TABLE_SIZE; i = i + 1) {
                entry = (shift + i) % FOG_TABLE_SIZE;
                next = (shift + i + 1) % FOG_TABLE_SIZE;
                sum += (slopes[entry] * (1.0F - pos)) + (slopes[next] * pos);
                value = sum;
                /* @hack redundant: an empty else */
                if (value > 1.0F) {
                    value = 1.0F;
                } else {
                }
                /* @bug logic: passes njSetFogTable entries below 0, where a
                 * fog table's entries lie from 0 to 1. a sum of the scrolled
                 * slopes can fall below 0. value's lower bound is -1, not 0
                 * (FCMP/GT FR7,FR5 at 0x34e of the object's code).
                 * kmSetFogTable stores each entry as the byte
                 * (int)(entry * 255) & 0xff (AND R12,R13 at 0x8c12c0ce). so
                 * an entry from -1 to -1/255 fogs about as entry + 1 does */
                /* @hack spelling: the lower bound as a conditional
                 * expression, where an if as for the upper would do */
                value = value < -1.0F ? -1.0F : value;
                /* @hack type: the table's address taken whole and cast, where
                 * fog_scroll_table indexed would do */
                ((Float *)&fog_scroll_table)[FOG_TABLE_SIZE - 1 - i] = value;
            }
        }
        /* @hack type: the table's address taken whole and cast, where
         * fog_scroll_table would do */
        njSetFogTable((Float *)&fog_scroll_table);
    }
}

/* apply the blend of from and to by mix, as fog_set_blend says */
static void fog_apply_blend(struct fog_setting *from, struct fog_setting *to,
                            Float mix)
{
    NJS_FOG_TABLE slopes;
    Uint32 color;
    Uint32 density;
    Float speed;
    Float pos;
    Float value;
    Float target;
    Float prev;
    Float cur;
    Float sum;
    Float *table;
    Float *from_table;
    Float *to_table;
    Float *out;
    Int i;
    Int entry;
    Int next;
    Int shift;
    Int whole;
    Int phase;

    if (mix > 1.0F) {
        mix = 1.0F;
    }
    if (mix < 0.0F) {
        mix = 0.0F;
    }
    /* @hack spelling: color and density, locals for the two results, where
     * each passed straight to its setter would do */
    color = color_blend(from->color, to->color, mix);
    density =
        njGenerateFogDensity(-(((to->far - from->far) * mix) + from->far));
    njSetFogColor(color);
    njSetFogDensity(density);
    if (mix > MIX_TO) {
        njSetFogTable(to->table);
        return;
    }
    /* @hack spelling: an else after the return, where the code after the if
     * would do */
    if (mix <= MIX_FROM && from->speed == 0.0F) {
        njSetFogTable(from->table);
        return;
    } else {
        if (from->speed == 0.0F) {
            /* @hack redundant: a test of pos that never runs */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (pos == 2.0F && pos == 2.0F) {
                }
            }
            /* @hack spelling: from_table, to_table and out stepped through
             * from's table, to's table and fog_blend_table, where the three
             * indexed by i would do */
            /* @hack spelling: from_table, to_table and out started in the
             * for, where statements before it would do */
            for (i = 0, from_table = from->table, to_table = to->table,
                /* @hack type: the table's address taken whole and cast, where
                 * fog_blend_table would do */
                 out = (Float *)&fog_blend_table;
                 i < FOG_TABLE_SIZE; i++) {
                value = *from_table++;
                target = *to_table++;
                /* @hack spelling: value set to the blend before the store,
                 * where the blend stored through out would do */
                value = value + ((target - value) * mix);
                *out++ = value;
            }
        } else {
            /* @hack spelling: setting, a copy of from, where from read in
             * place would do */
            struct fog_setting *setting = from;

            speed = setting->speed;
            if (speed < 0.0F) {
                speed = -speed;
            }
#ifdef NONMATCHING
            /* the original's runtime gives a remainder by 0 as 0, where
             * GCC's faults. STG21's fog has speed 2000 */
            phase = (Int)(FOG_TABLE_SIZE / speed);
            phase = phase != 0 ? game_state.steps % phase : 0;
#else
            phase = game_state.steps % (Int)(FOG_TABLE_SIZE / speed);
#endif
            if (setting->speed > 0.0F) {
                pos = phase * speed;
            } else {
                pos = (FOG_TABLE_SIZE - 1) - (phase * speed);
            }
            if (pos < 0.0F) {
                pos = 0.0F;
            }
            if (pos > SCROLL_END) {
                pos = SCROLL_END;
            }
            /* @hack spelling: the whole part through whole, where shift =
             * njRoundOff(pos) would do */
            whole = njRoundOff(pos);
            shift = whole;
            /* @hack spelling: pos = pos - shift, where pos -= shift would
             * do */
            pos = pos - shift;
            if (pos < 0.0F) {
                pos = 0.0F;
            }
            if (pos >= 1.0F) {
                pos = 1.0F;
            }
            /* @hack redundant: a test of setting and pos that never runs */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (setting && setting && setting && setting && pos) {
                }
            }
            if (setting->flags & FOG_SCROLL_ENTRIES) {
                /* @hack spelling: table and to_table for the two tables, where
                 * setting->table and to->table indexed would do */
                table = setting->table;
                to_table = to->table;
                for (i = 0; i < FOG_TABLE_SIZE; i++) {
                    entry = (shift + i) % FOG_TABLE_SIZE;
                    next = (shift + i + 1) % FOG_TABLE_SIZE;
                    /* @bug logic: swaps the two entries' weights, as
                     * fog_apply does (FMUL FR14,FR2 and FMAC FR0,FR3,FR2 at
                     * 0x610 and 0x612 of the object's code) */
                    value = (table[entry] * pos) + ((1.0F - pos) * table[next]);
                    target = *to_table++;
                    if (value > 1.0F) {
                        value = 1.0F;
                    }
                    if (value < -1.0F) {
                        value = -1.0F;
                    }
                    fog_blend_table[i] = value + ((target - value) * mix);
                }
            } else {
                /* @hack spelling: to_table stepped down through to's table,
                 * where to->table indexed as fog_blend_table is would do */
                to_table = &to->table[FOG_TABLE_SIZE - 1];
                prev = 0.0F;
                for (i = 0; i < FOG_TABLE_SIZE; i++) {
                    cur = setting->table[FOG_TABLE_SIZE - 1 - i];
                    slopes[i] = cur - prev;
                    prev = cur;
                }
                sum = 0.0F;
                for (i = 0; i < FOG_TABLE_SIZE; i++) {
                    entry = (shift + i) % FOG_TABLE_SIZE;
                    next = (shift + i + 1) % FOG_TABLE_SIZE;
                    sum +=
                        (slopes[entry] * (1.0F - pos)) + (slopes[next] * pos);
                    value = sum;
                    target = *to_table--;
                    if (value > 1.0F) {
                        value = 1.0F;
                    }
                    /* @bug logic: passes entries below 0 to njSetFogTable, as
                     * fog_apply does. a blended entry stays below 0 wherever
                     * target * mix is less than -value * (1 - mix) (FCMP/GT
                     * FR6,FR4 at 0x7be of the object's code) */
                    if (value < -1.0F) {
                        value = -1.0F;
                    }
                    fog_blend_table[FOG_TABLE_SIZE - 1 - i] =
                        value + ((target - value) * mix);
                }
            }
        }
        /* @hack type: the table's address taken whole and cast, where
         * fog_blend_table would do */
        njSetFogTable((Float *)&fog_blend_table);
    }
}

static void fog_step(struct task *task)
{
    if (fog_task != task || fog_busy) {
        return;
    }
    fog_busy = TRUE;
    if (fog_from != NULL) {
        if (fog_to != NULL) {
            fog_apply_blend(fog_from, fog_to, fog_mix);
        } else {
            fog_apply(fog_from);
        }
    }
    fog_busy = FALSE;
}

static void fog_destroy(struct task *task)
{
    if (task == fog_task) {
        fog_task = NULL;
    }
}

struct task *fog_get_task(void)
{
    return fog_task;
}

static void fog_manager_step(struct task *task)
{
    if (fog_task != NULL) {
        return;
    }
    fog_busy = TRUE;
    fog_task = task_create(0, FOG_TASK_LEVEL, fog_step, "FogTaskExec");
    if (fog_task != NULL) {
        fog_task->destructor = fog_destroy;
    }
    fog_busy = FALSE;
}

/* the manager's destructor, which destroys the fog task too */
static void fog_manager_destroy(struct task *task)
{
    fog_busy = TRUE;
    if (fog_task != NULL) {
        task_destroy(fog_task);
        fog_task = NULL;
    }
    if (fog_manager == task) {
        fog_manager = NULL;
    }
    fog_busy = FALSE;
}

void fog_stop(void)
{
    if (fog_manager != NULL) {
        task_destroy(fog_manager);
        fog_manager = NULL;
    }
}

void fog_set(struct fog_setting *setting)
{
    fog_busy = TRUE;
    fog_from = setting;
    fog_to = NULL;
    fog_busy = FALSE;
}

void fog_set_blend(struct fog_setting *from, struct fog_setting *to, Float mix)
{
    fog_busy = TRUE;
    fog_from = from;
    fog_to = to;
    fog_mix = mix;
    fog_busy = FALSE;
}

void fog_start(void)
{
    if (fog_manager != NULL) {
        return;
    }
    fog_busy = TRUE;
    fog_from = NULL;
    fog_to = NULL;
    njFogDisable();
    fog_manager =
        task_create(0, MANAGER_LEVEL, fog_manager_step, "FogtaskManExec");
    if (fog_manager != NULL) {
        fog_manager->destructor = fog_manager_destroy;
    }
    fog_busy = FALSE;
}

void fog_start_file(const char *name, struct fog_setting *setting)
{
    if (fog_manager != NULL) {
        return;
    }
    fog_busy = TRUE;
    fog_from = setting;
    fog_to = NULL;
    fog_clear(setting);
    fog_read(name, setting);
    njFogDisable();
    fog_manager =
        task_create(0, MANAGER_LEVEL, fog_manager_step, "FogtaskManExec");
    if (fog_manager != NULL) {
        fog_manager->destructor = fog_manager_destroy;
    }
    fog_busy = FALSE;
}
