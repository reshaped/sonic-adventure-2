/* the data adx_fini.c defines for the ADX file system */
#ifndef ADX_FINI_H
#define ADX_FINI_H
#include <cri_adxf.h>
#include <sg_xpt.h>

/* every handle, open or free */
extern ADX_FS adxf_obj[ADXF_OBJ_MAX];
/* the first partition of each partition number's chain, or null */
extern ADXF_PTINFO *adxf_ptinfo[ADXF_PART_MAX];
/* while adxf_ocbi_fg is TRUE, ADXF_ReadNw32 drops its buffer's lines from
 * the operand cache before each request. it drops them again as the request
 * ends, fails or stops. ADXF_SetOcbiSw sets adxf_ocbi_fg */
extern Sint32 adxf_ocbi_fg;
/* the command history. ADXF_CMD_HSTRY_FILL is the byte that fills each entry
 * not yet written */
extern ADXF_CMD_HSTRY adxf_cmd_hstry[ADXF_CMD_HSTRY_MAX];
#define ADXF_CMD_HSTRY_FILL 0xff
/* each command's calls so far, by the command's number */
extern Uint16 adxf_cmd_ncall[ADXF_CMD_NUM_MAX];
/* the history's next entry goes at adxf_hstry_no modulo ADXF_CMD_HSTRY_MAX */
extern Sint32 adxf_hstry_no;
/* the files that ADXF_LoadPartitionNw's load has listed */
extern Sint32 adxf_flno;
/* while ADXF_LoadPartitionNw's load runs, adxf_ldptnw_hn is the handle it
 * reads through, and adxf_ldptnw_ptid the partition it loads. while none
 * runs, they are null and ADXF_LDPTNW_NONE */
extern ADXF adxf_ldptnw_hn;
extern Sint32 adxf_ldptnw_ptid;
#define ADXF_LDPTNW_NONE (-1)

#endif
