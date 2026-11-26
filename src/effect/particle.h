/* particles: quads that move, turn, grow and age. a group's particles share
 * its texture list, draw flags and step. the particle system is a task that
 * steps and draws every group that holds a particle */
#ifndef EFFECT_PARTICLE_H
#define EFFECT_PARTICLE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a particle draws as a quad, a parallelogram on the screen spanned by two
 * edges from one corner. its center lies where pos does. its first edge lies
 * at ang from the screen's x axis. its second lies at edge_angle from the
 * first. each edge is as long as 2 * scale at the particle's depth. the
 * screen's aspect then stretches the quad in x and in y. the edges keep those
 * angles only under a square aspect */
struct particle {
    Angle ang;
    Float scale;
    NJS_POINT3 pos;
    Uint32 color; /* ARGB, with the alpha in the top byte */
    /* the particle's age, counted in its group's textures. frame's whole part
     * numbers the texture of texlist it draws with. The port skips frames
     * without a loaded texture, leaving the particle in its group. A whole part
     * below 0 draws nothing. a frame below 0 frees the particle at the next
     * step */
    Float frame;
    Angle edge_angle;
    NJS_VECTOR spd; /* what particle_move adds to pos */
    Angle ang_spd; /* what particle_move adds to ang */
    struct particle *next; /* the group's next particle, or null */
    Uint32 flags;
    /* under its group's PARTICLE_DEPTH, the particle draws nearer by
     * depth_bias. the system first divides it by the length of the current
     * matrix's z axis. the quad takes the size it would have at that depth,
     * and keeps its place on the screen. a group without the flag may keep
     * anything here */
    Float depth_bias;
};

/* in a particle's flags: particle_create made it during a time stop, while
 * game_state.time_stopped wasn't 0. during a time stop the system steps only
 * the particles that have it. the others wait unstepped, and still draw */
#define PARTICLE_MADE_IN_TIME_STOP 0x1

/* a group, which its user owns. the system links it into its list while it
 * holds a particle, and never frees it */
struct particle_group {
    Uint32 flags; /* the draw flags below */
    NJS_TEXLIST *texlist;
    Uint32 first_frame; /* the frame particle_create gives a particle */
    Uint32 frames; /* particle_move ends a particle whose frame reaches this */
    Float frame_step; /* what particle_move adds to a particle's frame */
    Float friction; /* what particle_move multiplies the particle's spd by */
    Float gravity; /* what particle_move then adds to spd.y */
    Float scale_step; /* what particle_move adds to the particle's scale */
    /* the system calls it for each of the group's particles at each step. a
     * result of 0 frees the particle */
    int (*step)(struct particle_group *group, struct particle *particle);
    /* the largest quad the system draws. a quad's size here is half its
     * unturned width times half its height, in screen units. a larger quad is
     * skipped. 0 or less skips none */
    Float size_limit;
    /* what particle_create copies, or null for the system's template. the
     * system's template is white, at a scale of 1, with an edge_angle of a
     * quarter turn. the template's other fields are 0 */
    struct particle *tmpl;
    Sint32 unk_2c; /* nothing reads it */
    struct particle *particles; /* the group's first particle, or null */
    struct particle_group *next; /* the next group that holds a particle */
};

/* the draw flags of a group, and of particle_draw_quad.
 *
 * PARTICLE_BLEND is a group's alone. PARTICLE_BLEND_ADD adds a quad, by its
 * alpha, to what lies behind it, and draws with fog off. PARTICLE_BLEND_ALPHA
 * lays the quad over what lies behind by its alpha, with fog on. a group with
 * both bits or neither draws with fog off, in the blend already set */
#define PARTICLE_BLEND 0x3
#define PARTICLE_BLEND_ADD 0x1
#define PARTICLE_BLEND_ALPHA 0x2
/* the quads go in the opaque list, not the translucent */
#define PARTICLE_OPAQUE 0x8
/* mirror the texture along the quad's first edge, or along its second */
#define PARTICLE_FLIP_U 0x10
#define PARTICLE_FLIP_V 0x20
/* the quad's first edge is its base. the base's middle lies where pos does, in
 * place of the quad's center */
