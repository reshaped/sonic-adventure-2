/* the calls that start an ADX stream's play, which the library's starts
 * share */
#ifndef ADX_TLK_H
#define ADX_TLK_H
#include <cri_adxt.h>
#include <sj.h>

/* adxt_start_sj has adxt play the ADX data that sj holds, from its header.
 * it empties the output joints, and has the decoder and any AHX decoder read
 * sj. it starts the time and the loop count over, and resets ahxflg, lesct,
 * trpnsmpl and pstready_flag. the stream becomes ADXT_STAT_DECINFO. the call
 * sets no pmode and no lnkflg.
 *
 * adxt_start_stm starts adxt's stm reading into sjf, plays sjf as
 * adxt_start_sj does, and sets pmode to ADXT_PMODE_FNAME. stm reads no more
 * than 25 sectors until the server has read the header */
void adxt_start_sj(ADXT adxt, SJ sj);
void adxt_start_stm(ADXT adxt);

#endif
