/* the macros the form controls share to take their bounds, lay out their
 * text and fill their quads. FORM_QUAD sets a quad's corners as
 * form_paint.h's STRIP_ macros do */
#ifndef CHAOSTGKINDER_FORM_LAYOUT_H
#define CHAOSTGKINDER_FORM_LAYOUT_H
#define FORM_BOUNDS(form, bounds)                                              \
    (form)->box.x = (bounds)->x;                                               \
    (form)->box.y = (bounds)->y;                                               \
    (form)->box.z = (bounds)->z;                                               \
    (form)->box.width = (bounds)->width;                                       \
    (form)->box.height = (bounds)->height;                                     \
    (form)->box.id = (bounds)->id

#define FORM_TEXT(run, form, string_value, left_value, top_value, depth)       \
    (run).layout.string = (string_value);                                      \
    (run).layout.length = KINDER_WHOLE_STRING;                                 \
    (run).layout.width = (form)->box.width;                                    \
    (run).layout.height = (form)->box.height;                                  \
    (run).layout.spacing = KINDER_TEXT_GAP;                                    \
    (run).layout.leading = KINDER_TEXT_GAP;                                    \
    (run).layout.x = 0.0F;                                                     \
    (run).layout.y = 0.0F;                                                     \
    (run).left = (left_value);                                                 \
    (run).top = (top_value);                                                   \
    (run).z = (depth);                                                         \
    (run).color = 0xff000000

#define FORM_QUAD(vertices, form, left, top, depth, color)                     \
    (vertices)[0].x = (vertices)[1].x = (left);                                \
    (vertices)[2].x = (vertices)[3].x = (left) + (form)->box.width;            \
    (vertices)[0].y = (vertices)[2].y = (top);                                 \
    (vertices)[1].y = (vertices)[3].y = (top) + (form)->box.height;            \
    (vertices)[0].z = (vertices)[1].z = (vertices)[2].z = (vertices)[3].z =    \
        (depth);                                                               \
    (vertices)[0].col = (vertices)[1].col = (vertices)[2].col =                \
        (vertices)[3].col = (color)

#endif
