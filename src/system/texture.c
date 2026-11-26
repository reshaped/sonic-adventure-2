/* loading texture lists from PVM files: a numbered set of lists, a list by
 * its textures' positions, and lists by their textures' names */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "system/areas.h"
#include "system/loader.h"
#include "system/prs.h"
#include "system/texture.h"

#ifdef NONMATCHING
/* strlen and memcpy, and the PVM checks of engine/dc/pvm.h, which this side's
 * loaders use */
#include <string.h>

#include "engine/dc/pvm.h"
#endif

/* the lists set_texlists holds, the null that ends them included */
#define SET_TEXLISTS 32

/* the most textures texture_load_pvm can take from one PVM: the length of
 * its scratch list */
#define PVM_TEXTURES 512

/* each load tries an open, and then a read, up to TRIES times. after each
 * failed open it waits out OPEN_WAIT passes of an empty loop */
#define TRIES 3
#define OPEN_WAIT 1000

/* the bytes of a path load_prs builds, a name and its .PRS suffix */
#define PATH_SIZE 256

#ifdef NONMATCHING
#else
static Sint32 load_prs(const char *name, NJS_TEXLIST *texlist);
static Sint32 load_prs_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                          Sint32 size);
#endif
static Bool names_match(const char *name, const char *other);
#ifdef NONMATCHING
#else
static int copy_named(NJS_TEXLIST *to, NJS_TEXLIST *from);
#endif

/* the lists texture_release_set releases, up to a null */
static NJS_TEXLIST *set_texlists[SET_TEXLISTS];
#ifdef NONMATCHING
#else
/* the scratch list texture_load_pvm loads each PVM into. past the last PVM's
 * textures, it holds what earlier loads left */
static NJS_TEXNAME pvm_textures[PVM_TEXTURES];
#endif

/* texture set 0, the only one */
#include "system/texture/text.inc"

static struct texture_pvm set_0[] = {
    {TEXTURE_SET_CONSOLE, &console_texlist},
    {TEXTURE_SET_AL_PANEL, (NJS_TEXLIST *)&al_panel_texlist},
    {TEXTURE_SET_CHAOSDRIVE_TEX, &chaosdrive_tex_texlist},
    {NULL, NULL},
};

static struct texture_pvm *texture_sets[] = {
    set_0,
};

#ifdef NONMATCHING
/* read name's PRS file into buffer, decoded, and return its size. it returns
 * -1 where name is null or too long for PATH_SIZE, or where the read fails */
static Sint32 read_pvm(const char *name, Uint8 *buffer, Uint32 capacity)
{
    char path[PATH_SIZE];
    size_t length;

    if (name == NULL) {
        return -1;
    }
    length = strlen(name);
    if (length > sizeof(path) - sizeof(".PRS")) {
        return -1;
    }
    memcpy(path, name, length);
    memcpy(path + length, ".PRS", sizeof(".PRS"));
    return loader_read_prs_buffer(path, buffer, capacity);
}

/* texture_load_pvm, texture_load_pvm_in and texture_load_pvm_alloc hand
 * texture_load_pvm_buffer the whole room they read the archive into, so that
 * an archive with a short chunk grows there and takes no copy from the heap */
Bool texture_load_pvm(const char *name, NJS_TEXLIST *texlist)
{
    Sint32 bytes = read_pvm(name, WORK_AREA, WORK_AREA_END - WORK_AREA);
    return texture_load_pvm_buffer(WORK_AREA, bytes > 0 ? (Uint32)bytes : 0,
                                   WORK_AREA_END - WORK_AREA, texlist);
}

Bool texture_load_pvm_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                         Sint32 size)
{
    Sint32 bytes = -1;

    if (name != NULL && buffer != NULL && size > 0) {
        bytes = loader_read_prs_buffer(name, buffer, (Uint32)size);
    }
    if (bytes <= 0) {
        return texture_load_pvm_memory(buffer, 0, texlist);
    }
    return texture_load_pvm_buffer(buffer, (Uint32)bytes, (Uint32)size,
                                   texlist);
}

/* this side repairs the matching side's tag: it counts the textures of a PVM
 * that read whole, and makes no list from one that didn't */
