/* particles: quads that move, turn, grow and age. a group's particles share
 * its texture list, draw flags and step. the particle system is a task that
 * steps and draws every group that holds a particle */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "draw/lines.h"
#include "effect/particle.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#ifdef NONMATCHING
/* texture_slot_loaded, by which this side's draws test a texture */
#include "system/texture.h"
#endif
#include "system/util.h"

#define CORE_LEVEL 1 /* the particle system's task level */

/* set in particle_draw_all's copy of a group's flags once the draw starts the
 * group's run of quads. the draw ends only a run it started */
#define DRAW_STARTED 0x80000000

#define FRONT_Z 0.001F /* the depth of a quad under PARTICLE_FRONT */
#define NEAR_Z 0.0001F /* the least depth at which a group's particle draws */

static void particle_step_all(void);
static void particle_draw_all(void);
static void particle_empty_groups(void);
static void particle_free_list(struct particle *particle);
static struct particle_group *particle_drop_group(struct particle_group *group);

/* no header declares these, and none is static. SHC leaves out a static
 * function that nothing calls, as nothing calls particle_forget_all or
 * particle_kill. it also leaves out one whose every call it expands in place.
 * it expands particle_core_destroy's call of particle_free_all */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* empty the list of groups without freeing a particle. a group that held
 * particles keeps them, unstepped and undrawn. it never rejoins the list,
 * since particle_create links only a group that holds none */
void particle_forget_all(void);
/* free the particles of every listed group, which then holds none, and empty
 * the list */
void particle_free_all(void);
/* have the system free particle at its next step, without drawing it again */
void particle_kill(struct particle *particle);
/* NOLINTEND(misc-use-internal-linkage) */

/* the groups that hold particles, linked by next, the newest first */
static struct particle_group *particle_groups = NULL;
/* the particle system's task, or null */
static struct task *particle_core = NULL;
/* what PARTICLE_TINT multiplies a color by. nothing changes it */
static Uint32 particle_tint = 0xffffffff;
/* the unit z vector. particle_draw_quad divides depth by its length once the
 * current matrix turns it */
static NJS_VECTOR particle_quad_z = {0.0F, 0.0F, 1.0F};
/* the same, for particle_draw_all's depth_bias */
static NJS_VECTOR particle_draw_z = {0.0F, 0.0F, 1.0F};
/* what particle_create copies for a group whose tmpl is null */
/* @hack type: an array of one, where an object would do */
/* clang-format off */
static struct particle particle_template[1] = {
    {0, 1.0F, {0.0F, 0.0F, 0.0F}, 0xffffffff, 0.0F, QUARTER_TURN, {0.0F, 0.0F, 0.0F}, 0, NULL, 0, 0.0F},
};
/* clang-format on */

static void particle_core_step(struct task *task)
{
    particle_step_all();
}

static void particle_core_draw(struct task *task)
{
    particle_draw_all();
}

static void particle_core_destroy(struct task *task)
{
    if (particle_core == task) {
        particle_core = NULL;
    }
    particle_free_all();
}

static void particle_core_init(struct task *task)
{
    particle_groups = NULL;
    task->draw = particle_core_draw;
    task->step = particle_core_step;
    task->destructor = particle_core_destroy;
}

void particle_core_start(void)
{
    if (particle_core == NULL) {
        particle_core =
            task_create(0, CORE_LEVEL, particle_core_init, "ParticleCoreTask");
    }
}

void particle_core_stop(void)
{
    if (particle_core != NULL) {
        task_destroy(particle_core);
        particle_core = NULL;
    }
}

