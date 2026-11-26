/* a texture file's path and global index, and which texture types are
 * palettized */
#include <machine.h>
#include <nj_texlist.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

/* the characters of an extension, with its dot */
#define EXTENSION_LENGTH 4

Sint32 njCheckFileName(char *path, const char *name, const char *ext)
{
    char suffix[EXTENSION_LENGTH + 1];
    Sint32 length;

    length = strlen(name);
    _builtin_strcpy(suffix, name + (length - EXTENSION_LENGTH));
    strupr(suffix);
    if (suffix[0] == '.') {
        /* @hack spelling: a goto into the else's arm, where one test of both
         * conditions would do */
        if (_builtin_strcmp(suffix, ext) != 0) {
            goto append;
        }
        _builtin_strcpy(path, _nj_current_texture_path);
        strcat(path, name);
        /* @hack redundant: the return before the else, which the shared
         * return after it would make */
        return 1;
    } else {
    append:
        _builtin_strcpy(path, _nj_current_texture_path);
        strcat(path, name);
        strcat(path, ext);
    }
    return 1;
}

Sint32 njCheckPaletteType(Uint32 type)
{
    switch (type) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        return TRUE;
    default:
        return FALSE;
    }
}

Sint32 njCheckFileInfo(Uint32 *globalIndex)
{
    Uint32 name;
    struct gbix_chunk gbix;
    struct pvrt_chunk pvrt;
    Uint32 type;

    njTexFseek(0, GDD_SEEK_SET);
    gbix.globalIndex = TEXMEM_FREE;
    do {
        if (njTexFread(&name, sizeof(name)) < 0) {
            return -NJD_TEXERR_HEADER;
        }
        switch (name) {
        case iff_GBIX:
            njTexFseek(-(Sint32)sizeof(name), GDD_SEEK_CUR);
            if (njTexFread(&gbix, sizeof(gbix)) < 0) {
                return -NJD_TEXERR_HEADER;
            }
            break;
        case iff_PVRT:
            if (njTexFread(&pvrt, sizeof(pvrt)) < 0) {
                return -NJD_TEXERR_HEADER;
            }
            break;
        }
    } while (name != iff_PVRT);
    njTexFseek(0, GDD_SEEK_SET);
    if (gbix.globalIndex != TEXMEM_FREE) {
        *globalIndex = gbix.globalIndex;
    } else {
        while (njSearchTexList(_nj_texmemlist, _nj_texmemsize,
                               _nj_user_globalIndex) != -1) {
            _nj_user_globalIndex--;
        }
        *globalIndex = _nj_user_globalIndex;
    }
    /* @hack spelling: type, a local for the texture's type, where the type
     * switched on straight would do */
    type = pvrt.header.nTextureType;
    switch (type & NJD_TEXFMT_TYPE_MASK) {
    case NJD_TEXFMT_PALETTIZE4:
    case NJD_TEXFMT_PALETTIZE4_MM:
    case NJD_TEXFMT_PALETTIZE8:
    case NJD_TEXFMT_PALETTIZE8_MM:
        return TRUE;
    default:
        return FALSE;
    }
}
