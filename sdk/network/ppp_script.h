/* the scripts that dial the modem and log in, and what the link keeps of the
 * call */
#ifndef NETWORK_PPP_SCRIPT_H
#define NETWORK_PPP_SCRIPT_H
#include <access/aveppp.h>
#include <sg_xpt.h>
struct ppp_statistics;
extern struct ppp_statistics _ppp_info;
/* the digits after the word in the modem's CONNECT line, or 0 where none
 * follow. a DIAL sets it once the modem connects */
extern long PP_ConnectRate;
/* run script, a line a command, in capitals or in lowercase:
 *
 *   DIAL text      send ATDT, or ATDP where dialtype isn't 0, then the
 *                  outside number where outside_line isn't 0, then text,
 *                  ending in a CR. then wait for the modem's answer
 *   SEND text      send text
 *   WAIT text, n   wait n seconds for text, matched in either case
 *   DELAY n        wait n seconds
 *
 * without n, a wait takes parameters' telephone timeout, as DIAL's does. an
 * argument ends at a space or a comma, or a quoted one at its closing quote,
 * where \r, \n, \\ and \" stand for their bytes. in a text, %t, %u, %p and %i
 * stand for parameters' telephone_number, login, password and signup, and %%
 * for a %. a field a macro names must not be NULL. a line, and a text as its
 * command sends it or waits for it, must fit in 255 bytes: the SHC build
 * checks neither. in the SHC build a macro expands in place only at the
 * start of a text or right after another macro. after any other byte, its
 * substitution lands at the first zero byte that follows, past bytes no
 * write set.
 *
 * the result is 0 after the last line, or the first command's failure.
 * AvepppErrorBusy is a DIAL the modem answers with BUSY, ERROR or DELAYED,
 * and AvepppErrorNoDialTone one it answers with NO CARRIER or NO DIALTONE.
 * AvepppErrorScript is any other: an unknown command, a send that fails, a
 * wait that runs out, or PPP_CloseRequest set during a wait */
short PP_GoScript(const Uint8 *script, AvepppOpenParam *parameters);
/* copy PP_GetPPPStatus's statistics into _ppp_info, which keeps its last
 * copy where that call fails */
void get_ppp_info(void);
#endif
