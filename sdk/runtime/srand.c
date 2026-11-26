#include <rnext.h>
#include <stdlib.h>

void srand(unsigned int seed)
{
    _rnext = seed;
}
