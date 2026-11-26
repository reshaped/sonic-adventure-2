/* the labels of an object editor's fields, which no code reads. each array but
 * the last rounds its string up to a multiple of 4 bytes */
struct boss_editor_labels {
    char angle_x[12];
    char angle_y[12];
    char shape[12];
    char scale_x[12];
    char scale_y[12];
    char scale_z[9];
};
static const struct boss_editor_labels editor_labels = {
#include "boss_last1/editor_labels/editor_labels.inc"
};
