/* Sofdec's comparison of two times in different units (sofdec_uty.h) */
#include <sg_xpt.h>
#include <sofdec_uty.h>

Bool UTY_CmpTime(Sint32 ticks_a, Sint32 unit_a, Sint32 ticks_b, Sint32 unit_b)
{
    Bool result;
    struct uty_s64 left;
    struct uty_s64 right;

    UTY_Dmuls(ticks_a, unit_b, &left);
    UTY_Dmuls(ticks_b, unit_a, &right);
    /* @hack spelling: UTY_CmpLeSint64's result returned through result, where
     * returning the call would do */
    result = UTY_CmpLeSint64(&left, &right);
    return result;
}
