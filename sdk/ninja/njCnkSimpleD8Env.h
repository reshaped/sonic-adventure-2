/* the vertex send that njCnkSimplePolygonD8Env and njCnkSimpleTextureD8Env
 * share with their Clip forms and with njCnkSimplePolygonD8EnvDbClip and
 * njCnkSimpleTextureD8EnvDbClip. SEND_VERTEX reads the caller's sq, uoff and
 * voff */
#ifndef NINJA_NJCNKSIMPLED8ENV_H
#define NINJA_NJCNKSIMPLED8ENV_H
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
        inten = *p++;                                                          \
        spec = *p++;                                                           \
        u = uoff + (*p++ * CNK_ENV_SCALE);                                     \
        v = voff + (*p * CNK_ENV_SCALE);                                       \
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
