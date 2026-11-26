/* the destroy and tick of the leaf forms: the label, the button and the
 * rectangle. a leaf's state is one allocation, which destroy frees with the
 * form */
#ifndef CHAOSTGKINDER_FORM_LEAF_IMPL_H
#define CHAOSTGKINDER_FORM_LEAF_IMPL_H
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/form.h"

static void form_leaf_destroy(struct kinder_form *form)
{
    syFree(form->state);
    syFree(form);
}

static void form_leaf_tick(struct kinder_form *form)
{
}

#endif