NJS_TEXLIST *texture_load_pvm_alloc(const char *name)
{
    NJS_TEXLIST *list;
    Uint32 count;
    Sint32 bytes = read_pvm(name, WORK_AREA, WORK_AREA_END - WORK_AREA);

    if (bytes <= 0 || (count = texture_pvm_count(WORK_AREA, bytes)) == 0) {
        return NULL;
    }
    /* The checked directory bounds count at 512 before allocation. */
    list = syCalloc(1, sizeof(*list) + count * sizeof(*list->textures));
    if (list != NULL) {
        list->textures = (NJS_TEXNAME *)(list + 1);
        list->nbTexture = count;
        if (!texture_load_pvm_buffer(WORK_AREA, bytes,
                                     WORK_AREA_END - WORK_AREA, list)) {
            syFree(list);
            return NULL;
        }
    }
    return list;
}

Sint32 texture_load_prs_file(const char *name, NJS_TEXLIST *texlist)
{
    return texture_load_pvm_in(name, texlist, STAGE_SLOT,
                               DEMO_AREA - STAGE_SLOT)
               ? 1
               : -1;
}

/* this side repairs the matching side's tag: it records each list in a slot of
 * its own */
Bool texture_load_set(int set)
{
    struct texture_pvm *pvm;
    Uint32 count = 0;

    if (set < 0 ||
        (Uint32)set >= sizeof(texture_sets) / sizeof(texture_sets[0])) {
        return FALSE;
    }
    texture_release_set();
    for (pvm = texture_sets[set]; pvm != NULL && pvm->texlist != NULL; pvm++) {
        if (count >= SET_TEXLISTS - 1 ||
            !texture_load_pvm(pvm->name, pvm->texlist)) {
            texture_release_set();
            return FALSE;
        }
        set_texlists[count++] = pvm->texlist;
        set_texlists[count] = NULL;
    }
    return TRUE;
}

void texture_release_set(void)
{
    Uint32 i;
    for (i = 0; i < SET_TEXLISTS && set_texlists[i] != NULL; i++) {
        texture_discard_pvm(set_texlists[i]);
        set_texlists[i] = NULL;
    }
}
#else
void texture_load_set(int set)
{
    NJS_TEXLIST **list = set_texlists;
    struct texture_pvm *pvm = texture_sets[set];
    NJS_TEXLIST *texlist;

    if (pvm == NULL) {
        /* @hack type: the store through a volatile-qualified pointer, where
         * *list = NULL would do */
        *(NJS_TEXLIST *volatile *)list = NULL;
    } else {
        texlist = pvm->texlist;
        while (texlist != NULL) {
            if (pvm->name != NULL) {
                texture_load_pvm(pvm->name, texlist);
            } else {
                njLoadTexture(texlist);
            }
            /* @bug logic: list never advances, so each list is recorded over
             * the last in set_texlists[0] (MOV.L R14,@R12 at 0x8c06057c).
             * the null stored after the loop clears that slot (0x8c060586),
             * and texture_release_set then finds no list to release */
            *list = texlist;
            pvm++;
            texlist = pvm->texlist;
        }
        *list = NULL;
    }
}

void texture_release_set(void)
{
    NJS_TEXLIST **texlist;

    for (texlist = set_texlists; *texlist != NULL; texlist++) {
        njReleaseTexture(*texlist);
        *texlist = NULL;
    }
}

void texture_load_pvm(const char *name, NJS_TEXLIST *texlist)
{
    NJS_TEXLIST loaded;
    char names[PVM_TEXTURES * PVM_NAME_SIZE];
    NJS_TEXNAME *textures;
    int count;
    int i;

    njSetPvmTextureList(&loaded, pvm_textures, names, PVM_TEXTURES);
    load_prs(name, &loaded);
    count = texlist->nbTexture;
    textures = texlist->textures;
    for (i = 0; i < count; i++) {
        textures[i].texaddr = pvm_textures[i].texaddr;
        textures[i].attr = pvm_textures[i].attr;
    }
}

void texture_load_pvm_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                         Sint32 size)
{
    NJS_TEXLIST loaded;
    char names[PVM_TEXTURES * PVM_NAME_SIZE];
    NJS_TEXNAME *textures;
    int count;
    int i;

    njSetPvmTextureList(&loaded, pvm_textures, names, PVM_TEXTURES);
    load_prs_in(name, &loaded, buffer, size);
    count = texlist->nbTexture;
    textures = texlist->textures;
    for (i = 0; i < count; i++) {
        textures[i].texaddr = pvm_textures[i].texaddr;
        textures[i].attr = pvm_textures[i].attr;
    }
}

