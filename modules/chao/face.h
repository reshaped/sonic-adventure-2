/* a garden Chao's face: its eye and mouth, and the lids that blink over its
 * eye */
#ifndef CHAO_FACE_H
#define CHAO_FACE_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* eyes 0 to 13. CHAO_EYE_OPEN, CHAO_EYE_HALF_SHUT and CHAO_EYE_NARROW show
 * the eye of the Chao's color, under lids that blink. their lids rest open,
 * half shut and narrowed. an open eye rests narrowed where the default eye is
 * CHAO_EYE_NARROW. every other eye is drawn whole, under lids held open. a
 * Chaos Chao shows the eye of its color for every eye but CHAO_EYE_ASLEEP */
enum chao_eye {
    CHAO_EYE_OPEN = 0,
    /* a hurt, crying, sick or frightened Chao's. a Chao that stretches,
     * frolics, taunts or exerts itself shows eye 1 too */
    CHAO_EYE_DISTRESSED = 1,
    CHAO_EYE_GLAD = 4,
    CHAO_EYE_ASLEEP = 7,
    CHAO_EYE_HALF_SHUT = 9,
    CHAO_EYE_NARROW = 10, /* a dark Chao's default */
    /* the eyes of a Chaos Chao, a hero Chaos Chao and a dark one */
    CHAO_EYE_CHAOS = 11,
    CHAO_EYE_HERO_CHAOS = 12,
    CHAO_EYE_DARK_CHAOS = 13
};

/* mouths 0 to 12. a Chaos Chao shows mouth 0 for every mouth */
enum chao_mouth { CHAO_MOUTH_GLAD = 3, CHAO_MOUTH_DISTRESSED = 8 };

/* a timer of the face that shows its eye or mouth until another replaces it */
#define CHAO_FACE_HOLD (-1)

struct chao_face {
    /* the steps the eye shows yet before the default, or below 0 to hold it */
    Sint32 eye_timer;
    Sint16 eye_color, eye, default_eye;
    Sint32 mouth_timer; /* as eye_timer, for the mouth */
    Sint16 mouth, default_mouth;
    Float eye_x, eye_y, eye_scale_x, eye_scale_y;
    Uint32 flags;
    NJS_CNK_OBJECT *eye_node[2], *mouth_node;
    Sint32 blink_mode, blink_timer;
    Angle blink_angle;
    Sint32 lid_mode, lid_timer;
    /* how far the lids close and slant, besides a blink, and the angles each
     * turns toward. chao_face_init sets the defaults to 0, and the face never
     * reads them. the editor's eyelid page steps them, and sets the other two
     * to them */
    Angle default_closure, closure, target_closure;
    Angle default_slope, slope, target_slope;
};

/* set the face from the Chao's type and parameters, and show its default eye
 * and mouth. a dark Chao's parameters.eye becomes CHAO_EYE_NARROW */
void chao_face_init(struct task *chao);
/* count down the eye's and mouth's timers, and move the lids and the blink */
void chao_face_step(struct task *chao);
/* the lids' turn about x: -QUARTER_TURN while they are open, up to
 * QUARTER_TURN while they are shut */
Angle chao_eyelid_angle(struct task *chao);
/* the lids' slant about z, which one lid takes negated */
Angle chao_eyelid_slope(struct task *chao);
Sint16 chao_current_eye(struct task *chao);
int chao_default_eye(struct task *chao);
int chao_default_mouth(struct task *chao);
/* show eye or mouth until another replaces it */
void chao_set_eye(struct task *chao, int eye);
void chao_set_mouth(struct task *chao, int mouth);
/* show eye or mouth through the next frames calls of chao_face_step. the
 * call after them shows the default. frames below 0 hold it */
void chao_set_timed_eye(struct task *chao, int eye, int frames);
void chao_set_timed_mouth(struct task *chao, int mouth, int frames);
/* show the default eye, or the default mouth, until another replaces it */
void chao_restore_eyes(struct task *chao);
void chao_restore_mouth(struct task *chao);

#endif
