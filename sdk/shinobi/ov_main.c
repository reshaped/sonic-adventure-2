/* the overlay library: the table, by ID, of the libraries that modules
 * share */
#include <machine.h>
#include <sg_ov.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <sj.h>
#include <string.h>

/* the words in low memory that hold the table's address and capacity */
#define LIBTBL (*(OVS_LIBTBL **)OVD_LIBTBL_ADR_PTR)
#define LIBTBL_MAX (*(Sint32 *)OVD_LIBTBL_ADR_MAX)

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the overlay library's version and build time */
const char *const ovg_build = "\nov Ver.1.00Build:Apr 23 1999 22:13:01\n";
/* the IDs of a module's start and end records, each a UUID's sixteen bytes
 * with no terminator */
/* NOLINTBEGIN(clang-diagnostic-unterminated-string-initialization) */
const char ovg_mdl_start_lidstr[16] = "Lib Handle Start";
const char ovg_mdl_end_lidstr[16] = "Lib Handle End  ";
/* NOLINTEND(clang-diagnostic-unterminated-string-initialization) */
/* ovLockCs masks every interrupt, and saves the mask it replaces in *cs.
 * ovUnlockCs puts *cs back */
void ovLockCs(Sint32 *cs);
void ovUnlockCs(const Sint32 *cs);
/* the table, and its capacity in *max. where no table is set, it sets up the
 * default table first */
OVS_LIBTBL *ovGetLibTbl(Sint32 *max);
/* the table's index of lib, or OVE_ERR_INVALID_LIB_HDL where it holds none */
Sint32 ovSearchLib(OVLIB lib);
/* the table's index of the library with ID lid, or OVE_ERR_INVALID_LIB_ID
 * where it holds none */
Sint32 ovSearchLibByUuid(UUID *lid);
/* NOLINTEND(misc-use-internal-linkage) */

/* the overlay library's own handle, which the module's records hold after
 * the start record */
#pragma section LH
/* NOLINTNEXTLINE(misc-use-internal-linkage) */
OVS_LIB_OBJ ovg_libhdl = {NULL, OVD_VERSION_NO, OVD_REVISION_NO, 0, 0,
                          0,    OVD_LID};
#pragma section

void ovLockCs(Sint32 *cs)
{
    *cs = get_imask();
    set_imask(IMASK_ALL);
}

void ovUnlockCs(const Sint32 *cs)
{
    set_imask(*cs);
}

OVS_LIBTBL *ovGetLibTbl(Sint32 *max)
{
    OVS_LIBTBL *table;

    /* @hack spelling: table, read before the test and again after it, where
     * LIBTBL in the test and the return would do */
    table = LIBTBL;
    if (table == NULL) {
        ovInit((void *)OVD_DEF_LIBTBL_ADR, OVD_DEF_LIBTBL_MAX);
    }
    table = LIBTBL;
    *max = LIBTBL_MAX;
    return table;
}

Sint32 ovSearchLib(OVLIB lib)
{
    Sint32 max;
    Sint32 i;
    OVS_LIBTBL *table;

    table = ovGetLibTbl(&max);
    for (i = 0; i < table->nlib; i++) {
        if (lib == table->entry[i].lib) {
            return i;
        }
    }
    return OVE_ERR_INVALID_LIB_HDL;
}

Sint32 ovSearchLibByUuid(UUID *lid)
{
    Sint32 max;
    Sint32 i;
    OVS_LIBTBL *table;
    OVLIB lib;

    table = ovGetLibTbl(&max);
    for (i = 0; i < table->nlib; i++) {
        /* @hack spelling: lib, where the entry's lib in the call would do */
        lib = table->entry[i].lib;
        if (memcmp(lid, &lib->lid, sizeof(UUID)) == 0) {
            return i;
        }
    }
    return OVE_ERR_INVALID_LIB_ID;
}

void ovInit(void *ovltbl, Sint32 nlib)
{
    LIBTBL = ovltbl;
    LIBTBL_MAX = nlib;
}

void ovFinish(void)
{
}

OVLIB ovLibOpen(UUID *lid)
{
    Sint32 cs;
    Sint32 max;
    Sint32 i;
    OVS_LIBTBL *table;
    OVLIB lib;

    ovLockCs(&cs);
    table = ovGetLibTbl(&max);
    i = ovSearchLibByUuid(lid);
    if (i >= 0) {
        lib = table->entry[i].lib;
        table->entry[i].ref++;
    } else {
        lib = NULL;
    }
    ovUnlockCs(&cs);
    return lib;
}

