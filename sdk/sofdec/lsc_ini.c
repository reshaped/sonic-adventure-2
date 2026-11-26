/* LSC's data, and the calls that start and end the library */
#include <cri_lsc.h>
#include <lsc_crs.h>
#include <lsc_ini.h>
#include <sg_xpt.h>
#include <string.h>

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
void lsc_EntrySvrInt(void);
void lsc_DeleteSvrInt(void);
/* NOLINTEND(misc-use-internal-linkage) */

const char *lsc_build = "\nCRI LSC Ver 1.10 Build:Jul 12 2000 15:40:17\n";
char lsc_obj_mark[] = "MARK:lsc_obj";
Sint32 lsc_init_cnt = 0;
LSC_OBJ lsc_obj[LSC_OBJ_MAX];

void lsc_EntrySvrInt(void)
{
}

void lsc_DeleteSvrInt(void)
{
}

void LSC_Init(void)
{
    Sint32 mask;

    LSC_LockCrs(&mask);
    if (lsc_init_cnt == 0) {
        memset(lsc_obj, 0, sizeof(lsc_obj));
        lsc_EntrySvrInt();
        LSC_EntryErrFunc(NULL, NULL);
    }
    lsc_init_cnt++;
    LSC_UnlockCrs(&mask);
}

void LSC_Finish(void)
{
    Sint32 mask;
    Sint32 i;

    LSC_LockCrs(&mask);
    lsc_init_cnt--;
    if (lsc_init_cnt == 0) {
        for (i = 0; i < LSC_OBJ_MAX; i++) {
            if (lsc_obj[i].used == TRUE) {
                LSC_Destroy(&lsc_obj[i]);
            }
        }
        memset(lsc_obj, 0, sizeof(lsc_obj));
        lsc_DeleteSvrInt();
        LSC_EntryErrFunc(NULL, NULL);
    }
    LSC_UnlockCrs(&mask);
}
