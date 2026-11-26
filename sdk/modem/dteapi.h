/* dteapi: the modem's interface to the host, as far as the sources use it */
#ifndef MODEM_DTEAPI_H
#define MODEM_DTEAPI_H
#include <access/dte_if.h> /* IWYU pragma: export */
#include <sg_xpt.h>

#include "cmm.h"

/* the host's side of the port, from an Open that succeeds until Close, and
 * null otherwise */
extern struct SCC_PC *glbScc_Pc;

/* the data pump's interrupt handler, which target_InstallDpIrq hooks */
void ModemIntrCall(void *arg);
/* fill transfer's data with up to its length of the host's bytes, through the
 * host's get_tx. it returns what moved. the call moves nothing for a transfer
 * of length 0, or while the modem isn't running. the modem runs from the host's
 * ready event until its next Open or Close */
struct ddal_result *dteGetTxBuffer(struct ddalDataTransfer *transfer);
/* give the host transfer's bytes, through the host's set_rx, and return
 * what moved. while the modem isn't running, the call moves nothing */
struct ddal_result *dteSetRxBuffer(struct ddalDataTransfer *transfer);
/* while the modem runs, pass status to the host's set_status */
void dteSetModemStatusRegister(struct dteModemStatusRegister *status);

#endif
