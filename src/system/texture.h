/* loading texture lists from PVM files: a numbered set of lists, a list by
 * its textures' positions, and lists by their textures' names */
#ifndef SYSTEM_TEXTURE_H
#define SYSTEM_TEXTURE_H
#include <ninja.h>
#include <sg_xpt.h>

/* a texture list and the PVM that fills it, named without its .PRS suffix.
 * a null name marks a list that no PVM fills. a table of these ends at an
 * entry whose texlist is null */
struct texture_pvm {
    const char *name;
    NJS_TEXLIST *texlist;
};

/* the lists of texture set 0, which texture_load_set fills from the PVMs
 * CONSOLE, AL_PANEL and CHAOSDRIVE_TEX. a fill writes a list's textures, not
 * the list itself. so al_panel_texlist can be const, as hud/item_stock.c
 * defines it */
extern NJS_TEXLIST console_texlist;
extern const NJS_TEXLIST al_panel_texlist;
extern NJS_TEXLIST chaosdrive_tex_texlist;

#ifdef NONMATCHING
/* Load every owner before publishing its handles. The fixed list retains its
 * names and count. Failure clears the target and relinquishes earlier PVM
 * references. The allocated variant returns NULL on failure. */
Bool texture_load_set(int set);
void texture_release_set(void);
Bool texture_load_pvm(const char *name, NJS_TEXLIST *texlist);
Bool texture_load_pvm_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                         Sint32 size);
Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target);
/* Release acquired handles and empty the list. Repeated calls are safe. */
void texture_discard_pvm(NJS_TEXLIST *target);
NJS_TEXLIST *texture_load_pvm_alloc(const char *name);
Sint32 texture_load_prs_file(const char *name, NJS_TEXLIST *texlist);
/* Shared destinations borrow the owners' handles by name. All uploads finish
 * synchronously. A missing archive or unresolved name returns FALSE. */
Bool texture_load_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
/* Only the listed borrowed slots may remain empty. Null entries are ignored.
 * Every archive and every other borrowed slot must resolve. */
Bool texture_load_shared_with_optional(struct texture_pvm *pvms,
                                       NJS_TEXLIST ***groups,
                                       const NJS_TEXNAME *const *optional,
                                       Uint32 optional_count);
Bool texture_await_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
void texture_release_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
#else
/* load the lists of the texture set numbered set. each loads from its PVM,
 * or with njLoadTexture where the set names none. set must be 0, unchecked. it
 * leaves no list recorded for texture_release_set */
void texture_load_set(int set);
/* release each list texture_load_set left recorded, which is none */
void texture_release_set(void);
/* load name.PRS, a PVM, into a scratch list of 512 textures. each texture of
 * texlist then takes the texaddr and attr of the scratch list's texture at
 * its index. texlist must hold at most 512 textures, unchecked. a texture
 * past the PVM's last takes what an earlier load left in the scratch list.
 * so does every texture when the file won't open in three tries.
 *
 * the file is read into the end of WORK_AREA and decodes from its start.
 * nothing checks that the decoded PVM stops short of the file. a read that
 * fails three times goes unchecked. the PVM then decodes from whatever those
 * bytes of WORK_AREA held */
void texture_load_pvm(const char *name, NJS_TEXLIST *texlist);
/* texture_load_pvm through a buffer of the caller's, size bytes at buffer,
 * in place of WORK_AREA. name is the whole file name, suffix and all */
void texture_load_pvm_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                         Sint32 size);
/* load name.PRS as texture_load_pvm does, and return a new list of every
 * texture the PVM holds. each entry has its texaddr and attr, and no name.
 * the list and its entries are one block from the Shinobi heap, for syFree
 * to free. freeing it leaves the textures loaded; njReleaseTexture on the
 * list releases them. returns null when the heap can't give the block. the
 * textures then stay loaded, with no list to release them by. the file must
 * open, unchecked: otherwise nothing sets the count that sizes the block */
NJS_TEXLIST *texture_load_pvm_alloc(const char *name);

/* in the three calls below, pvms is a table of struct texture_pvm, and
 * groups is a null-ended array of null-ended arrays of lists */

/* for each entry of pvms, load its list as texture_load_pvm does, unless its
 * name is null. then each texture in groups' lists takes attr and texaddr
 * from the entry's first texture of its name. names match whatever the case
 * of their letters */
void texture_load_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
/* wait until every texture in groups' lists has a texaddr. it spins on each
 * in turn, and never returns while one has none. pvms is unread */
void texture_await_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
/* clear the attr and texaddr of every texture in groups' lists, then release
 * each list of pvms that has a name */
void texture_release_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups);
#endif
#ifdef NONMATCHING
/* TRUE when the selected slot has acquired a texture handle. */
Bool texture_slot_loaded(const NJS_TEXLIST *texlist, Uint32 index);
#endif
/* TRUE when every texture of texlist has a texaddr */
Bool texture_list_loaded(const NJS_TEXLIST *texlist);

#endif
