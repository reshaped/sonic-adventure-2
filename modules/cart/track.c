/* cart_build_course, which lays a layout's pieces end to end as sections,
 * with their gates and signs */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

#include "cart/course.h"
#include "cart/objects.h"
#include "cart/race.h"
#include "cart/track.h"
#include "draw/light.h"
#include "system/areas.h"
#include "system/util.h"
#include "system/vector.h"

/* where cart_build_course compiles the pieces' direct models, up to
 * DEMO_AREA. the first piece that would pass DEMO_AREA ends the compile. it
 * and the pieces after it keep the compiled model they had */
#define BUILD_AREA ((Uint8 *)0x8c980000)
/* the pieces compile under these bits, so that modifier volumes change their
 * polygons, opaque and translucent alike */
#define SHADOW_CONTROL (NJD_CONTROL_3D_SHADOW | NJD_CONTROL_3D_TRANS_MODIFIER)
/* a sign spot takes a sign half the time, where a random fraction passes
 * SIGN_THRESHOLD. the sign's kind is random too. the last SIGNLESS_SECTIONS
 * sections take none, and keep the signs they had */
#define SIGN_THRESHOLD 0.5F
#define SIGNLESS_SECTIONS 20
/* cart_course_lowest before any section lies lower */
#define NO_LOWEST 100000.0F

struct cart_section cart_sections[CART_SECTIONS];
Sint32 cart_section_count;
static void transform_gate(struct cart_gate_point *src, Angle yaw,
                           NJS_POINT3 *pos, struct cart_gate_point *dest);

int cart_build_course(const Sint8 *pieces, int count)
{
    int i;
    int j;
    Uint8 *buffer = BUILD_AREA;
    Uint8 *next;
    NJS_VECTOR step;

    draw_control_set(SHADOW_CONTROL);
    njSetTexture(&cart_object_textures);
    light_apply(LIGHT_LAND);
    njSetDirectClip();
    for (i = 0; i < CART_PIECES; i++) {
        if (!cart_pieces[i].model) {
            continue;
        }
        next = buffer + njCnkDirectObjectCompileSize(cart_pieces[i].model);
        if ((Uint32)next >= (Uint32)DEMO_AREA) {
            break;
        }
        njCnkDirectObjectCompile(cart_pieces[i].model, buffer);
        cart_pieces[i].compiled = (NJS_DIRECT_HEAD *)buffer;
        buffer = next;
    }
    draw_control_clear(SHADOW_CONTROL);
    cart_course_lowest = NO_LOWEST;
    cart_gate_count = 0;
    cart_section_count = count;
    for (i = 0; i < count; i++) {
        if (i < count - SIGNLESS_SECTIONS) {
            for (j = 0; j < CART_SIGNS; j++) {
                if (j < cart_pieces[pieces[i]].sign_count) {
                    if (rand() * RAND_SCALE > SIGN_THRESHOLD) {
                        cart_sections[i].signs[j] =
                            (int)((rand() * (CART_SIGN_KINDS * RAND_SCALE)) +
                                  1.0F);
                    } else {
                        cart_sections[i].signs[j] = 0;
                    }
                } else {
                    cart_sections[i].signs[j] = 0;
                }
            }
        }
        if (i == 0) {
            cart_sections[i].pos.x = 0;
            cart_sections[i].pos.y = 0;
            cart_sections[i].pos.z = 0;
            cart_sections[i].yaw = 0;
        } else {
            step = cart_pieces[pieces[i - 1]].step;
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njRotateY(NULL, cart_sections[i - 1].yaw);
            njCalcVector(NULL, &step, &step);
            njPopMatrixEx();
            vector_sum(&cart_sections[i - 1].pos, &step, &cart_sections[i].pos);
            cart_sections[i].yaw =
                cart_pieces[pieces[i - 1]].turn + cart_sections[i - 1].yaw;
            if (cart_sections[i].pos.y < cart_course_lowest) {
                cart_course_lowest = cart_sections[i].pos.y;
            }
        }
        if (cart_piece_gates[pieces[i]].gates) {
            for (j = 0; j < cart_piece_gates[pieces[i]].count; j++) {
                transform_gate(cart_piece_gates[pieces[i]].gates[j],
                               cart_sections[i].yaw, &cart_sections[i].pos,
                               cart_gates[cart_gate_count]);
                cart_gate_sections[cart_gate_count] = i;
                cart_gate_count++;
            }
        }
        cart_sections[i].model = cart_pieces[pieces[i]].model;
        cart_sections[i].compiled = cart_pieces[pieces[i]].compiled;
        cart_sections[i].solid = cart_pieces[pieces[i]].solid;
        cart_sections[i].piece = pieces[i];
    }
    return 0;
}

static void transform_gate(struct cart_gate_point *src, Angle yaw,
                           NJS_POINT3 *pos, struct cart_gate_point *dest)
{
    int i;
    NJS_VECTOR point;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, yaw);
    for (i = 0; i < CART_GATE_POINTS; i++) {
        point.x = src[i].x;
        point.z = src[i].z;
        point.y = 0;
        njCalcVector(NULL, &point, &point);
        vector_sum(&point, pos, &point);
        dest[i].x = point.x;
        dest[i].z = point.z;
    }
    njPopMatrixEx();
}
