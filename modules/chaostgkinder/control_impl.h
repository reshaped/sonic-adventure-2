/* what the controls share: control_destroy, and control_update, which does
 * nothing. they are static, so each control's object holds its own copy of
 * them, as the image does */
#ifndef CHAOSTGKINDER_CONTROL_IMPL_H
#define CHAOSTGKINDER_CONTROL_IMPL_H
#include <shinobi.h>

#include "chaostgkinder/widget.h"

static void control_destroy(struct kinder_widget *widget)
{
    syFree(widget->state);
    kinder_widget_free(widget);
}

static void control_update(struct kinder_widget *widget)
{
}
#endif