/* @hack redundant: register on flags */
void particle_draw_quad(NJS_POINT3 *pos, Sint32 texid, Angle edge_angle,
                        Angle ang, Float half_width, Float half_height,
                        Uint32 color, register Sint32 flags, Float depth,
                        Float size_limit)
{
    NJS_POINT3 point;
    NJS_QUAD_TEXTURE_EX quad;
    NJS_VECTOR dir;
    Float z;
    /* @hack redundant: register on scale_x, scale_y, cx, cy, inv_z, sx, rx,
     * ry, dx, dy, cos_ang and sin_ang */
    register Float scale_x;
    register Float scale_y;
    register Float cx;
    register Float cy;
    register Float inv_z;
    register Float sx;
    register Float rx;
    register Float ry;
    register Float dx;
    register Float dy;
    register Float cos_ang;
    register Float sin_ang;
    /* @hack spelling: sy declared after the register locals, where beside z
     * would do */
    Float sy;

#ifdef NONMATCHING
    /* this side draws nothing with a texid the current list hasn't loaded */
    if (!texture_slot_loaded(_nj_current_texlist, (Uint32)texid)) {
        return;
    }
#endif
    njCalcPoint(NULL, pos, &point);
    z = point.z;
    /* @bug arithmetic: divides by a z of 0. the test returns only for a z
     * below 0, by FCMP/GT at 0x8c036628, so FDIV at 0x8c036762 makes inv_z
     * infinite. the quad then takes infinite or not-a-number coordinates. a
     * size_limit above 0 returns first at the size test, but only where
     * half_width and half_height aren't 0 */
    if (z < 0.0F) {
        return;
    }
    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    cx = _nj_screen_.cx;
    cy = _nj_screen_.cy;
    /* @hack redundant: the store of 0 to quad.v, which both arms overwrite */
    quad.v = 0.0F;
    if (flags & PARTICLE_FLIP_U) {
        quad.u = 1.0F;
        quad.vu1 = -1.0F;
    } else {
        quad.u = 0.0F;
        quad.vu1 = 1.0F;
    }
    quad.vu2 = 0.0F;
    /* @hack spelling: quad.vv1 set to 0 in each arm, where once before the if
     * would do */
    if (flags & PARTICLE_FLIP_V) {
        quad.v = 0.0F;
        quad.vv1 = 0.0F;
        quad.vv2 = 1.0F;
    } else {
        quad.v = 1.0F;
        quad.vv1 = 0.0F;
        quad.vv2 = -1.0F;
    }
    njQuadTextureStart((flags & PARTICLE_OPAQUE) == 0);
    njSetQuadTexture(texid, color);
    if (depth != 0.0F) {
        if (flags & PARTICLE_DEPTH_Z) {
            point.z = depth;
        } else {
            njCalcVector(NULL, &particle_quad_z, &dir);
            point.z -= depth / njScalor(&dir);
        }
    }
    if (flags & PARTICLE_FRONT) {
        point.z = FRONT_Z;
    }
    /* @hack spelling: inv_z computed in each arm, where once before the if
     * would do */
    if (ang != 0 || edge_angle != QUARTER_TURN) {
        inv_z = 1.0F / z;
        rx = half_width * scale_x * inv_z;
        ry = -(half_height * scale_y * inv_z);
        if (size_limit > 0.0F && _builtin_fabsf(rx * ry) > size_limit) {
            /* the return skips njQuadTextureEnd, so the run started above
             * draws nothing */
            return;
        }
        rx *= 2.0F;
        ry *= 2.0F;
        /* @hack spelling: two arms that repeat the edges and quad.z, quad.x and
         * quad.y, where one arm that adds the second edge to dx and dy without
         * PARTICLE_BASE would do */
        if (flags & PARTICLE_BASE) {
            if (edge_angle == QUARTER_TURN) {
                cos_ang = njCos(ang);
                sin_ang = njSin(ang);
                dx = quad.vx1 = rx * cos_ang;
                quad.vx2 = -(rx * sin_ang);
                dy = quad.vy1 = ry * sin_ang;
                quad.vy2 = ry * cos_ang;
            } else {
                if (ang != 0) {
                    dx = quad.vx1 = rx * njCos(ang);
                    dy = quad.vy1 = ry * njSin(ang);
                } else {
                    dx = quad.vx1 = rx;
                    dy = quad.vy1 = 0.0F;
                }
                quad.vx2 = rx * njCos(edge_angle + ang);
                quad.vy2 = ry * njSin(edge_angle + ang);
            }
            quad.z = 1.0F / point.z;
            quad.x = cx + (point.x * inv_z * scale_x) - (dx * 0.5F);
            quad.y = cy + (point.y * inv_z * scale_y) - (dy * 0.5F);
        } else {
            if (edge_angle == QUARTER_TURN) {
                cos_ang = njCos(ang);
                sin_ang = njSin(ang);
                dx = quad.vx1 = rx * cos_ang;
                dx += quad.vx2 = -(rx * sin_ang);
                dy = quad.vy1 = ry * sin_ang;
                dy += quad.vy2 = ry * cos_ang;
            } else {
                if (ang != 0) {
                    dx = quad.vx1 = rx * njCos(ang);
                    dy = quad.vy1 = ry * njSin(ang);
                } else {
                    dx = quad.vx1 = rx;
                    dy = quad.vy1 = 0.0F;
                }
                dx += quad.vx2 = rx * njCos(edge_angle + ang);
                dy += quad.vy2 = ry * njSin(edge_angle + ang);
            }
            quad.z = 1.0F / point.z;
            quad.x = cx + (point.x * inv_z * scale_x) - (dx * 0.5F);
            quad.y = cy + (point.y * inv_z * scale_y) - (dy * 0.5F);
        }
    } else {
        inv_z = 1.0F / z;
        /* @hack spelling: sx and sy scaled in steps, half_height first, where
         * sx = half_width * scale_x * inv_z and sy = -(half_height * scale_y *
         * inv_z) would do */
        sy = half_height * scale_y;
        sx = half_width * scale_x;
        sx *= inv_z;
        sy *= inv_z;
        sy = -sy;
        if (size_limit > 0.0F && _builtin_fabsf(sx * sy) > size_limit) {
            /* the return skips njQuadTextureEnd, so the run started above
             * draws nothing */
            return;
        }
        quad.x = cx + (point.x * inv_z * scale_x) - sx;
        quad.y = cy + (point.y * inv_z * scale_y) -
                 ((flags & PARTICLE_BASE) ? 0.0F : sy);
        sx *= 2.0F;
        sy *= 2.0F;
        quad.z = 1.0F / point.z;
        quad.vx1 = sx;
        quad.vy1 = 0.0F;
        quad.vx2 = 0.0F;
        quad.vy2 = sy;
    }
    njDrawQuadTextureEx(&quad);
    njQuadTextureEnd();
}

