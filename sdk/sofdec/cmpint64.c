/* Sofdec's comparisons of 64-bit values (sofdec_uty.h) */
#include <sg_xpt.h>
#include <sofdec_uty.h>

/* @hack spelling: out set in each branch of an if, where out = test would
 * do */
#define SET_RESULT(out, test)                                                  \
    do {                                                                       \
        if (test) {                                                            \
            (out) = TRUE;                                                      \
        } else {                                                               \
            (out) = FALSE;                                                     \
        }                                                                      \
    } while (0)

Bool UTY_CmpEqUint64(const struct uty_u64 *left, const struct uty_u64 *right)
{
    Bool result;

    SET_RESULT(result, left->high == right->high && left->low == right->low);
    return result;
}

Bool UTY_CmpEqSint64(const struct uty_s64 *left, const struct uty_s64 *right)
{
    Bool result;

    SET_RESULT(result, left->high == right->high && left->low == right->low);
    return result;
}

Bool UTY_CmpLeUint64(const struct uty_u64 *left, const struct uty_u64 *right)
{
    Bool result;

    if (left->high == right->high) {
        SET_RESULT(result, left->low <= right->low);
    } else if (left->high <= right->high) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}

Bool UTY_CmpLeSint64(const struct uty_s64 *left, const struct uty_s64 *right)
{
    Bool result;

    if (left->high == right->high) {
        if (left->high >= 0) {
            SET_RESULT(result, left->low <= right->low);
            /* @bug logic: the call finds -2 <= -1 false. where both high
             * words are the same negative value, the low words compare
             * reversed, at 0x8c551540, though a low word orders the same at
             * either sign */
        } else if (left->low >= right->low) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else if (left->high <= right->high) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}

Bool UTY_CmpLtUint64(const struct uty_u64 *left, const struct uty_u64 *right)
{
    Bool result;

    if (left->high == right->high) {
        SET_RESULT(result, left->low < right->low);
    } else if (left->high < right->high) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}

Bool UTY_CmpLtSint64(const struct uty_s64 *left, const struct uty_s64 *right)
{
    Bool result;

    if (left->high == right->high) {
        if (left->high >= 0) {
            SET_RESULT(result, left->low < right->low);
            /* @bug logic: the call finds -2 < -1 false. where both high words
             * are the same negative value, the low words compare reversed, at
             * 0x8c55159e, though a low word orders the same at either sign */
        } else if (left->low > right->low) {
            result = TRUE;
        } else {
            result = FALSE;
        }
    } else if (left->high < right->high) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    return result;
}

Bool UTY_CmpPlSint64(const struct uty_s64 *value)
{
    Bool positive = FALSE;

    if (value->high > 0) {
        positive = TRUE;
    } else if (value->high == 0 && value->low != 0) {
        positive = TRUE;
    }
    return positive;
}

Bool UTY_CmpPzSint64(const struct uty_s64 *value)
{
    /* @hack spelling: the test returned through nonnegative, where returning
     * it would do */
    Bool nonnegative;

    nonnegative = value->high >= 0;
    return nonnegative;
}
