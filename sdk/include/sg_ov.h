/* Shinobi overlay library, as far as the sources use it */
#ifndef SG_OV_H
#define SG_OV_H
#include <sg_xpt.h>
#include <sj.h>

/* the overlay library's own ID, version and revision */
#define OVD_LID                                                                \
    {0xba7e0a00,                                                               \
     0xbda9,                                                                   \
     0x11d1,                                                                   \
     {0x8a, 0xeb, 0x00, 0x60, 0x08, 0x95, 0x62, 0x5f}}
#define OVD_VERSION_NO 0x01
#define OVD_REVISION_NO 0x00

typedef enum {
    OVE_ERR_OK = 0,
    OVE_ERR_INVALID_FORMAT = -1,
    OVE_ERR_INVALID_LIB_ID = -2,
    OVE_ERR_INVALID_LIB_HDL = -3
} OVE_ERR;

/* a library's handle, a record of the module that brings the library */
typedef struct {
    struct _ov_lif *lif;
    Uint8 ver;
    Uint8 revi;
    Sint16 rsv1;
    Sint32 rsv2;
    Sint32 rsv3;
    UUID lid;
} OVS_LIB_OBJ;
typedef OVS_LIB_OBJ *OVLIB;
typedef struct _ov_lif OVS_LIB_IF;
typedef OVS_LIB_IF *OVLIB_IF;

/* a module: 32 bytes of its own, then a start record, its library handles
 * and an end record. the start record's ID reads "Lib Handle Start". the end
 * record's ID reads "Lib Handle End  " */
typedef struct {
    Sint32 rsv[8];
    OVS_LIB_OBJ libobj[1]; /* the records, declared as one */
} OVS_MDL_OBJ;
typedef OVS_MDL_OBJ *OVMDL;

/* every module finds the library table through two words in low memory: the
 * table's address and its capacity */
#define OVD_LIBTBL_ADR_PTR 0x8c00fff0
#define OVD_LIBTBL_ADR_MAX 0x8c00fff4
/* the default table's address and capacity */
#define OVD_DEF_LIBTBL_ADR 0x8c00fc00
#define OVD_DEF_LIBTBL_MAX 120
/* the table: nlib libraries, each with its count of references */
typedef struct {
    Sint32 nlib;
    Sint32 rsv1;
    struct {
        OVLIB lib;
        Sint32 ref;
    } entry[1]; /* the libraries, declared as one */
} OVS_LIBTBL;

/* a call that reads the table finds it through OVD_LIBTBL_ADR_PTR. where that
 * word is null, the call first makes the default table the table, as ovInit
 * does. a call that changes the table masks every interrupt while it does.
 *
 * ovInit stores ovltbl and nlib in the two words. every call then finds the
 * table at ovltbl, with room for nlib libraries. it doesn't clear the table.
 * the table's own nlib must already count what it holds, 0 for none. nothing
 * clears the default table either. ovFinish does nothing */
void ovInit(void *ovltbl, Sint32 nlib);
void ovFinish(void);
/* ovLibOpen returns the library with ID lid, and counts a reference to it.
 * it returns NULL where the table holds none. ovLibClose takes a reference
 * to that library away, and does nothing where the table holds none */
OVLIB ovLibOpen(UUID *lid);
void ovLibClose(UUID *lid);
/* add lib to the table, with no references, unless the table holds a
 * library with lib's ID. it returns OVE_ERR_OK either way. nothing checks
 * the table's capacity */
OVE_ERR ovLibEntryTbl(OVLIB lib);
/* remove lib from the table, and move the libraries after it up one place.
 * it returns OVE_ERR_INVALID_LIB_ID where the table doesn't hold lib */
OVE_ERR ovLibDeleteTbl(OVLIB lib);
/* ovMdlEntryTbl passes each library handle of the module at ovmdl to
 * ovLibEntryTbl, and ovMdlDeleteTbl each to ovLibDeleteTbl. each returns
 * OVE_ERR_OK. the search for the end record has no bound. a module whose first
 * record isn't the start record returns OVE_ERR_INVALID_FORMAT, and changes
 * nothing */
OVE_ERR ovMdlEntryTbl(OVMDL ovmdl);
OVE_ERR ovMdlDeleteTbl(OVMDL ovmdl);
/* call the module at ovmdl, with ovmdl as its argument, through P2,
 * uncached. the call returns the module's result */
Sint32 ovMdlExec(OVMDL ovmdl);
OVLIB_IF ovLibGetLibIf(OVLIB lib);
void ovLibGetVersion(OVLIB lib, Uint8 *ver, Uint8 *revi);
UUID *ovLibGetUuid(OVLIB lib);

#endif