Uint32 color_blend(Uint32 from, Uint32 to, Float mix)
{
    Float a2;
    Float r2;
    Float g2;
    Float b2;
    Float a1;
    Float r1;
    Float g1;
    Float b1;
    int alpha;
    int red;
    int green;
    int blue;
    Uint32 color;

    a2 = (to >> 24) & 0xff;
    r2 = (to >> 16) & 0xff;
    g2 = (to >> 8) & 0xff;
    b2 = to & 0xff;
    a1 = (from >> 24) & 0xff;
    r1 = (from >> 16) & 0xff;
    g1 = (from >> 8) & 0xff;
    b1 = from & 0xff;
    alpha = a1 + ((a2 - a1) * mix);
    red = r1 + ((r2 - r1) * mix);
    green = g1 + ((g2 - g1) * mix);
    blue = b1 + ((b2 - b1) * mix);
    color = 0;
    if (alpha & 0xffffff00) {
        color |= 0xff000000;
    } else {
        color |= (Uint32)(alpha & 0xff) << 24;
    }
    if (red & 0xffffff00) {
        color |= 0xff0000;
    } else {
        color |= (red & 0xff) << 16;
    }
    if (green & 0xffffff00) {
        color |= 0xff00;
    } else {
        color |= (green & 0xff) << 8;
    }
    if (blue & 0xffffff00) {
        color |= 0xff;
    } else {
        color |= blue & 0xff;
    }
    return color;
}

Uint32 color_multiply(Uint32 first, Uint32 second)
{
    Float a2;
    Float r2;
    Float g2;
    Float b2;
    Float a1;
    Float r1;
    Float g1;
    Float b1;
    Float alpha;
    Float red;
    Float green;
    Float blue;
    Uint32 color;

    a2 = ((second >> 24) & 0xff) * (1.0F / 255.0F);
    r2 = ((second >> 16) & 0xff) * (1.0F / 255.0F);
    g2 = ((second >> 8) & 0xff) * (1.0F / 255.0F);
    b2 = (second & 0xff) * (1.0F / 255.0F);
    a1 = ((first >> 24) & 0xff) * (1.0F / 255.0F);
    r1 = ((first >> 16) & 0xff) * (1.0F / 255.0F);
    g1 = ((first >> 8) & 0xff) * (1.0F / 255.0F);
    b1 = (first & 0xff) * (1.0F / 255.0F);
    alpha = a2 * a1;
    red = r2 * r1;
    green = g2 * g1;
    blue = b2 * b1;
    color = 0;
    if (alpha >= 1.0F) {
        color |= 0xff000000;
    } else {
        color |= (Uint32)(alpha * 255.0F) << 24;
    }
    if (red >= 1.0F) {
        color |= 0xff0000;
    } else {
        color |= (Uint32)(red * 255.0F) << 16;
    }
    if (green >= 1.0F) {
        color |= 0xff00;
    } else {
        color |= (Uint32)(green * 255.0F) << 8;
    }
    if (blue >= 1.0F) {
        color |= 0xff;
    } else {
        color |= (Uint32)(blue * 255.0F);
    }
    return color;
}