NJS_TEXLIST *texture_load_pvm_alloc(const char *name)
{
    NJS_TEXLIST loaded;
    char names[PVM_TEXTURES * PVM_NAME_SIZE];
    NJS_TEXLIST *texlist;
    NJS_TEXNAME *textures;
    /* @hack spelling: i, which holds the PVM's count and then counts down
     * from it, where a count of its own would do */
    int i;
    Uint32 size;

    njSetPvmTextureList(&loaded, pvm_textures, names, PVM_TEXTURES);
    load_prs(name, &loaded);
    /* @bug uninitialized: reads loaded.nbTexture, which nothing sets when the
     * file won't open. load_prs then returns before njLoadTexturePvmMemory,
     * and njSetPvmTextureList stores only the list's textures (0x8c114a2e).
     * MOV.L @(R0,R15),R14 at 0x8c0606fa reads the count as the stack left
     * it, and the count sizes the block and the copy */
    i = loaded.nbTexture;
    /* @hack spelling: size built in two statements, where one sum would
     * do */
    size = sizeof(NJS_TEXLIST);
    size += i * sizeof(NJS_TEXNAME);
    texlist = syCalloc(1, size);
    if (texlist == NULL) {
        return NULL;
    }
    /* @hack spelling: the assignment to textures inside the store to
     * texlist->textures, where a statement of its own would do */
    texlist->textures = textures = (NJS_TEXNAME *)(texlist + 1);
    texlist->nbTexture = i;
    for (i--; i >= 0; i--) {
        textures[i].texaddr = pvm_textures[i].texaddr;
        textures[i].attr = pvm_textures[i].attr;
    }
    return texlist;
}

/* load name.PRS into texlist, as texture_load_pvm says. it returns -1 when
 * every open fails, and njLoadTexturePvmMemory's result otherwise */
static Sint32 load_prs(const char *name, NJS_TEXLIST *texlist)
{
    char path[PATH_SIZE];
    Sint32 size;
    GDFS gdfs;
    int tries;
    int i;
    Sint32 sectors;
    Uint8 *file;

    sprintf(path, "%s.PRS", name);
    for (tries = TRIES; tries > 0; tries--) {
        gdfs = gdFsOpen(path, NULL);
        if (gdfs != NULL) {
            break;
        }
        for (i = 0; i < OPEN_WAIT; i++) {
        }
    }
    if (gdfs == NULL) {
        return -1;
    }
    gdFsGetFileSize(gdfs, &size);
    sectors = SECTORS(size);
    file = WORK_AREA_END - (sectors << SCT_SHIFT);
    for (tries = TRIES; tries > 0; tries--) {
        if (gdFsRead(gdfs, sectors, file) == GDD_ERR_OK) {
            break;
        }
    }
    gdFsClose(gdfs);
    prs_decompress(file, WORK_AREA);
    return njLoadTexturePvmMemory(WORK_AREA, texlist);
}

/* load the PRS file name, a PVM, into texlist. name is the whole file name,
 * suffix and all. the file is read into the bytes below DEMO_AREA. the PVM
 * decodes at STAGE_SLOT, over any overlay there. a read that fails three
 * times goes unchecked. returns -1 when every open fails, and
 * njLoadTexturePvmMemory's result otherwise. nothing calls it */
/* @hack redundant: external linkage, which no call needs, where static would
 * do */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 texture_load_prs_file(const char *name, NJS_TEXLIST *texlist)
{
    Sint32 size;
    GDFS gdfs;
    int tries;
    int i;
    Sint32 sectors;
    Uint8 *file;

    for (tries = TRIES; tries > 0; tries--) {
        gdfs = gdFsOpen(name, NULL);
        if (gdfs != NULL) {
            break;
        }
        for (i = 0; i < OPEN_WAIT; i++) {
        }
    }
    if (gdfs == NULL) {
        return -1;
    }
    gdFsGetFileSize(gdfs, &size);
    sectors = SECTORS(size);
    file = DEMO_AREA - (sectors << SCT_SHIFT);
    for (tries = TRIES; tries > 0; tries--) {
        if (gdFsRead(gdfs, sectors, file) == GDD_ERR_OK) {
            break;
        }
    }
    gdFsClose(gdfs);
    prs_decompress(file, STAGE_SLOT);
    return njLoadTexturePvmMemory(STAGE_SLOT, texlist);
}
/* NOLINTEND(misc-use-internal-linkage) */

/* load the PRS file name into texlist through size bytes at buffer, as
 * texture_load_pvm_in says. it returns what load_prs does */
