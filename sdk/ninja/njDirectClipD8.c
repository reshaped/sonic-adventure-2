/* the D8 near clipper, and the globals the direct drawer's clippers share.
 * the clipper queues the first SENT_AT_ONCE vertices the clip leaves as it
 * makes them, and holds the rest. where the clip leaves four vertices, the
 * fourth goes ahead of the third. the four then make one strip */
#include <kamui.h>
#include <machine.h>
#include <ninja.h>
#include <nj_direct_draw.h>
#include <sg_xpt.h>

/* a store queue holds SQ_WORDS words, and one vertex. a D8 vertex takes
 * D8_VERTEX_WORDS of them: its command, x, y, 1/z, u, v and color */
#define SQ_WORDS 8
#define D8_VERTEX_WORDS 7
#define SENT_AT_ONCE 2
/* the near plane, z = NEAR_Z, and the 1/z of a point on it */
#define NEAR_Z 1.0F
#define NEAR_INVERSE_Z 1.0F
/* the byte of a struct direct_d8_corner that holds its color's alpha. the
 * red, green and blue bytes lie below it */
#define COLOR_ALPHA_BYTE 15

Float direct_clip_light[4];
NJS_POINT3 direct_point;
Uint32 *direct_queue;

#ifdef NONMATCHING
#define DEAD_LOOP(statement) ((void)0)
#else
#define DEAD_LOOP(statement)                                                   \
    /* @hack redundant: statement, in a loop that never runs */                \
    while (0) {                                                                \
        statement;                                                             \
    }
#endif

/* the channel fraction of the way from previous to current. the step is
 * truncated toward zero before previous is added */
#define CHANNEL_AT(previous, current, fraction)                                \
    ((Uint32)((Int)((Float)((current) - (previous)) * (fraction)) + (previous)))

#define SEND_PENDING(command)                                                  \
    {                                                                          \
        queue += D8_VERTEX_WORDS;                                              \
        *--queue = packed_color;                                               \
        *(Float *)--queue = screen_v;                                          \
        *(Float *)--queue = screen_u;                                          \
        *(Float *)--queue = depth;                                             \
        *(Float *)--queue = screen_y;                                          \
        *(Float *)--queue = screen_x;                                          \
        *--queue = (command);                                                  \
        prefetch(queue);                                                       \
        queue += SQ_WORDS;                                                     \
    }

/* first_w is unread. under SHC's convention it takes FR7, so that second_x
 * arrives in FR8, where njDirectCalcPointLoopD8 passes the second corner */
