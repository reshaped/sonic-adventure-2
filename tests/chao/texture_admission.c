/* Check that the garden loads its eight textures in order, and that a texture
 * whose file won't read, or whose PVM won't load, stops the game with the
 * file's name. */
#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include "chao/garden.c"

NJS_TEXLIST chao_icon_texlist, chao_eye_texlist, chao_mouth_texlist;
NJS_TEXLIST chao_mini_parts_texlist, chao_body_texlist, chao_jewel_texlist;
NJS_TEXLIST chao_common_texlist, chao_env_texlist;

static const struct {
    const char *file;
    NJS_TEXLIST *texlist;
} expected[] = {
    {"AL_ICON.PRS", &chao_icon_texlist},
    {"AL_EYE.PRS", &chao_eye_texlist},
    {"AL_MOUTH.PRS", &chao_mouth_texlist},
    {"AL_MINI_PARTS_TEX.PRS", &chao_mini_parts_texlist},
    {"AL_BODY.PRS", &chao_body_texlist},
    {"AL_jewel.PRS", &chao_jewel_texlist},
    {"AL_COMMON_TEX.PRS", &chao_common_texlist},
    {"AL_ENV_TEX.PRS", &chao_env_texlist},
};
#define TEXTURES (sizeof(expected) / sizeof(expected[0]))

/* the bytes each read gives, which each load must take */
#define FILE_BYTES 64

static unsigned int reads;
static unsigned int loads;
/* the index of the read that fails, and of the load, or TEXTURES for none */
static unsigned int failed_read = TEXTURES;
static unsigned int failed_load = TEXTURES;
static jmp_buf stopped;
/* the last stop's words, null for loader_stop's, and its name */
static const char *stop_what;
static const char *stop_name;

Sint32 loader_read_prs_buffer(const char *name, void *buffer, Uint32 capacity)
{
    assert(reads < TEXTURES && strcmp(name, expected[reads].file) == 0);
    assert(buffer == WORK_AREA && capacity == WORK_AREA_END - WORK_AREA);
    return reads++ == failed_read ? -1 : FILE_BYTES;
}

Bool texture_load_pvm_memory(const void *archive, Uint32 size,
                             NJS_TEXLIST *target)
{
    assert(archive == WORK_AREA && size == FILE_BYTES);
    assert(loads < reads && target == expected[loads].texlist);
    return loads++ != failed_load;
}

void loader_stop(const char *name)
{
    stop_what = NULL;
    stop_name = name;
    longjmp(stopped, 1);
}

void port_stop(const char *what, const char *name, Uint32 number)
{
    assert(number == 0);
    stop_what = what;
    stop_name = name;
    longjmp(stopped, 1);
}

/* load the garden's textures. they must stop at texture index, once index + 1
 * reads and loaded loads have run */
static void expect_stop(unsigned int index, unsigned int loaded)
{
    reads = loads = 0;
    if (setjmp(stopped) == 0) {
        load_garden_textures();
        assert(!"no stop");
    }
    assert(strcmp(stop_name, expected[index].file) == 0);
    assert(reads == index + 1 && loads == loaded);
}

int main(void)
{
    unsigned int index;

    load_garden_textures();
    assert(reads == TEXTURES && loads == TEXTURES);
    for (index = 0; index < TEXTURES; index++) {
        failed_read = index;
        expect_stop(index, index);
        assert(stop_what == NULL);
        failed_read = TEXTURES;
        failed_load = index;
        expect_stop(index, index + 1);
        assert(strcmp(stop_what, "PVM") == 0);
        failed_load = TEXTURES;
    }
    return 0;
}