static Sint32 load_prs_in(const char *name, NJS_TEXLIST *texlist, Uint8 *buffer,
                          Sint32 size)
{
    /* @hack redundant: path, PATH_SIZE bytes that nothing reads */
    char path[PATH_SIZE];
    Sint32 file_size;
    GDFS gdfs;
    int tries;
    int i;
    Sint32 sectors;
    Uint8 *file;

    for (tries = TRIES; tries > 0; tries--) {
        gdfs = gdFsOpen(name, NULL);
        if (gdfs != NULL) {
            break;
        }
        for (i = 0; i < OPEN_WAIT; i++) {
        }
    }
    if (gdfs == NULL) {
        return -1;
    }
    gdFsGetFileSize(gdfs, &file_size);
    sectors = SECTORS(file_size);
    /* @hack type: the address computed as an int, where buffer + size -
     * (sectors << SCT_SHIFT) on the pointer would do */
    file = (Uint8 *)((int)buffer - (sectors << SCT_SHIFT) + size);
    for (tries = TRIES; tries > 0; tries--) {
        if (gdFsRead(gdfs, sectors, file) == GDD_ERR_OK) {
            break;
        }
    }
    gdFsClose(gdfs);
    prs_decompress(file, buffer);
    return njLoadTexturePvmMemory(buffer, texlist);
}

#endif

/* TRUE when the names match, with A to Z taken as a to z */
static Bool names_match(const char *name, const char *other)
{
    char ch;
    char other_ch;

#ifdef NONMATCHING
    /* this side matches no null name, which the matching side reads through */
    if (name == NULL || other == NULL) {
        return FALSE;
    }
#endif
    for (;;) {
        ch = *name++;
        other_ch = *other++;
        if (ch >= 'A' && ch <= 'Z') {
            ch += 'a' - 'A';
        }
        if (other_ch >= 'A' && other_ch <= 'Z') {
            other_ch += 'a' - 'A';
        }
        /* @hack spelling: the mismatch leaves the loop by a break, where
         * return FALSE here, with no return after the loop, would do */
        if (ch != other_ch) {
            break;
        }
        if (ch == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

#ifdef NONMATCHING
/* Shared lists borrow the owner's handles. A load is synchronous, so a missing
 * name is terminal and a caller can test readiness without waiting a frame. */
Bool texture_await_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    NJS_TEXLIST **list;
    (void)pvms;

    for (; *groups != NULL; groups++) {
        for (list = *groups; *list != NULL; list++) {
            if (!texture_list_loaded(*list)) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

void texture_release_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    NJS_TEXLIST **list;
    Uint32 i;

    for (; pvms->texlist != NULL; pvms++) {
        if (pvms->name != NULL) {
            texture_discard_pvm(pvms->texlist);
        }
    }
    for (; *groups != NULL; groups++) {
        for (list = *groups; *list != NULL; list++) {
            for (i = 0; i < (*list)->nbTexture; i++) {
                (*list)->textures[i].texaddr = 0;
                (*list)->textures[i].attr = 0;
            }
        }
    }
}

/* A name takes the first match in each archive, with the last archive
 * taking precedence. Unnamed PVM entries supply already resident textures. */
static NJS_TEXNAME *shared_texture(struct texture_pvm *pvms, const char *name)
{
    NJS_TEXNAME *found = NULL;
    NJS_TEXNAME *entry;
    Uint32 i;

    for (; pvms->texlist != NULL; pvms++) {
        for (i = 0; i < pvms->texlist->nbTexture; i++) {
            entry = &pvms->texlist->textures[i];
            if (entry->texaddr != 0 && names_match(entry->filename, name)) {
                found = entry;
                break;
            }
        }
    }
    return found;
}

static Bool optional_slot(const NJS_TEXNAME *slot,
                          const NJS_TEXNAME *const *optional,
                          Uint32 optional_count)
{
    Uint32 i;

    for (i = 0; optional != NULL && i < optional_count; i++) {
        if (optional[i] == slot) {
            return TRUE;
        }
    }
    return FALSE;
}

static Bool resolve_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups,
                           const NJS_TEXNAME *const *optional,
                           Uint32 optional_count, Bool publish)
{
    NJS_TEXLIST **list;
    NJS_TEXNAME *target;
    NJS_TEXNAME *source;
    Uint32 i;

    for (; *groups != NULL; groups++) {
        for (list = *groups; *list != NULL; list++) {
            for (i = 0; i < (*list)->nbTexture; i++) {
                target = &(*list)->textures[i];
                source = shared_texture(pvms, target->filename);
                if (source == NULL &&
                    !optional_slot(target, optional, optional_count)) {
                    return FALSE;
                }
                if (publish) {
                    target->attr = source != NULL ? source->attr : 0;
                    target->texaddr = source != NULL ? source->texaddr : 0;
                }
            }
        }
    }
    return TRUE;
}

Bool texture_load_shared_with_optional(struct texture_pvm *pvms,
                                       NJS_TEXLIST ***groups,
                                       const NJS_TEXNAME *const *optional,
                                       Uint32 optional_count)
{
    struct texture_pvm *pvm;

    for (pvm = pvms; pvm->texlist != NULL; pvm++) {
        if (pvm->name != NULL && !texture_load_pvm(pvm->name, pvm->texlist)) {
            texture_release_shared(pvms, groups);
            return FALSE;
        }
    }
    if (!resolve_shared(pvms, groups, optional, optional_count, FALSE)) {
        texture_release_shared(pvms, groups);
        return FALSE;
    }
    return resolve_shared(pvms, groups, optional, optional_count, TRUE);
}

Bool texture_load_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    return texture_load_shared_with_optional(pvms, groups, NULL, 0);
}
#else
void texture_load_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    NJS_TEXLIST ***group;
    NJS_TEXLIST **list;

    for (; pvms->texlist != NULL; pvms++) {
        if (pvms->name != NULL) {
            texture_load_pvm(pvms->name, pvms->texlist);
        }
        /* @hack spelling: group and list stepped where each is read, where
         * both stepped in the for's third clause would do */
        for (group = groups; *group != NULL;) {
            for (list = *group++; *list != NULL;) {
                copy_named(*list++, pvms->texlist);
            }
        }
    }
}

void texture_await_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    NJS_TEXLIST ***group;
    NJS_TEXLIST **list;
    NJS_TEXNAME *texture;
    int count;
    int i;

    /* @hack spelling: group stepped where the lists are read from it, not in
     * the for's third clause */
    for (group = groups; *group != NULL;) {
        for (list = *group++; *list != NULL; list++) {
            count = (*list)->nbTexture;
            texture = (*list)->textures;
            for (i = 0; i < count; i++) {
                while (texture->texaddr == 0) {
                }
                texture++;
            }
        }
    }
}

