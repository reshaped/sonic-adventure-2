/* the models of the course's objects: the checkpoint, the dash panel, the
 * start gate and its lamps, the signs, the airship and KAKUSI */
#ifndef CART_COURSE_ART_H
#define CART_COURSE_ART_H
#include <ninja.h>

extern NJS_CNK_OBJECT cart_checkpoint_model[];
extern NJS_CNK_OBJECT cart_dash_frame, cart_dash_belt;
extern NJS_CNK_OBJECT cart_gate_model[], cart_countdown_lamp[],
    cart_race_lamp[], cart_final_lamp[];
extern NJS_CNK_OBJECT cart_sign_model;
extern NJS_CNK_OBJECT cart_sign_first, cart_sign_panel, cart_sign_second;
extern NJS_CNK_OBJECT cart_airship_first, cart_airship_second;
extern NJS_CNK_OBJECT cart_kakusi_model;

#endif
