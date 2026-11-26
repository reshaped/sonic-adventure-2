/* the skull: SKULL, PSKULL and KSKULL in a stage's object list. a hunter's
 * dig throws one up too. a player can pick up and carry a lying skull */
#ifndef SHARED_SKULL_H
#define SHARED_SKULL_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* the skull's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *skull_texlists[];
/* the skull's texture list and model. NOCOLSKULL uses both in Aquatic
 * Mine and Egg Quarters */
extern NJS_TEXLIST skull_texlist;
extern NJS_CNK_MODEL skull_model;

/* the object list's steps for KSKULL and PSKULL. KSKULL's skull waits until
 * a touch throws it up, and PSKULL's lies. task needs a work and a motion
 * record. the heap must have room for the skull's collision record and its
 * body. without the record, the call reads through null. without the body,
 * a lying skull's step reads and writes through null.
 *
 * the low byte of the work's ang.z holds a chance and an animal's kind. the
 * skull's first hold releases a small animal of that kind by that chance. the
 * chance is 0.05 plus 0.1 times the byte's high four bits. high bits of 0
 * release none. the low four bits, modulo 3, pick the kind. a hold sets the
 * state of the skull's placement to 1. it releases an animal only where the
 * state was 0.
 *
 * a lying skull fades out over 60 steps once no player has held it for 2880.
 * it then ends. a player within 50 of it keeps it opaque */
void skull_waiting_init(struct task *task);
void skull_lying_init(struct task *task);
/* throw a skull up from pos at speed, and return its task. it returns null
 * where the heap can't give the task. the skull flies through the stage until
 * it starts to fall, and then lies as skull_lying_init's does. the heap must
 * have room for its collision record and body, as skull_lying_init's must */
struct task *skull_throw_up(NJS_POINT3 *pos, Float speed);
/* the object list's step for SKULL, a skull that stands still and never
 * fades. task needs a work. a scl.x below 1 gives the skull a sphere that
 * pushes players, enemies and objects */
void skull_still_init(struct task *task);

/* A placed NOCOLSKULL draws the skull model with no collision shapes. */
void nocolskull_init(struct task *task);

#endif