void texture_release_shared(struct texture_pvm *pvms, NJS_TEXLIST ***groups)
{
    NJS_TEXLIST ***group;
    NJS_TEXLIST **list;
    NJS_TEXNAME *texture;
    int count;
    int i;

    /* @hack spelling: group stepped where the lists are read from it, not in
     * the for's third clause */
    for (group = groups; *group != NULL;) {
        for (list = *group++; *list != NULL; list++) {
            count = (*list)->nbTexture;
            texture = (*list)->textures;
            for (i = 0; i < count; i++) {
                texture->attr = 0;
                texture->texaddr = 0;
                texture++;
            }
        }
    }
    for (; pvms->texlist != NULL; pvms++) {
        if (pvms->name != NULL) {
            njReleaseTexture(pvms->texlist);
        }
    }
}

/* give each texture of to the attr and texaddr of from's first texture of
 * its name. names match whatever the case of their letters. one without a
 * match keeps its own. returns to's count */
static int copy_named(NJS_TEXLIST *to, NJS_TEXLIST *from)
{
    NJS_TEXNAME *texture;
    NJS_TEXNAME *match;
    int count;
    int from_count;
    int i;
    int j;

    texture = to->textures;
    count = to->nbTexture;
    from_count = from->nbTexture;
    for (i = 0; i < count; i++, texture++) {
        match = from->textures;
        for (j = 0; j < from_count; j++, match++) {
            if (names_match(match->filename, texture->filename)) {
                break;
            }
        }
        if (j < from_count) {
            texture->attr = match->attr;
            texture->texaddr = match->texaddr;
        }
    }
    return count;
}
#endif

#ifdef NONMATCHING
/* whether texlist holds a loaded texture at index. this side's draws test it
 * before they select the texture */
Bool texture_slot_loaded(const NJS_TEXLIST *texlist, Uint32 index)
{
    return texlist != NULL && index < texlist->nbTexture &&
           texlist->textures != NULL && texlist->textures[index].texaddr != 0;
}
#endif

Bool texture_list_loaded(const NJS_TEXLIST *texlist)
{
    const NJS_TEXNAME *texture;
    int count;
    int i;

    count = texlist->nbTexture;
    texture = texlist->textures;
    for (i = 0; i < count; i++) {
        if (texture->texaddr == 0) {
            return FALSE;
        }
        texture++;
    }
    return TRUE;
}
