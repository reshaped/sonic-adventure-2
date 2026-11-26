/* the vector copies spark_point.c and spark_sprite.c share. GET_XYZ reads a
 * vector's x, y and z into three Floats, and SET_XYZ writes them back */
#ifndef SHARED_SPARK_XYZ_H
#define SHARED_SPARK_XYZ_H

/* @hack spelling: x, y and z read through a walking pointer, where three
 * assignments would do */
#define GET_XYZ(vec, a, b, c)                                                  \
    {                                                                          \
        Float *part = &(vec)->x;                                               \
        (a) = *part++;                                                         \
        (b) = *part++;                                                         \
        (c) = *part;                                                           \
    }
/* @hack spelling: x, y and z written through a walking pointer, where three
 * assignments would do */
#define SET_XYZ(vec, a, b, c)                                                  \
    {                                                                          \
        Float *part = &(vec)->x;                                               \
        *part++ = (a);                                                         \
        *part++ = (b);                                                         \
        *part = (c);                                                           \
    }

#endif
