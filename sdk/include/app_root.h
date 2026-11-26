/* the record in low memory that can name an app root. an app root is a
 * directory of the disc that stands in for the root. the file system and
 * syBtExit read the record */
#ifndef APP_ROOT_H
#define APP_ROOT_H
#include <sg_xpt.h>

/* the bytes of the record's name */
#define APP_ROOT_NAME_SIZE 12
struct app_root_info {
    /* the app root's name, ended by a space or by the field's end */
    char root[APP_ROOT_NAME_SIZE];
    Sint8 mode; /* APP_ROOT_SET when the record names an app root */
    /* the track gdFsDaPlayG plays for a track of APP_TRACK_BASE */
    Sint8 track;
    /* where mode is APP_ROOT_SET, a 1 here has syBtExit call the code at
     * 0x8c008300, not the boot ROM */
    Sint8 unk_e;
};
#define APP_ROOT_INFO ((struct app_root_info *)0x8c0080f0)
#define APP_ROOT_SET 1
#define APP_TRACK_BASE 4

#endif
