/* the GCC build's check that Ninja's matrix stack has room for a walk of a
 * Chao's body. the walk pushes a matrix for each level of the body's tree */
#ifndef CHAO_BODY_MATRIX_H
#define CHAO_BODY_MATRIX_H
#include <ninja.h>

#ifdef NONMATCHING
extern Sint32 _nj_max_stack_size_;

/* the matrices each walk pushes beyond the tree's levels. each pushes one
 * before the root. drawing pushes one more for a held prop. posing a spring
 * joint pushes two, for the joint and its relative transform */
enum chao_matrix_frames {
    CHAO_BUILD_FRAMES = 1,
    CHAO_DRAW_FRAMES = 2,
    CHAO_POSE_FRAMES = 3
};
/* whether the stack has room for a walk of root's tree that pushes extra
 * matrices beyond its levels. it is FALSE for a null root, and for a stack
 * that holds no matrix */
Bool chao_body_has_matrix_room(const NJS_CNK_OBJECT *root,
                               enum chao_matrix_frames extra);

/* whether the stack holds a matrix and has room for one more */
static inline Bool chao_matrix_slot_available(void)
{
    return _nj_stack_size_ > 0 && _nj_stack_size_ < _nj_max_stack_size_;
}
#endif

#endif
