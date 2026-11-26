/* lnk_time: the firmware's identification in 22 characters: SD, then its
 * build's date and time. a build at 22:48:23 on 2 October 2000 reads
 * SD 3000.10.02.22.48.23. the year's first digit is one past the build's */
#ifndef MODEM_LNK_TIME_H
#define MODEM_LNK_TIME_H
#include <sg_xpt.h>

/* lnk_timeDisplayCompiledTime sends the identification, and DisplayTime
 * sends text, each between two line ends */
void lnk_timeDisplayCompiledTime(void);
void DisplayTime(Uint8 *text);
/* copy the identification and its nul to destination */
void CompiledTime(Uint8 *destination);

#endif
