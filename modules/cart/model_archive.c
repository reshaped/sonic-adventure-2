/* the unpacking of a kart's model archive into heap copies of its model and
 * texture list. an archive's model and texture list each open with their root's
 * offset. the pointers in their records hold offsets from the part's start.
 * each copy's pointers are relocated, and the archive is never written */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "cart/course.h"
#include "system/relocate.h"

#ifdef NONMATCHING
#include <string.h>

#include "cart/chunk_stream.h"
#include "cart/download_format_impl.h"
#include "system/texture.h"

/* what model_graph's records say of the model's bytes. a byte in no record
 * holds 0, and a byte inside one OCCUPIED. a record's first byte holds its
 * kind */
enum model_record { OCCUPIED = 1, NODE, MESH, VERTICES, POLYGONS };

struct model_graph {
    const Uint8 *bytes;
    Uint8 *records;
    Uint32 size;
    Uint32 textures;
};

static Bool record(struct model_graph *graph, Uint32 offset, Uint32 size,
                   enum model_record type)
{
    Uint32 i;
    if (offset < 4 || !size || !cart_range(offset, size, graph->size)) {
        return FALSE;
    }
    if (graph->records[offset] == type && type != NODE) {
        return TRUE;
    }
    for (i = offset; i < offset + size; ++i) {
        if (graph->records[i]) {
            return FALSE;
        }
    }
    memset(graph->records + offset, OCCUPIED, size);
    graph->records[offset] = type;
    return TRUE;
}

static Bool read_mesh(struct model_graph *graph, Uint32 offset)
{
    Uint32 vlist, plist, vertex_bytes, polygon_bytes;
    if ((offset & 3) || !cart_range(offset, 24, graph->size)) {
        return FALSE;
    }
    if (graph->records[offset] == MESH) {
        return TRUE;
    }
    if (!record(graph, offset, 24, MESH)) {
        return FALSE;
    }
    vlist = cart_word(graph->bytes + offset);
    plist = cart_word(graph->bytes + offset + 4);
    return cart_chunk_streams_valid(graph->bytes, graph->size, vlist, plist,
                                    graph->textures, &vertex_bytes,
                                    &polygon_bytes) &&
           (!vlist || record(graph, vlist, vertex_bytes, VERTICES)) &&
           (!plist || record(graph, plist, polygon_bytes, POLYGONS));
}

static Bool read_nodes(struct model_graph *graph, Uint32 offset, unsigned depth)
{
    Uint32 flags, mesh, child;
    /* the drawer pushes a matrix for each level it descends. read_nodes takes
     * no model deeper than 32 levels. the karts released for download nest at
     * most four levels, the root's included */
    if (depth > 32) {
        return FALSE;
    }
    while (offset) {
        if ((offset & 3) || !record(graph, offset, 52, NODE)) {
            return FALSE;
        }
        flags = cart_word(graph->bytes + offset);
        mesh = cart_word(graph->bytes + offset + 4);
        child = cart_word(graph->bytes + offset + 44);
        if ((!mesh && !(flags & NJD_EVAL_HIDE)) ||
            (!child && !(flags & NJD_EVAL_BREAK)) ||
            (mesh && !read_mesh(graph, mesh)) ||
            (child && !read_nodes(graph, child, depth + 1))) {
            return FALSE;
        }
        offset = cart_word(graph->bytes + offset + 48);
    }
    return TRUE;
}

static void *local_pointer(Uint8 *base, Uint32 offset)
{
    return offset ? base + offset : NULL;
}

