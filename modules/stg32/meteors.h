/* Meteor Herd's meteors: the rocks, METEO in its object list, and the meteors
 * and fireballs that meteor_herd_step throws. the header also declares the
 * builds and the footprints that meteor_herd_init starts */
#ifndef STG32_METEORS_H
#define STG32_METEORS_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the meteors' texture lists for texture_load_shared, chips and shards
 * included, ended by null */
extern NJS_TEXLIST *meteor_textures[];
/* the footprints' texture list for texture_load_shared, ended by null */
extern NJS_TEXLIST *meteor_footprint_textures[];
/* the modifier volume that each fireball and each large meteor draws at its
 * pos, as its shadow */
extern NJS_CNK_OBJECT meteor_fireball_shadow[];

/* how many falling meteors live. meteor_fall_init adds 1, and the meteor's
 * destructor takes 1 away */
extern Sint32 falling_meteor_count;
/* how many fireballs live. meteor_fireball_init adds 1, and the fireball's
 * destructor takes 1 away */
extern Sint32 fireball_count;

/* compile the models of the meteors, the tank and the platform at buffer, as
 * model_builds_compile does. each returns the buffer past its builds.
 * meteor_tank_build first compiles the tank's pieces, as debris_build
 * does. where a call builds fewer than all its models, their objects draw the
 * models with njCnkEasyDrawModel */
Uint8 *meteor_models_build(Uint8 *buffer);
Uint8 *meteor_tank_build(Uint8 *buffer);
Uint8 *meteor_platform_build(Uint8 *buffer);

/* start the footprints that player's feet leave. only a Knuckles, a Tical or a
 * Rouge leaves prints. the task is a child of parent, or a task of its own
 * where parent is null. returns the task, or null where player is GAME_PLAYERS
 * or more or its slot has no work. it returns null too where the task or its
 * prints can't be allocated. player must not be below 0, unchecked */
struct task *meteor_footprints_create(int player, struct task *parent);

/* the object list's step for METEO: a rock that breaks apart where a player
 * touches it. it does nothing where task leaves at its placement's range. task
 * needs a work, whose pos places the rock. the low bit of scl.x's whole part,
 * taken unsigned, picks rock A or rock B. the rock starts at random angles. it
 * spins 0.36 + scl.y degrees a step about x, and 0.45 + scl.z about y. where
 * the heap has no room for its collision record, the rock never breaks */
void meteor_rock_init(struct task *task);

/* a meteor that meteor_herd_step throws. task needs a work, whose scl is the
 * meteor's speed. the work's timer counts the meteor's steps, and the meteor
 * ends once the timer passes METEOR_LIFE, the work's ang.z */
void meteor_fall_init(struct task *task);
#define METEOR_LIFE(work) ((work)->ang.z)

/* a fireball that meteor_herd_step throws from the work's pos at the point
 * scl. task needs a work. the fireball lands on scl once it comes within a
 * step of it, or touches another record. where the work's submode isn't 0, it
 * breaks apart as it lands. otherwise it bounces where it lands, and breaks
 * apart 58 steps later. it blasts just before it breaks, where a
 * player is near. it hurts the players it touches.
 *
 * the heap must have room for the fireball's two records, its motion and its
 * collision. without the motion record, the fireball ends at its first step.
 * without the collision record, the step that breaks it apart reads through
 * null */
void meteor_fireball_init(struct task *task);
/* the submode meteor_herd_step gives a fireball that breaks apart as it lands,
 * without a bounce */
#define FIREBALL_SHATTERS 1

#endif
