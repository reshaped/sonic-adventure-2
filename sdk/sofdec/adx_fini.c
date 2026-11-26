/* the ADX file system's data, and the calls that start and end it */
#include <adx_fini.h>
#include <cri_adxf.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports adxf_build, though no header of the SDK declares it */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *const adxf_build = "\nADXF Ver.5.78 Build:Oct 11 2000 20:37:39\n";
/* NOLINTEND(misc-use-internal-linkage) */
ADX_FS adxf_obj[ADXF_OBJ_MAX];
ADXF_PTINFO *adxf_ptinfo[ADXF_PART_MAX];
Sint32 adxf_ocbi_fg;
ADXF_CMD_HSTRY adxf_cmd_hstry[ADXF_CMD_HSTRY_MAX];
Uint16 adxf_cmd_ncall[ADXF_CMD_NUM_MAX];
Sint32 adxf_hstry_no;
Sint32 adxf_flno;
ADXF adxf_ldptnw_hn;
Sint32 adxf_ldptnw_ptid;

void ADXF_Init(void)
{
    memset(adxf_obj, 0, sizeof(adxf_obj));
    memset(adxf_ptinfo, 0, sizeof(adxf_ptinfo));
    memset(adxf_cmd_hstry, ADXF_CMD_HSTRY_FILL, sizeof(adxf_cmd_hstry));
    memset(adxf_cmd_ncall, 0, sizeof(adxf_cmd_ncall));
    adxf_hstry_no = 0;
    adxf_ocbi_fg = 0;
    adxf_flno = 0;
    adxf_ldptnw_hn = NULL;
    adxf_ldptnw_ptid = ADXF_LDPTNW_NONE;
}

void ADXF_Finish(void)
{
    ADXF_CloseAll();
    adxf_ldptnw_ptid = ADXF_LDPTNW_NONE;
    adxf_ldptnw_hn = NULL;
    adxf_flno = 0;
    adxf_ocbi_fg = 0;
    adxf_hstry_no = 0;
    memset(adxf_cmd_ncall, 0, sizeof(adxf_cmd_ncall));
    memset(adxf_cmd_hstry, ADXF_CMD_HSTRY_FILL, sizeof(adxf_cmd_hstry));
    memset(adxf_ptinfo, 0, sizeof(adxf_ptinfo));
    memset(adxf_obj, 0, sizeof(adxf_obj));
}