void particle_forget_all(void)
{
    particle_groups = NULL;
}

void particle_free_all(void)
{
    particle_empty_groups();
}

void particle_kill(struct particle *particle)
{
    particle->frame = -1.0F;
}

static void particle_step_all(void)
{
    struct particle_group *group;
    struct particle_group *prev_group;
    int (*step)(struct particle_group *group, struct particle *particle);
    struct particle *particle;
    struct particle *prev;
    struct particle *next;

    group = particle_groups;
    prev_group = NULL;
    /* @hack spelling: a loop of its own for a time stop, where one loop that
     * tests game_state.time_stopped at each particle would do, while no step
     * changes it */
    if (game_state.time_stopped != 0) {
        while (group != NULL) {
            step = group->step;
            prev = NULL;
            particle = group->particles;
            while (particle != NULL) {
                if (particle->frame < 0.0F ||
                    ((particle->flags & PARTICLE_MADE_IN_TIME_STOP) &&
                     !step(group, particle))) {
                    next = particle->next;
                    if (prev == NULL) {
                        group->particles = next;
                    } else {
                        prev->next = next;
                    }
                    syFree(particle);
                    particle = next;
                } else {
                    prev = particle;
                    particle = particle->next;
                }
            }
            if (group->particles != NULL) {
                prev_group = group;
            } else if (prev_group != NULL) {
                prev_group->next = group->next;
            } else {
                particle_groups = group->next;
            }
            group = group->next;
        }
    } else {
        while (group != NULL) {
            step = group->step;
            prev = NULL;
            particle = group->particles;
            while (particle != NULL) {
                if (particle->frame < 0.0F || !step(group, particle)) {
                    next = particle->next;
                    if (prev == NULL) {
                        group->particles = next;
                    } else {
                        prev->next = next;
                    }
                    syFree(particle);
                    particle = next;
                } else {
                    prev = particle;
                    particle = particle->next;
                }
            }
            if (group->particles != NULL) {
                prev_group = group;
            } else if (prev_group != NULL) {
                prev_group->next = group->next;
            } else {
                particle_groups = group->next;
            }
            group = group->next;
        }
    }
}

