/* the SFA library's build text and its count of uses */
#include <adx_sfa.h>
#include <sg_xpt.h>

/* the member exports these data, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
const char *sfa_build = "\nSFA Ver.5.88 Build:Jun 23 2000 11:59:29\n";
Sint32 sfa_init_cnt = 0;
/* NOLINTEND(misc-use-internal-linkage) */

void SFA_Init(void)
{
    sfa_init_cnt++;
}

void SFA_Finish(void)
{
    sfa_init_cnt--;
}
