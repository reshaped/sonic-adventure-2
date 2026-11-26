/* the vertex sends that njCnkSimplePolygonEnvDb, njCnkSimpleTextureEnvDb and
 * their Clip forms share. each reads the caller's sq. SEND_FRONT and SEND_BACK
 * also read its ambient */
#ifndef NINJA_NJCNKSIMPLEENVDB_H
#define NINJA_NJCNKSIMPLEENVDB_H
#include <machine.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* send a vertex at sq, with param as its control word and (u, v) as its
 * texture coordinate. its other words are the locals x, y, ooz, inten and
 * spec. sq is left past the vertex */
#define SEND_VERTEX(u, v, param)                                               \
    {                                                                          \
        /* @hack spelling: the words stored as sq steps down through the       \
         * vertex, where indexed stores from sq would do */                    \
        sq += CNK_BLOCK_WORDS;                                                 \
        *(Float *)--sq = spec;                                                 \
        *(Float *)--sq = inten;                                                \
        *(Float *)--sq = (v);                                                  \
        *(Float *)--sq = (u);                                                  \
        *(Float *)--sq = ooz;                                                  \
        *(Float *)--sq = y;                                                    \
        *(Float *)--sq = x;                                                    \
        *--sq = (param);                                                       \
        prefetch(sq);                                                          \
        sq += CNK_BLOCK_WORDS;                                                 \
    }

/* send entry through SEND_VERTEX as a vertex of a run that faces the viewer */
#define SEND_FRONT(entry, u, v, param)                                         \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float x;                                                      \
        register Float y;                                                      \
        register Float ooz;                                                    \
        register Float inten;                                                  \
        register Float spec;                                                   \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = *p++ + ambient;                                                \
        if (inten < ambient) {                                                 \
            inten = ambient;                                                   \
        }                                                                      \
        spec = *p;                                                             \
        SEND_VERTEX(u, v, param)                                               \
    }

/* send entry through SEND_VERTEX as a vertex of a run that faces away */
#define SEND_BACK(entry, u, v, param)                                          \
    {                                                                          \
        /* @hack redundant: register on the macro's locals */                  \
        register Float *p;                                                     \
        register Float x;                                                      \
        register Float y;                                                      \
        register Float ooz;                                                    \
        register Float inten;                                                  \
        register Float spec;                                                   \
        /* @hack spelling: the entry's fields read as p steps along it, where  \
         * reads of entry's fields would do */                                 \
        p = &(entry)->x;                                                       \
        x = *p++;                                                              \
        y = *p++;                                                              \
        ooz = *p++;                                                            \
        inten = ambient - *p++;                                                \
        spec = -*p;                                                            \
        if (inten < ambient) {                                                 \
            inten = ambient;                                                   \
        }                                                                      \
        SEND_VERTEX(u, v, param)                                               \
    }

#endif
