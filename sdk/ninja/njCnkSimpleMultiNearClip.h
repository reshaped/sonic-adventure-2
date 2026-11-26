/* the vertex and the vertex sends that the simple multi drawer's four texture
 * near clips share */
#ifndef NINJA_NJCNKSIMPLEMULTINEARCLIP_H
#define NINJA_NJCNKSIMPLEMULTINEARCLIP_H
#include <machine.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* a vertex of the near clip's strip. its fields go out in order. SEND_VERTEX
 * and SEND_DIFF_VERTEX send them as words 1 to 5, 8 to 11 and 13 to 15 */
struct clip_vertex {
    Float sx;
    Float sy;
    Float ooz;
    Float u;
    Float v;
    Float a;
    Float r;
    Float g;
    Float b;
    Float sr;
    Float sg;
    Float sb;
};
/* the Floats of struct clip_vertex before its color: sx, sy, ooz, u and v */
#define CLIP_COLOR 5

/* send vertex at sq, with param as its control word. sq is left past the
 * vertex. the macro overwrites the parameters a, ar, ag and ab */
#define SEND_VERTEX(vertex, param)                                             \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        /* @hack spelling: vertex's fields read as q steps along it, where     \
         * reads of vertex's fields would do */                                \
        q = &(vertex)->sx;                                                     \
        /* @hack spelling: each field copied through a variable of the         \
         * function, where a local of the macro would do */                    \
        x = *q++;                                                              \
        y = *q++;                                                              \
        z = *q++;                                                              \
        u = *q++;                                                              \
        v = *q++;                                                              \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = z;                                                    \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        a = *q++;                                                              \
        ar = *q++;                                                             \
        ag = *q++;                                                             \
        ab = *q++;                                                             \
        x = *q++;                                                              \
        y = *q++;                                                              \
        z = *q;                                                                \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = z;                                                    \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        sq--;                                                                  \
        *(Float *)--sq = ab;                                                   \
        *(Float *)--sq = ag;                                                   \
        *(Float *)--sq = ar;                                                   \
        *(Float *)--sq = a;                                                    \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

/* SEND_VERTEX for a Diff near clip, which takes r, g and b. it overwrites those
 * three and a, where SEND_VERTEX overwrites a, ar, ag and ab */
#define SEND_DIFF_VERTEX(vertex, param)                                        \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through each      \
         * block, where indexed stores from sq would do */                     \
        sq += CNK_UV_END;                                                      \
        /* @hack spelling: vertex's fields read as q steps along it, where     \
         * reads of vertex's fields would do */                                \
        q = &(vertex)->sx;                                                     \
        /* @hack spelling: each field copied through a variable of the         \
         * function, where a local of the macro would do */                    \
        x = *q++;                                                              \
        y = *q++;                                                              \
        z = *q++;                                                              \
        u = *q++;                                                              \
        v = *q++;                                                              \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = z;                                                    \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        a = *q++;                                                              \
        r = *q++;                                                              \
        g = *q++;                                                              \
        b = *q++;                                                              \
        x = *q++;                                                              \
        y = *q++;                                                              \
        z = *q;                                                                \
        sq += CNK_VERTEX64_WORDS;                                              \
        *(Float *)--sq = z;                                                    \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        sq--;                                                                  \
        *(Float *)--sq = b;                                                    \
        *(Float *)--sq = g;                                                    \
        *(Float *)--sq = r;                                                    \
        *(Float *)--sq = a;                                                    \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

#endif
