/* the startup that START_SYS jumps to, and the calls of the constructor
 * and destructor tables */
#include <sg_ov.h>
#include <sg_sysrt.h>
#include <sg_xpt.h>

/* the stack's lowest word, and the word past its top. _INIT fills the stack
 * with STACK_FILL, "SEGA" in memory order */
#define STACK_BOTTOM ((Uint32 *)0x8c00c000)
#define STACK_TOP ((Uint32 *)0x8c00f400)
#define STACK_FILL 0x41474553

extern void main(void);

/* the startup exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
char *sygStartBuild = "\nsyStart Ver 1.17 Build:Dec 01 1999 12:17:57\n";
void _INIT(void);
void _INITLIBH(void);
void _FINISHLIBH(void);
void _INITSCT(void);
/* NOLINTEND(misc-use-internal-linkage) */

void _INIT(void)
{
    /* @hack redundant: version, a volatile local that holds the version
     * text's address, which nothing reads */
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    const char *volatile version;
    Uint32 *word;

    version = __0ShinobiVersion;
    for (word = STACK_BOTTOM; word < STACK_TOP; word++) {
        *word = STACK_FILL;
    }
    _INITSCT();
    _INITLIBH();
    main();
    _FINISHLIBH();
}

void _INITLIBH(void)
{
    ovMdlEntryTbl(_DSGLH_BGN);
}

void _FINISHLIBH(void)
{
    ovMdlDeleteTbl(_DSGLH_BGN);
}

void _INITSCT(void)
{
    Uint32 *word;
    Uint8 *dst;
    Uint8 *src;

    for (word = _B_BGN; word < _B_END; word++) {
        *word = 0;
    }
    for (dst = _D_BGN, src = _D_ROM; dst < _D_END; dst++, src++) {
        *dst = *src;
    }
    for (dst = _BSG_BGN; dst < _BSG_END; dst++) {
        *dst = 0;
    }
    for (dst = _DSG_BGN, src = _DSG_ROM; dst < _DSG_END; dst++, src++) {
        *dst = *src;
    }
}

void syStartGlobalConstructor(void)
{
    /* @hack type: the entries as volatile, where plain ones would do */
    volatile table_func *entry;

    for (entry = _PRESG_BGN; entry < _PRESG_END; entry++) {
        if (*entry != NULL && *entry != NO_FUNCTION) {
            (*entry)();
        }
    }
    for (entry = _PRE_BGN; entry < _PRE_END; entry++) {
        if (*entry != NULL && *entry != NO_FUNCTION) {
            (*entry)();
        }
    }
}

void syStartGlobalDestructor(void)
{
    /* @hack type: the entries as volatile, where plain ones would do */
    volatile table_func *entry;

    for (entry = _POSTSG_BGN; entry < _POSTSG_END; entry++) {
        if (*entry != NULL && *entry != NO_FUNCTION) {
            (*entry)();
        }
    }
    for (entry = _POST_BGN; entry < _POST_END; entry++) {
        if (*entry != NULL && *entry != NO_FUNCTION) {
            (*entry)();
        }
    }
}
