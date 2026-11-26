/* the course as cart_build_course lays it out. each section places one of the
 * CART_PIECES pieces where the section before it ends */
#ifndef CART_TRACK_H
#define CART_TRACK_H
#include <ninja.h>
#include <sg_xpt.h>

#include "cart/objects.h"
#include "cart/race.h"

#define CART_PIECES 85
/* the pieces that are tunnels */
#define CART_FIRST_TUNNEL 50
#define CART_LAST_TUNNEL 71
#define CART_SECTIONS 512
#define CART_SIGNS 4

/* a piece. step reaches from its start to the next piece's, in its own frame.
 * the next piece turns turn further about y. compiled is model compiled for
 * njDirectDrawObject, or null. signs holds sign_count spots, up to
 * CART_SIGNS, where a sign may stand */
struct cart_piece {
    NJS_CNK_OBJECT *model;
    NJS_OBJECT *solid;
    NJS_VECTOR step;
    Angle turn;
    Uint8 unk_18[0x48];
    NJS_DIRECT_HEAD *compiled;
    struct cart_sign *signs;
    Sint32 sign_count;
};
/* a section: its piece's index in cart_pieces, where it stands and its yaw,
 * and its piece's models. signs holds a value for each of its piece's sign
 * spots. 0 is no sign, and any other value cart_draw_sign's kind plus 1 */
struct cart_section {
    Sint32 piece;
    NJS_POINT3 pos;
    NJS_CNK_OBJECT *model;
    NJS_OBJECT *solid;
    Angle yaw;
    NJS_DIRECT_HEAD *compiled;
    Sint32 signs[CART_SIGNS];
};
/* a piece's gates in its own frame, which cart_build_course places in
 * cart_gates */
struct cart_gate_list {
    struct cart_gate_point (*gates)[CART_GATE_POINTS];
    Sint32 count;
};

extern struct cart_gate_list cart_piece_gates[CART_PIECES];
extern struct cart_piece cart_pieces[CART_PIECES];
extern struct cart_section cart_sections[CART_SECTIONS];
extern Sint32 cart_section_count;
/* end the course's draw task, and its sections' collision tasks, at their
 * next steps. no code calls either */
void cart_stop_course_draw(void);
void cart_stop_course_collision(void);
#endif