void njDirectNearClipSubD8(NJS_DIRECT_VERTEX_D8 *source, register Uint32 mask,
                           register Float first_x, register Float first_y,
                           Float first_z, Float first_w, Float second_x,
                           register Float second_y, Float second_z)
{
    /* @hack redundant: register on mask, first_x, first_y and second_y, and
     * on each local declared with it */
    struct direct_d8_corner triangle[DIRECT_TRIANGLE_CORNERS];
    struct direct_d8_screen_vertex pending[DIRECT_MOST_VERTICES - SENT_AT_ONCE];
    register struct direct_d8_corner *current;
    register struct direct_d8_corner *previous;
    register struct direct_d8_corner *corner;
    register struct direct_d8_corner *first;
    struct direct_d8_screen_vertex *next;
    struct direct_d8_screen_vertex *first_pending;
    struct direct_lighting *light;
    Float *scale_x;
    Float *scale_y;
    Float *center_x;
    Float *center_y;
    register Uint32 *queue;
    register Uint32 *word;
    register Float *previous_fields;
    register Float *current_fields;
    register Float *field;
    register Uint8 *previous_channel;
    register Uint8 *current_channel;
    register Uint32 color;
    Uint32 packed_color;
    register Int edges;
    register Int corners;
    register Int emitted;
    register Float fraction;
    register Float x;
    Float y;
    Float previous_u;
    Float previous_v;
    register Float projected_x;
    register Float projected_y;
    register Float u;
    Float v;
    register Float inverse_z;

    DEAD_LOOP(second_z = -second_z);
    DEAD_LOOP(++source);
    /* @hack spelling: queue, a copy of direct_queue held across the clip,
     * where direct_queue itself would do */
    queue = direct_queue;
    /* @hack spelling: first, a pointer to triangle, where triangle itself
     * would do */
    first = triangle;
    current = first;
    /* @hack spelling: each corner's x, y, z and behind written through
     * current, where triangle[0] to triangle[2] would do */
    current->x = first_x;
    current->y = first_y;
    current->z = first_z;
    /* @hack spelling: each corner's bit of mask shifted down to bit 0, where
     * (mask & DIRECT_FIRST_BEHIND) != 0 and the same tests of the other two
     * bits would do */
    current->behind = (mask >> 2) & 1;
    current[1].x = second_x;
    current[1].y = second_y;
    current[1].z = second_z;
    current[1].behind = (mask >> 1) & 1;
    current[2].x = direct_point.x;
    current[2].y = direct_point.y;
    current[2].z = direct_point.z;
    current[2].behind = mask & 1;
#ifdef NONMATCHING
#else
    /* @hack redundant: a test of first in a block that never runs */
    if (0) {
        if (first) {
        }
    }
#endif
    corners = DIRECT_TRIANGLE_CORNERS;
    corner = current;
    do {
        corner->color = source->base;
        corner->uv.half[0] = 0;
        corner->uv.half[1] = source->uv >> 16;
        corner->uv.half[2] = 0;
        corner->uv.half[3] = (Uint16)source->uv;
        ++source;
        ++corner;
    } while (--corners);

    emitted = 0;
    previous = current + DIRECT_TRIANGLE_CORNERS - 1;
    next = pending;
    /* @hack spelling: first_pending, a copy of next, where pending itself
     * would do */
    first_pending = next;
    edges = DIRECT_TRIANGLE_CORNERS;
    light = &direct_light;
    /* @hack type: the screen fields reached as floats from light's address,
     * where &light->scale_x and the others would do */
    scale_x = (Float *)light + 5;
    center_x = (Float *)light + 7;
    scale_y = (Float *)light + 6;
    center_y = (Float *)light + 8;
    DEAD_LOOP(++scale_x);
    DEAD_LOOP(++center_x);
    DEAD_LOOP(++scale_y);
    DEAD_LOOP(++center_y);
    DEAD_LOOP(emitted = 0);
    do {
        /* @hack spelling: the two behind flags compared by ^, where != would
         * do */
        if (previous->behind ^ current->behind) {
            /* @hack type: the corners' fields read through float and byte
             * pointers stepped along each corner, where their named fields
             * would do */
            previous_channel = (Uint8 *)previous;
            current_fields = (Float *)current;
            previous_fields = (Float *)previous;
            current_channel = (Uint8 *)current + COLOR_ALPHA_BYTE;
            previous_channel += COLOR_ALPHA_BYTE;
            fraction = (NEAR_Z - *previous_fields) /
                       (*current_fields - *previous_fields);
            ++previous_fields;
            ++current_fields;
            color = CHANNEL_AT(*previous_channel, *current_channel, fraction)
                    << 24;
            --current_channel;
            --previous_channel;
            x = *previous_fields++;
            projected_x =
                ((x + (fraction * (*current_fields++ - x))) * *scale_x) +
                *center_x;
            color |= CHANNEL_AT(*previous_channel, *current_channel, fraction)
                     << 16;
            --current_channel;
            --previous_channel;
            y = *previous_fields;
            projected_y =
                (*scale_y * (y + (fraction * (*current_fields - y)))) +
                *center_y;
            current_fields += 2;
            previous_fields += 2;
            color |= CHANNEL_AT(*previous_channel, *current_channel, fraction)
                     << 8;
            --current_channel;
            --previous_channel;
            previous_u = *previous_fields++;
            u = previous_u + (fraction * (*current_fields++ - previous_u));
            color |= CHANNEL_AT(*previous_channel, *current_channel, fraction);
            previous_v = *previous_fields;
            v = previous_v + (fraction * (*current_fields - previous_v));

            if (emitted < SENT_AT_ONCE) {
                queue += D8_VERTEX_WORDS;
                *--queue = color;
                *(Float *)--queue = v;
                *(Float *)--queue = u;
                *(Float *)--queue = NEAR_INVERSE_Z;
                *(Float *)--queue = projected_y;
                *(Float *)--queue = projected_x;
                *--queue = KM_VERTEXPARAM_NORMAL;
                prefetch(queue);
                queue += SQ_WORDS;
            } else {
                word = (Uint32 *)(next + 1);
                *--word = color;
                *(Float *)--word = v;
                *(Float *)--word = u;
                *(Float *)--word = NEAR_INVERSE_Z;
                *(Float *)--word = projected_y;
                *(Float *)--word = projected_x;
                ++next;
            }
            ++emitted;
        }
        if (!current->behind) {
            inverse_z = 1.0F / current->z;
            if (emitted < SENT_AT_ONCE) {
                queue += D8_VERTEX_WORDS;
                *--queue = current->color;
                *(Float *)--queue = current->uv.coordinate[1];
                *(Float *)--queue = current->uv.coordinate[0];
                projected_x = *center_x + ((current->x * inverse_z) * *scale_x);
                *(Float *)--queue = inverse_z;
                projected_y = *center_y + ((current->y * inverse_z) * *scale_y);
                *(Float *)--queue = projected_y;
                *(Float *)--queue = projected_x;
                *--queue = KM_VERTEXPARAM_NORMAL;
                prefetch(queue);
                queue += SQ_WORDS;
            } else {
                word = (Uint32 *)(next + 1);
                *--word = current->color;
                *(Float *)--word = current->uv.coordinate[1];
                *(Float *)--word = current->uv.coordinate[0];
                *(Float *)--word = inverse_z;
                *(Float *)--word =
                    *center_y + ((current->y * inverse_z) * *scale_y);
                *(Float *)--word =
                    *center_x + ((current->x * inverse_z) * *scale_x);
                ++next;
            }
            ++emitted;
        }
        previous = current;
        ++current;
    } while (--edges);

    if (emitted != DIRECT_TRIANGLE_CORNERS) {
        register Float screen_x;
        register Float screen_y;
        register Float depth;
        Float screen_u;
        Float screen_v;

        /* @hack type: the pending vertices read through a float pointer
         * stepped along each, where their named fields would do */
        field = (Float *)(pending + 1);
        screen_x = *field++;
        screen_y = *field++;
        depth = *field++;
        screen_u = *field++;
        screen_v = *field++;
        DEAD_LOOP(screen_y = *field);
        DEAD_LOOP(depth = *field);
        packed_color = *(Uint32 *)field;
        SEND_PENDING(KM_VERTEXPARAM_NORMAL);
    }
    {
        register Float screen_y;
        register Float screen_u;
        register Float screen_v;
        Float screen_x;
        Float depth;

        field = (Float *)first_pending;
        screen_x = *field++;
        screen_y = *field++;
        depth = *field++;
        screen_u = *field++;
        screen_v = *field++;
        DEAD_LOOP(screen_u = *field);
        DEAD_LOOP(screen_v = *field);
        packed_color = *(Uint32 *)field;
        SEND_PENDING(KM_VERTEXPARAM_ENDOFSTRIP);
    }
    direct_queue = queue;
}
