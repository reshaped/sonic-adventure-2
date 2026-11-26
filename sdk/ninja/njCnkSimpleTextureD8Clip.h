/* the vertex send that njCnkSimpleTextureD8Clip and njCnkSimpleTextureD8DbClip
 * share. SEND_VERTEX reads the caller's sq */
#ifndef NINJA_NJCNKSIMPLETEXTURED8CLIP_H
#define NINJA_NJCNKSIMPLETEXTURED8CLIP_H
#include <machine.h>
#include <nj_cnk.h>
#include <sg_xpt.h>

/* send entry as a vertex at sq, with param as its control word. (u, v) is its
 * texture coordinate. sq is left past the vertex */
#define SEND_VERTEX(entry, u, v, param)                                        \
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
        inten = *p++;                                                          \
        spec = *p;                                                             \
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

#endif
