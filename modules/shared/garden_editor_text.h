/* the labels of the garden props' object editor fields, which no code reads.
 * each field rounds its string up to a multiple of 4 bytes */
#ifndef SHARED_GARDEN_EDITOR_TEXT_H
#define SHARED_GARDEN_EDITOR_TEXT_H

struct garden_prop_editor_text {
    char angle_x[12];
    char angle_y[20];
    char angle_z[12];
    char scale_x[28];
};

extern const struct garden_prop_editor_text garden_prop_editor_labels;

#endif
