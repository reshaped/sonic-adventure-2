/* Transactional PVM loading for the Dreamcast port. */
#ifndef ENGINE_DC_PVM_H
#define ENGINE_DC_PVM_H

#include <ninja.h>

/* Read the PVM's directory and validate all chunks before Ninja opens it. */
Uint32 texture_pvm_count(const void *archive, Uint32 size);
Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target);
/* texture_load_pvm_memory for an archive of size bytes at the start of
 * capacity bytes that the load may write. a chunk that ends short of what
 * Ninja copies of it grows with zeros. where capacity holds the grown
 * archive, it grows where it lies and the load takes no heap for it.
 * otherwise it grows in a heap copy of the archive, as texture_load_pvm_memory
 * grows every one. the bytes from size up to capacity don't survive the
 * call */
Bool texture_load_pvm_buffer(void *buffer, Uint32 size, Uint32 capacity,
                             NJS_TEXLIST *target);
/* Release the reference owned by a previous publication and clear its handles.
 * A target with no recorded acquisition has no references to release. */
void texture_discard_pvm(NJS_TEXLIST *target);
/* Release exactly the handles acquired for target. A bank reset can follow
 * an overlay change, so the global hook never dereferences the old lists. */
Bool texture_release_owned(NJS_TEXLIST *target);
void texture_forget_all_pvms(void);

#endif
