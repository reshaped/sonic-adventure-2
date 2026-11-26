/* the vertex send that njCnkSimplePolygonEnv, njCnkSimpleTextureEnv and their
 * Clip forms share. SEND_VERTEX reads the caller's sq, ambient, uoff and
 * voff */
#ifndef NINJA_NJCNKSIMPLEENV_H
#define NINJA_NJCNKSIMPLEENV_H
#include <machine.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* send entry as a vertex at sq, with param as its control word. sq is left past
 * the vertex */
#define SEND_VERTEX(entry, param)                                              \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float x;                                                      \
        register Float y;                                                      \
        register Float ooz;                                                    \
        register Float inten;                                                  \
        register Float spec;                                                   \
        register Float u;                                                      \
        register Float v;                                                      \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p++ + ambient;                                                \
        spec = *p++;                                                           \
        if (inten < ambient) {                                                 \
            inten = ambient;                                                   \
        }                                                                      \
        u = (*p++ * CNK_ENV_SCALE) + uoff;                                     \
        v = (*p * CNK_ENV_SCALE) + voff;                                       \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = spec;                                                 \
        *(Float *)--sq = inten;                                                \
        *(Float *)--sq = v;                                                    \
        *(Float *)--sq = u;                                                    \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

#endif
