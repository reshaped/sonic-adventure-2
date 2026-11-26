/* the Egg Golem's statue and its texture list. each fight's golem.c places a
 * collision sensor at each of the nine nodes after golem_statue. the hero
 * story's golem.c also finds the flash at the head, and the hands' points at
 * the hands */
#ifndef SHARED_GOLEM_STATUE_H
#define SHARED_GOLEM_STATUE_H

#include <ninja.h>

extern NJS_TEXLIST golem_statue_texlist;
extern NJS_CNK_OBJECT golem_statue;
extern NJS_CNK_OBJECT golem_statue_joint_1;
extern NJS_CNK_OBJECT golem_statue_joint_5;
extern NJS_CNK_OBJECT golem_statue_joint_6;
extern NJS_CNK_OBJECT golem_statue_joint_7;
extern NJS_CNK_OBJECT golem_statue_hand_a;
extern NJS_CNK_OBJECT golem_statue_joint_13;
extern NJS_CNK_OBJECT golem_statue_joint_14;
extern NJS_CNK_OBJECT golem_statue_hand_b;
extern NJS_CNK_OBJECT golem_statue_head;

#endif