static NJS_CNK_OBJECT *copy_model(const Uint8 *bytes, Uint32 size,
                                  Uint32 textures, void **storage)
{
    struct model_graph graph;
    Uint32 root, i;
    NJS_CNK_OBJECT *object;
    NJS_CNK_MODEL *mesh;
    Uint8 *copy;

    if (size < 4) {
        return NULL;
    }
    root = cart_word(bytes);
    graph.bytes = bytes;
    graph.size = size;
    graph.textures = textures;
    graph.records = syCalloc(size, 1);
    if (!graph.records) {
        return NULL;
    }
    copy = NULL;
    if (root && read_nodes(&graph, root, 1)) {
        copy = syMalloc(size);
    }
    if (copy) {
        memcpy(copy, bytes, size);
        for (i = 4; i < size; i += 4) {
            if (graph.records[i] == NODE) {
                object = (NJS_CNK_OBJECT *)(copy + i);
                object->model = local_pointer(copy, cart_word(bytes + i + 4));
                object->child = local_pointer(copy, cart_word(bytes + i + 44));
                object->sibling =
                    local_pointer(copy, cart_word(bytes + i + 48));
            } else if (graph.records[i] == MESH) {
                mesh = (NJS_CNK_MODEL *)(copy + i);
                mesh->vlist = local_pointer(copy, cart_word(bytes + i));
                mesh->plist = local_pointer(copy, cart_word(bytes + i + 4));
            }
        }
    }
    syFree(graph.records);
    *storage = copy;
    return copy ? (NJS_CNK_OBJECT *)(copy + root) : NULL;
}

static Bool read_textures(const Uint8 *bytes, Uint32 size, Uint32 *root,
                          Uint32 *slots, Uint32 *count)
{
    Uint32 i, name, limit;
    struct cart_region regions[2];
    if (size < 4) {
        return FALSE;
    }
    *root = cart_word(bytes);
    if ((*root & 3) || *root < 4 || !cart_range(*root, 8, size)) {
        return FALSE;
    }
    *slots = cart_word(bytes + *root);
    *count = cart_word(bytes + *root + 4);
    if ((*slots & 3) || !*count || *count > size / 12) {
        return FALSE;
    }
    regions[0].offset = *root;
    regions[0].size = 8;
    regions[1].offset = *slots;
    regions[1].size = *count * 12;
    if (!cart_regions(regions, 2, 4, size)) {
        return FALSE;
    }
    for (i = 0; i < *count; ++i) {
        name = cart_word(bytes + *slots + 12 * i);
        if (name < 4 || name >= size) {
            return FALSE;
        }
        /* names may share bytes. relocation rewrites the list's root record and
         * its slots. a name that starts before either must end before it */
        limit = size;
        if (name < *root) {
            limit = *root;
        } else if (name < *root + 8) {
            return FALSE;
        }
        if (name < *slots && *slots < limit) {
            limit = *slots;
        } else if (name >= *slots && name < *slots + *count * 12) {
            return FALSE;
        }
        if (!cart_string(bytes + name, limit - name)) {
            return FALSE;
        }
    }
    return TRUE;
}

int cart_model_unpack(struct cart_model_request *request)
{
    enum { MODEL_DATA, TEXLIST_DATA, PVM_DATA, PARTS };
    struct cart_region part[PARTS];
    const Uint8 *bytes = (const Uint8 *)request->archive;
    const Uint8 *names = NULL;
    Uint32 root = 0, slots = 0, count = 0, i;
    NJS_CNK_OBJECT *object;
    NJS_TEXLIST *list = NULL;
    Uint8 *tex_copy = NULL;
    void *model_copy = NULL;

    if (!bytes || request->archive_size < 24 ||
        request->archive_size > CART_PACK_CAPACITY) {
        return FALSE;
    }
    for (i = 0; i < PARTS; ++i) {
        part[i].offset = cart_word(bytes + i * 8);
        part[i].size = cart_word(bytes + i * 8 + 4);
    }
    if (!cart_regions(part, PARTS, 24, request->archive_size) ||
        !part[MODEL_DATA].size ||
        (!part[TEXLIST_DATA].size != !part[PVM_DATA].size)) {
        return FALSE;
    }
    if (part[TEXLIST_DATA].size) {
        /* an archive's last 4 bytes may follow its texture list, and hold
         * three spaces and a NUL. they then end the list's last name. the
         * list takes them in, so its copy keeps the name's terminator */
        if (request->archive_size -
                    (part[TEXLIST_DATA].offset + part[TEXLIST_DATA].size) ==
                4 &&
            !memcmp(bytes + request->archive_size - 4, "   ", 4)) {
            part[TEXLIST_DATA].size += 4;
            if (!cart_regions(part, PARTS, 24, request->archive_size)) {
                return FALSE;
            }
        }
        names = bytes + part[TEXLIST_DATA].offset;
        if (!read_textures(names, part[TEXLIST_DATA].size, &root, &slots,
                           &count)) {
            return FALSE;
        }
    }
    object = copy_model(bytes + part[MODEL_DATA].offset, part[MODEL_DATA].size,
                        count, &model_copy);
    if (!object) {
        return FALSE;
    }
    if (names) {
        tex_copy = syMalloc(part[TEXLIST_DATA].size);
        if (!tex_copy) {
            syFree(model_copy);
            return FALSE;
        }
        memcpy(tex_copy, names, part[TEXLIST_DATA].size);
        list = (NJS_TEXLIST *)(tex_copy + root);
        list->textures = (NJS_TEXNAME *)(tex_copy + slots);
        for (i = 0; i < count; ++i) {
            list->textures[i].filename =
                tex_copy + cart_word(names + slots + i * 12);
            list->textures[i].attr = 0;
            list->textures[i].texaddr = 0;
        }
        if (!texture_load_pvm_memory(bytes + part[PVM_DATA].offset,
                                     part[PVM_DATA].size, list)) {
            syFree(tex_copy);
            syFree(model_copy);
            return FALSE;
        }
    }
    *request->model_buffer = model_copy;
    *request->object = object;
    *request->texture_buffer = tex_copy;
    *request->textures = list;
    return TRUE;
}

