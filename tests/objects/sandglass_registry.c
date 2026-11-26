/* sandglass_register where syCalloc can't give the table: the call enters
 * nothing, writes no slot and leaves the table null */
#include <assert.h>
#include <stdlib.h>

#include <shinobi.h>

#include "shared/sandglass_registry.h"
#include "system/task.h"

static int refuse;

void *syCalloc(Uint32 count, Uint32 size)
{
    return refuse ? NULL : calloc(count, size);
}

void syFree(void *block)
{
    free(block);
}

int main(void)
{
    static struct task glass;

    refuse = 1;
    assert(sandglass_register(&glass) == NULL);
    assert(sandglasses == NULL && sandglass_count == 0);
    refuse = 0;
    assert(sandglass_register(&glass) == &sandglasses[0]);
    assert(sandglass_count == 1);
    sandglass_unregister(&glass);
    assert(sandglasses == NULL && sandglass_count == 0);
    return 0;
}
