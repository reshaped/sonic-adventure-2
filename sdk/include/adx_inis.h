/* the data adx_inis.c defines for CRI ADX's streams */
#ifndef ADX_INIS_H
#define ADX_INIS_H
#include <cri_adxt.h>
#include <sg_xpt.h>

/* every stream's record, in use or free */
extern ADX_TALK adxt_obj[ADXT_MAX_OBJ];
/* ADXT_VsyncProc's calls since the outermost ADXT_Init */
extern Sint32 adxt_vsync_cnt;

#endif
