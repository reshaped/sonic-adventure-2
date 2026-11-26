/* King Boom Boo's sway: a wave along x that moves part of a chunk model's
 * mesh in z */
#ifndef BOSS_BIGBOGY_SWAY_H
#define BOSS_BIGBOGY_SWAY_H
#include <ninja.h>

struct task;
/* make a sway of object as parent's child. the task keeps its own copy of
 * object's model and vertex list. each step it moves the copy's swaying
 * vertices, and points object->model at the copy's model. as the task ends,
 * it puts *object back as the call found it. object must outlive the task.
 * object's model must have a vertex list of only NJD_CV_VN and NJD_CV_VN_NF
 * chunks. the list, NJD_CE included, must be at most 32767 words long. the
 * call returns null where the task or its record can't be allocated. where
 * only the copy can't be, it returns the task, whose step then reads through
 * null. parent must not be null, or the call returns an unset pointer */
struct task *bigbogy_sway_create(struct task *parent, NJS_CNK_OBJECT *object);
/* the task's copy of its object, whose model is the one that sways */
NJS_CNK_OBJECT *bigbogy_sway_object(struct task *task);
#endif
