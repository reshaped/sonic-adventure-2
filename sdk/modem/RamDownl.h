/* RamDownl: the data pump's RAM modules, downloaded from the Rockwell
 * database */
#ifndef MODEM_RAMDOWNL_H
#define MODEM_RAMDOWNL_H
#include <sg_xpt.h>
/* RamDownload's banks: the signal processor's modules, and the controller's */
#define RAM_BANK_SIGNAL 0
#define RAM_BANK_CONTROLLER 1
/* the controller's modules for V.34 and V.90. the pump asks for a module by
 * writing the module's id to abcode, and V.90 needs V34_MODULE first */
#define V34_MODULE 67
#define V90_MODULE 0x4a
/* download the eleven modules the pump needs after its reset, in the order it
 * needs them. it returns 0 at the first that fails, and 1 where none does */
char InitialDownloadRamModule(void);
/* PostV8DownloadRamModule's results */
#define RAM_DOWNLOAD_WAITING 0
#define RAM_DOWNLOAD_DONE 1
#define RAM_DOWNLOAD_FAILED 2
/* download the modules of the modulation the V.8 exchange chose, once the pump
 * asks for V34_MODULE. the call returns RAM_DOWNLOAD_WAITING until then,
 * RAM_DOWNLOAD_DONE once the download succeeds, and RAM_DOWNLOAD_FAILED once
 * it fails */
int PostV8DownloadRamModule(void);
/* download module id from the controller's modules where bank is
 * RAM_BANK_CONTROLLER, and from the signal processor's otherwise. with request
 * clear, the pump must already be asking for id. wait has no effect, and
 * custom must be 0. it returns 1, or 0 where the download fails */
char RamDownload(Uint8 bank, Uint8 id, char wait, Uint8 custom, char request);
extern Uint8 K56SavedConf, K56SavedOrg, K56SavedEarc, K56SavedArc;
extern Uint8 K56SavedData, K56SavedAutomode, K56SavedTxSpeed;
#endif
