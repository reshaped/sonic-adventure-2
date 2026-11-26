/* the data lsc_ini.c defines for LSC */
#ifndef LSC_INI_H
#define LSC_INI_H
#include <cri_lsc.h>
#include <sg_xpt.h>

/* a string that names LSC's version and build time */
extern const char *lsc_build;
/* the text "MARK:lsc_obj", which nothing in LSC reads */
extern char lsc_obj_mark[];
/* the LSC_Init calls that no LSC_Finish has matched */
extern Sint32 lsc_init_cnt;
/* every handle, in use or free. a free handle holds zeros */
extern LSC_OBJ lsc_obj[LSC_OBJ_MAX];

#endif
