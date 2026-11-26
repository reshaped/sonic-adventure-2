#ifndef STG31_ENEMIES_H
#define STG31_ENEMIES_H

struct task;

/* the object list's steps for Green Hill's three enemies, which it names
 * E BEETON, E BATABATA and E AKAHIGE. green_hill_akahige_step keeps
 * akahige_step's contract, which shared/akahige.h gives */
void green_hill_beeton_step(struct task *task);
void green_hill_batabata_step(struct task *task);
void green_hill_akahige_step(struct task *task);

#endif