static void particle_draw_all(void)
{
    NJS_VECTOR point;
    NJS_QUAD_TEXTURE_EX quad;
    Sint32 last_frame;
    Uint32 last_color;
    Float scale_x;
    Float scale_y;
    Float cx;
    Float cy;
    Float depth_scale;
    Float z;
    Float inv_z;
    Float sx;
    Float sy;
    Float cos_ang;
    /* @hack redundant: register on group, particle, flags, frame, color,
     * scale, dx, dy and sin_ang */
    register struct particle_group *group;
    register struct particle *particle;
    register Uint32 flags;
    register Sint32 frame;
    register Uint32 color;
    register Float scale;
    register Float dx;
    register Float dy;
    register Float sin_ang;

    /* @hack spelling: group set before the screen's reads, where the for's
     * first clause would do */
    group = particle_groups;
    scale_x = _nj_screen_.dist * _nj_scr_aspect_x_;
    scale_y = _nj_screen_.dist * _nj_scr_aspect_y_;
    /* @hack spelling: cx and cy set in one statement, where two would do */
    cx = _nj_screen_.cx, cy = _nj_screen_.cy;
    /* @hack spelling: the turned z axis kept in point, where a vector of its
     * own would do */
    njCalcVector(NULL, &particle_draw_z, &point);
    depth_scale = 1.0F / njScalor(&point);
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_OFFSET_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    njFogDisable();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    for (; group != NULL; group = group->next) {
        particle = group->particles;
        if (particle == NULL) {
            continue;
        }
        flags = group->flags & ~DRAW_STARTED;
        if (group->size_limit <= 0.0F) {
            flags |= PARTICLE_ANY_SIZE;
        }
        switch (flags & PARTICLE_BLEND) {
        case PARTICLE_BLEND_ADD:
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
            njFogDisable();
            break;
        case PARTICLE_BLEND_ALPHA:
            njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
            njColorBlendingMode(NJD_DESTINATION_COLOR,
                                NJD_COLOR_BLENDING_INVSRCALPHA);
            njFogEnable();
            break;
        default:
            njFogDisable();
            break;
        }
        last_frame = -1;
        last_color = -1;
        /* @hack redundant: the store of 0 to quad.v, which both arms
         * overwrite */
        quad.v = 0.0F;
        if (flags & PARTICLE_FLIP_U) {
            quad.u = 1.0F;
            quad.vu1 = -1.0F;
        } else {
            quad.u = 0.0F;
            quad.vu1 = 1.0F;
        }
        quad.vu2 = 0.0F;
        /* @hack spelling: quad.vv1 set to 0 in each arm, where once before the
         * if would do */
        if (flags & PARTICLE_FLIP_V) {
            quad.v = 0.0F;
            quad.vv1 = 0.0F;
            quad.vv2 = 1.0F;
        } else {
            quad.v = 1.0F;
            quad.vv1 = 0.0F;
            quad.vv2 = -1.0F;
        }
        for (; particle != NULL; particle = particle->next) {
            njCalcPoint(NULL, &particle->pos, &point);
            z = point.z;
            if ((flags & PARTICLE_DEPTH) && particle->depth_bias != 0.0F) {
                z -= particle->depth_bias * depth_scale;
            }
            if (flags & PARTICLE_FRONT) {
                z = FRONT_Z;
            }
            if (z < NEAR_Z) {
                continue;
            }
            /* @hack spelling: z inverted in place, then copied to quad.z, where
             * quad.z = 1.0F / z would do */
            z = 1.0F / z;
            quad.z = z;
            /* @bug arithmetic: divides by a point.z of 0. the test above
             * bounds z, not point.z, so under PARTICLE_FRONT or a depth_bias
             * below 0 a point.z of 0 passes FCMP/GT at 0x8c037160. FDIV at
             * 0x8c03717a then makes inv_z infinite, and the quad's place
             * infinite or not a number */
            inv_z = 1.0F / point.z;
            quad.x = cx + (point.x * scale_x * inv_z);
            quad.y = cy + (point.y * scale_y * inv_z);
#ifdef NONMATCHING
            /* The whole part is the signed Ninja texture index. Check the
             * conversion range first; fractions above -1 still name row 0. */
            if (!(particle->frame > -1.0F && particle->frame < 2147483648.0F)) {
                continue;
            }
#endif
            frame = particle->frame;
            color = particle->color;
#ifdef NONMATCHING
            /* this side skips a frame whose texture the group's list hasn't
             * loaded. the test above leaves frame at 0 or more */
            if (!texture_slot_loaded(group->texlist, (Uint32)frame)) {
                continue;
            }
#else
            if (frame < 0) {
                continue;
            }
#endif
            if (flags & PARTICLE_TINT) {
                color = color_multiply(color, particle_tint);
            }
            if (last_frame != frame || color != last_color) {
                if (!(flags & DRAW_STARTED)) {
                    njSetTexture(group->texlist);
                    njQuadTextureStart((flags & PARTICLE_OPAQUE) == 0);
                    flags |= DRAW_STARTED;
                }
                last_color = color;
                last_frame = frame;
                njSetQuadTexture(frame, color);
            }
            /* @hack spelling: the scale and the size test in each arm, where
             * once before the if would do */
            if (particle->ang != 0 || particle->edge_angle != QUARTER_TURN) {
                scale = particle->scale * quad.z;
                sx = scale_x * scale;
                sy = -(scale_y * scale);
                if (!(flags & PARTICLE_ANY_SIZE) &&
                    group->size_limit < _builtin_fabsf(sx * sy)) {
                    continue;
                }
                sx *= 2.0F;
                sy *= 2.0F;
                /* @hack spelling: two arms that repeat the edges, where one arm
                 * that adds the second edge to dx and dy without PARTICLE_BASE
                 * would do */
                if (flags & PARTICLE_BASE) {
                    if (particle->edge_angle == QUARTER_TURN) {
                        cos_ang = njCos(particle->ang);
                        sin_ang = njSin(particle->ang);
                        dx = quad.vx1 = sx * cos_ang;
                        quad.vx2 = -(sx * sin_ang);
                        dy = quad.vy1 = sy * sin_ang;
                        quad.vy2 = sy * cos_ang;
                    } else {
                        if (particle->ang != 0) {
                            dx = quad.vx1 = sx * njCos(particle->ang);
                            dy = quad.vy1 = sy * njSin(particle->ang);
                        } else {
                            dx = quad.vx1 = sx;
                            dy = quad.vy1 = 0.0F;
                        }
                        quad.vx2 =
                            sx * njCos(particle->ang + particle->edge_angle);
                        quad.vy2 =
                            sy * njSin(particle->ang + particle->edge_angle);
                    }
                } else {
                    if (particle->edge_angle == QUARTER_TURN) {
                        cos_ang = njCos(particle->ang);
                        sin_ang = njSin(particle->ang);
                        dx = quad.vx1 = sx * cos_ang;
                        dx += quad.vx2 = -(sx * sin_ang);
                        dy = quad.vy1 = sy * sin_ang;
                        dy += quad.vy2 = sy * cos_ang;
                    } else {
                        if (particle->ang != 0) {
                            dx = quad.vx1 = sx * njCos(particle->ang);
                            dy = quad.vy1 = sy * njSin(particle->ang);
                        } else {
                            dx = quad.vx1 = sx;
                            dy = quad.vy1 = 0.0F;
                        }
                        dx += quad.vx2 =
                            sx * njCos(particle->ang + particle->edge_angle);
                        dy += quad.vy2 =
                            sy * njSin(particle->ang + particle->edge_angle);
                    }
                }
                quad.x -= dx * 0.5F;
                quad.y -= dy * 0.5F;
            } else {
                scale = particle->scale * quad.z;
                sx = scale_x * scale;
                sy = -(scale_y * scale);
                if (!(flags & PARTICLE_ANY_SIZE) &&
                    group->size_limit < _builtin_fabsf(sx * sy)) {
                    continue;
                }
                quad.x -= sx;
                quad.y -= (flags & PARTICLE_BASE) ? 0.0F : sy;
                quad.vx1 = sx * 2.0F;
                quad.vy1 = 0.0F;
                quad.vx2 = 0.0F;
                quad.vy2 = sy * 2.0F;
            }
            njDrawQuadTextureEx(&quad);
        }
        if (flags & DRAW_STARTED) {
            njQuadTextureEnd();
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
    draw_control_restore();
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
}

static void particle_empty_groups(void)
{
    while (particle_groups != NULL) {
        /* @hack redundant: the group's particles freed and cleared, which
         * particle_drop_group does again */
        particle_free_list(particle_groups->particles);
        particle_groups->particles = NULL;
        particle_groups = particle_drop_group(particle_groups);
    }
}

/* free particle and each particle after it by next. no group's list changes */
static void particle_free_list(struct particle *particle)
{
    struct particle *next;

    while (particle != NULL) {
        next = particle->next;
        syFree(particle);
        particle = next;
    }
}

/* free group's particles, so that it holds none, and return the group after
 * it */
static struct particle_group *particle_drop_group(struct particle_group *group)
{
    struct particle_group *next;

    particle_free_list(group->particles);
    group->particles = NULL;
    /* @hack spelling: next, a local for group->next, where returning it would
     * do */
    next = group->next;
    return next;
}

struct particle *particle_create(struct particle_group *group)
{
    struct particle *particle;

    particle = syMalloc(sizeof(struct particle));
    if (particle == NULL) {
        return NULL;
    }
    if (group->tmpl != NULL) {
        *particle = *group->tmpl;
    } else {
        *particle = particle_template[0];
    }
    if (group->particles == NULL) {
        group->next = particle_groups;
        particle_groups = group;
    }
    particle->next = group->particles;
    group->particles = particle;
    particle->frame = group->first_frame;
    if (game_state.time_stopped != 0) {
        particle->flags |= PARTICLE_MADE_IN_TIME_STOP;
    }
    return particle;
}

int particle_move(struct particle_group *group, struct particle *particle)
{
    particle->ang += particle->ang_spd;
    particle->scale += group->scale_step;
    particle->pos.x += particle->spd.x;
    particle->pos.y += particle->spd.y;
    particle->pos.z += particle->spd.z;
    particle->spd.x *= group->friction;
    particle->spd.y = (particle->spd.y * group->friction) + group->gravity;
    particle->spd.z *= group->friction;
    particle->frame += group->frame_step;
    /* @hack type: the frame compared through a Sint16, where
     * particle->frame >= group->frames would do for a frame from 0 below
     * 32768 */
    if ((Uint32)(Sint16)particle->frame >= group->frames) {
        particle->frame = group->frames - 1;
        return 0;
    }
    return 1;
}
