#include "chaostgkinder/widget.h"

struct kinder_widget *
kinder_custom_create(const struct kinder_widget_desc *desc)
{
    const struct kinder_custom_desc *factory = desc->settings;
    struct kinder_widget *widget;

    /* @hack spelling: widget, a local for the result, where returning
     * factory->create's would do */
    widget = factory->create(desc);
    return widget;
}
