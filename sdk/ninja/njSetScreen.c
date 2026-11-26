#include <ninja.h>

void njSetScreen(NJS_SCREEN *s)
{
    _nj_screen_.dist = s->dist;
    _nj_screen_.cx = s->cx;
    _nj_screen_.cy = s->cy;
}
