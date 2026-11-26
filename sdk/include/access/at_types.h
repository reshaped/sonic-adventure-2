/* ACCESS's buffer lists: up to AT_MAX_BUFFERS buffers, each a length and an
 * address of an AT_PTR_TYPE kind */
#ifndef ACCESS_AT_TYPES_H
#define ACCESS_AT_TYPES_H
#include <sg_xpt.h>

#define AT_PTR_TYPE_NONE 0
#define AT_PTR_TYPE_LOCAL 1
#define AT_PTR_TYPE_POOL 2
#define AT_PTR_TYPE_EXTERNAL 3
#define AT_MAX_BUFFERS 12

typedef struct _AT_Vptr {
    Uint32 buff;
    Uint8 type;
} AT_Vptr;

typedef struct {
    Sint16 len;
    AT_Vptr vptr;
} AT_VptrArray;

/* a list handed to UDP_Send, to IP or below runs against the wire.
 * buffers[bufnum - 1] goes out first, and each layer appends its header as
 * the last buffer. TCP_Write's list and a received list run with the wire,
 * from buffers[0] */
typedef struct _AT_MaxBuffs {
    Sint32 bufnum;
    Sint32 total_len;
    AT_VptrArray buffers[AT_MAX_BUFFERS];
} AT_MaxBuffs;
#endif
