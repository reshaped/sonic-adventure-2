/* ntLDBrowser, the Network SDK's way out of the game to the Dream Passport
 * browser */
#include <nt_utl.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* the video chip's punch-through alpha threshold register, through P2, the
 * uncached area */
#define PT_ALPHA_REF (*(volatile Uint32 *)0xa05f811c)

/* where the loader is read and run, just past the ntUtl record */
#define LD_ADDRESS ((void *)0x8c008300)

/* the game's own exit, which no header in sdk/include declares */
void sbExitSystem(void);

static const char *const ld_build =
    "\nntLD Ver 1.04 Build:Jan 15 2001 11:13:14\n";

/* load fname from the disc's root to address, and stop the game with
 * sbExitSystem. then run the loaded code, or where the file didn't open or
 * read, leave through syBtExit */
/* @hack redundant: unused, a parameter it never reads */
static Sint32 ld_load_and_run(const char *fname, void *address, Sint32 unused)
{
    GDFS gdfs;
    Sint32 nsct;
    Sint32 result;

    result = 0;
    gdFsChangeDir("/");
    gdfs = gdFsOpen(fname, NULL);
    if (gdfs != NULL) {
        gdFsGetFileSctSize(gdfs, &nsct);
        if (gdFsRead(gdfs, nsct, address) != 0) {
            result = -1;
        }
        gdFsClose(gdfs);
    } else {
        result = -1;
    }
    sbExitSystem();
    if (result != 0) {
        syBtExit();
    }
    PT_ALPHA_REF = 0xff;
    ((void (*)(void))address)();
    /* @hack redundant: a result, 1, that no caller reads, where a void
     * function would do */
    return 1;
}

void ntLDBrowser(void)
{
    ld_load_and_run("sg_dpldr.bin", LD_ADDRESS, 1);
}
