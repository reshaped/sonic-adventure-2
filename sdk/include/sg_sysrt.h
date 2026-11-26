/* Shinobi's startup: the program's section bounds and its calls, as far as
 * the sources use them */
#ifndef SG_SYSRT_H
#define SG_SYSRT_H
#include <sg_ov.h>
#include <sg_xpt.h>

/* the Shinobi library's version, copyright and build time, as text */
extern const char __0ShinobiVersion[];

/* the program's entry holds these words in its tables, each the address of a
 * section's bound. a _BGN word gives a section's first byte, and an _END
 * word the byte past its last. a _ROM word gives the address of its data
 * section's initial values */
extern OVMDL _DSGLH_BGN;
extern Uint32 *_B_BGN, *_B_END;
extern Uint8 *_D_BGN, *_D_END, *_D_ROM;
extern Uint8 *_BSG_BGN, *_BSG_END;
extern Uint8 *_DSG_BGN, *_DSG_END, *_DSG_ROM;
/* the bounds of four tables of functions. two construct the objects of the
 * SDK's libraries and of the program, and two destruct them. an entry that
 * is null or NO_FUNCTION is no function */
typedef void (*table_func)(void);
extern table_func *_PRESG_BGN, *_PRESG_END;
extern table_func *_PRE_BGN, *_PRE_END;
extern table_func *_POSTSG_BGN, *_POSTSG_END;
extern table_func *_POST_BGN, *_POST_END;
#define NO_FUNCTION ((table_func)(-1))

/* syStartGlobalConstructor calls each function of the two constructor
 * tables, the SDK's libraries' first. syStartGlobalDestructor calls each of
 * the destructor tables in the same order. nothing in the program calls
 * either */
void syStartGlobalConstructor(void);
void syStartGlobalDestructor(void);

#endif