#define PARTICLE_BASE 0x40
/* multiply each color by the system's tint with color_multiply. nothing
 * changes the tint from white */
#define PARTICLE_TINT 0x80
/* draw each particle nearer by its depth_bias */
#define PARTICLE_DEPTH 0x100
/* draw every quad at a depth of 0.001, in front of nearly all else. a group's
 * quads also take the size they would have that near. a group's particle
 * behind the camera still draws, mirrored through the screen's center.
 * particle_draw_quad's quad keeps its size and place */
#define PARTICLE_FRONT 0x200
/* ignore the group's size_limit */
#define PARTICLE_ANY_SIZE 0x400
/* for particle_draw_quad alone: depth is the quad's depth, not a bias */
#define PARTICLE_DEPTH_Z 0x800

/* start the particle system, a task named ParticleCoreTask in level 1, unless
 * one runs. nothing starts where the heap can't give the task. the task's first
 * step empties the list of groups without freeing a particle, as
 * particle_forget_all does. each later step steps every listed group, and each
 * draw draws them. the task's end, however it comes, frees every listed
 * group's particles */
void particle_core_start(void);
/* destroy the particle system's task, if one runs */
void particle_core_stop(void);
/* draw a quad at pos in texture texid of the current texture list, colored
 * color. The port skips an absent or unloaded texture before starting the
 * quad. The quad is a particle's of scale 1, turned by ang and edge_angle. it
 * is then stretched by half_width along the screen's x and by half_height along
 * its y. unturned, it is 2 * half_width wide and 2 * half_height high. flags
 * takes the draw flags but the blend, PARTICLE_TINT, PARTICLE_DEPTH and
 * PARTICLE_ANY_SIZE. the blend and the fog stay as they are.
 *
 * a depth that isn't 0 moves only the quad's depth, not its size or place.
 * the quad draws nearer by depth divided by the length of the current matrix's
 * z axis. under PARTICLE_DEPTH_Z, depth is the quad's depth itself.
 *
 * it draws nothing where pos, through the current matrix, has a z below 0. a z
 * of 0 gives the quad an infinite or not-a-number size and place. it draws
 * nothing where half the unturned quad's width times half its height passes a
 * size_limit above 0 */
void particle_draw_quad(NJS_POINT3 *pos, Sint32 texid, Angle edge_angle,
                        Angle ang, Float half_width, Float half_height,
                        Uint32 color, Sint32 flags, Float depth,
                        Float size_limit);
/* the bits of a packed ARGB color that hold its alpha */
#define COLOR_ALPHA 0xff000000
/* blend two packed colors byte by byte. each byte of the result is from's plus
 * (to's - from's) * mix, cut toward 0. a byte that comes out below 0 or above
 * 255 is 255 */
Uint32 color_blend(Uint32 from, Uint32 to, Float mix);
/* multiply two packed colors byte by byte, taking each byte as a fraction of
 * 255. each byte of the result is the product times 255, cut toward 0 */
Uint32 color_multiply(Uint32 first, Uint32 second);
/* make a particle at the head of group's list. it is a copy of the group's
 * template, with frame set to first_frame. during a time stop it also has
 * PARTICLE_MADE_IN_TIME_STOP. a group that held no particle joins the
 * system's list. it returns the particle, or null where the heap can't give
 * one */
struct particle *particle_create(struct particle_group *group);
/* a step for a group to use. it moves, turns, grows and ages the particle by
 * the group's numbers above. it returns 1, or 0 once frame's whole part reaches
 * frames, with frame left at frames - 1. the test reads the whole part as a
 * Sint16 and compares it unsigned. a part from -32768 to -1, or from 32768 to
 * 65535, thus ends the particle as well */
int particle_move(struct particle_group *group, struct particle *particle);

#endif