void ovLibClose(UUID *lid)
{
    Sint32 cs;
    Sint32 max;
    Sint32 i;
    OVS_LIBTBL *table;

    ovLockCs(&cs);
    table = ovGetLibTbl(&max);
    i = ovSearchLibByUuid(lid);
    if (i >= 0) {
        table->entry[i].ref--;
    }
    ovUnlockCs(&cs);
}

OVE_ERR ovLibEntryTbl(OVLIB lib)
{
    Sint32 cs;
    Sint32 max;
    Sint32 i;
    OVS_LIBTBL *table;

    ovLockCs(&cs);
    table = ovGetLibTbl(&max);
    i = ovSearchLibByUuid(&lib->lid);
    if (i < 0) {
        i = table->nlib;
        table->entry[i].lib = lib;
        table->entry[i].ref = 0;
        table->nlib++;
    }
    ovUnlockCs(&cs);
    return OVE_ERR_OK;
}

OVE_ERR ovLibDeleteTbl(OVLIB lib)
{
    Sint32 cs;
    Sint32 max;
    Sint32 i;
    Sint32 found;
    OVE_ERR err;
    OVS_LIBTBL *table;

    ovLockCs(&cs);
    table = ovGetLibTbl(&max);
    /* @hack spelling: found, a copy of the index the loop starts from, where
     * i would do */
    found = ovSearchLib(lib);
    if (found >= 0) {
        table->nlib--;
        for (i = found; i < table->nlib; i++) {
            table->entry[i].lib = table->entry[i + 1].lib;
            table->entry[i].ref = table->entry[i + 1].ref;
        }
        table->entry[i].lib = NULL;
        table->entry[i].ref = 0;
        err = OVE_ERR_OK;
    } else {
        err = OVE_ERR_INVALID_LIB_ID;
    }
    ovUnlockCs(&cs);
    return err;
}

OVLIB_IF ovLibGetLibIf(OVLIB lib)
{
    return lib->lif;
}

void ovLibGetVersion(OVLIB lib, Uint8 *ver, Uint8 *revi)
{
    *ver = lib->ver;
    *revi = lib->revi;
}

UUID *ovLibGetUuid(OVLIB lib)
{
    return &lib->lid;
}

OVE_ERR ovMdlEntryTbl(OVMDL ovmdl)
{
    /* @hack spelling: start and end, pointers to the records' IDs, where the
     * IDs named in the calls would do */
    const char *start = ovg_mdl_start_lidstr;
    const char *end = ovg_mdl_end_lidstr;
    OVLIB lib;
    Sint32 i;

    if (memcmp(&ovmdl->libobj[0].lid, start, sizeof(UUID)) != 0) {
        return OVE_ERR_INVALID_FORMAT;
    }
    for (i = 1;; i++) {
        lib = &ovmdl->libobj[i];
        if (memcmp(&lib->lid, end, sizeof(UUID)) == 0) {
            break;
        }
        ovLibEntryTbl(lib);
    }
    return OVE_ERR_OK;
}

OVE_ERR ovMdlDeleteTbl(OVMDL ovmdl)
{
    /* @hack spelling: start and end, pointers to the records' IDs, where the
     * IDs named in the calls would do */
    const char *start = ovg_mdl_start_lidstr;
    const char *end = ovg_mdl_end_lidstr;
    OVLIB lib;
    Sint32 i;

    if (memcmp(&ovmdl->libobj[0].lid, start, sizeof(UUID)) != 0) {
        return OVE_ERR_INVALID_FORMAT;
    }
    for (i = 1;; i++) {
        lib = &ovmdl->libobj[i];
        if (memcmp(&lib->lid, end, sizeof(UUID)) == 0) {
            break;
        }
        ovLibDeleteTbl(lib);
    }
    return OVE_ERR_OK;
}

Sint32 ovMdlExec(OVMDL ovmdl)
{
    /* @hack implicit: the module's result returned without a return
     * statement, where a return of the call would do */
#ifdef NONMATCHING
    /* GCC returns the module's result only through the return statement */
    return ((Sint32 (*)(OVMDL))((Uint32)ovmdl | P2_BASE))(ovmdl);
#else
    ((Sint32 (*)(OVMDL))((Uint32)ovmdl | P2_BASE))(ovmdl);
#endif
    /* NOLINTNEXTLINE(clang-diagnostic-return-type) */
}