#else
/* each copy is allocated COPY_SLACK bytes past its part, which it leaves
 * unwritten */
#define COPY_SLACK 4
static int unpack(NJS_CNK_OBJECT **object, NJS_TEXLIST **textures,
                  struct cart_archive *top, void **model_buffer,
                  void **texture_buffer)
{
    /* @hack spelling: archive, a local for top, where top alone would do */
    struct cart_archive *archive;
    Uint8 *src;
    Uint8 *dst;
    int i;
    /* @hack spelling: allocation, a local for the model's address, where base
     * alone would do */
    Uint32 allocation;
    Uint32 base;

    archive = top;
    /* @hack spelling: the assignment inside the test, here and below, where a
     * statement of its own would do */
    if ((*object = syMalloc(archive->model_size + COPY_SLACK)) == NULL) {
        *model_buffer = NULL;
        return FALSE;
    }
    *model_buffer = *object;
    src = (Uint8 *)top + archive->model_offset;
    dst = (Uint8 *)*object;
    for (i = 0; i < archive->model_size; i++) {
        /* @hack redundant: a test of object no path reaches */
        if (0) {
            if (object) {
            }
        }
        *dst = *src;
        /* @hack spelling: dst = dst + 1, where dst++ would do */
        dst = dst + 1;
        src++;
    }
    /* @hack redundant: a test of textures no path reaches */
    if (0) {
        if (textures) {
        }
    }
    allocation = (Uint32)*object;
    base = allocation;
    *object = (NJS_CNK_OBJECT *)(*(Uint32 *)*object + base);
    relocate_cnk_object(*object, base);
    if (archive->texlist_size == 0) {
        *textures = NULL;
        return TRUE;
    }
    if ((*textures = syMalloc(archive->texlist_size + COPY_SLACK)) == NULL) {
        syFree(*model_buffer);
        *model_buffer = NULL;
        *texture_buffer = NULL;
        return FALSE;
    }
    *texture_buffer = *textures;
    src = (Uint8 *)top + archive->texlist_offset;
    dst = (Uint8 *)*textures;
    for (i = 0; i < archive->texlist_size; i++) {
        *dst = *src;
        dst++;
        src++;
    }
    base = (Uint32)*textures;
    *textures = (NJS_TEXLIST *)(*(Uint32 *)*textures + base);
    (*textures)->textures =
        (NJS_TEXNAME *)((Uint32)(*textures)->textures + base);
    for (i = 0; (Uint32)i < (*textures)->nbTexture; i++) {
        (*textures)->textures[i].filename =
            (void *)((Uint32)(*textures)->textures[i].filename + base);
    }
    if (njLoadTexturePvmMemory((Uint8 *)top + archive->pvm_offset, *textures) ==
        -1) {
        syFree(*model_buffer);
        syFree(*texture_buffer);
        *model_buffer = NULL;
        *texture_buffer = NULL;
        return FALSE;
    }
    return TRUE;
}

int cart_model_unpack(struct cart_model_request *request)
{
    return unpack(request->object, request->textures, request->archive,
                  request->model_buffer, request->texture_buffer);
}
#endif
