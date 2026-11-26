/* the camera editor's record and colors, which the editor's sources share */
#ifndef CAMERA_EDITOR_H
#define CAMERA_EDITOR_H
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camlin.h"
#include "camera/zones.h"

/* the editors' opaque colors, as ARGB words */
#define RED 0xffff0000
#define YELLOW 0xffffff00
#define GREEN 0xff00ff00

/* what the camera editor and the camlin editor keep from frame to frame */
struct camera_editor {
    Uint8 unk_00[4];
    Bool camlin_open; /* TRUE from camlin_editor_open to camlin_editor_close */
    struct camera_zone held_zone; /* the zone the camera editor holds */
    struct camlin_node held; /* the node the camlin editor holds */
    /* 1 where the held zone came from table 0, which every player shares,
     * else 0. camset_pick_up sets it by the table it took the zone from. the
     * BASE page's left sets it to 1, and right to 0 */
    Sint32 shared;
    /* the point the camlin editor moves on its FRIEND page.
     * camlin_link_nearest and camlin_unlink_nearest move it to the node they
     * find */
    NJS_POINT3 link_cursor;
    Uint8 unk_e0[0x9c];
    NJS_POINT3 camera_pos; /* where the EDITOR2 mode puts the camera */
    /* the point the camlin and camset searches measure from. each page of the
     * camlin editor puts it at the point the page moves. the EDITOR and EDITOR2
     * modes look at it */
    NJS_POINT3 cursor;
    Uint8 unk_194[0x14];
    Angle roll; /* the roll the EDITOR2 mode gives the camera */
};
extern struct camera_editor camera_editor;

#endif
