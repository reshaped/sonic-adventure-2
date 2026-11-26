/* the karts' model slots of cart_setup.models: the built-in karts, and the
 * download slot. the download slot holds the model a download course unpacks */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "cart/course.h"
#include "cart/kart_models.h"
#ifdef NONMATCHING
/* texture_discard_pvm, by which this side's release_download releases the
 * download's textures */
#include "system/texture.h"
#endif

#define MODEL(id, object, second, textures)                                    \
    {id, &(object), second, &(textures), NULL, NULL}
static struct cart_model builtins[] = {
    MODEL(CART_MODEL_TAILS_STORY, cart_tails_story_model, NULL,
          cart_tails_textures),
    MODEL(CART_MODEL_ROUGE_STORY, cart_rouge_story_model,
          &cart_rouge_story_second, cart_rouge_textures),
    MODEL(CART_MODEL_SONIC, cart_sonic_model, &cart_sonic_second,
          cart_race_textures),
    MODEL(CART_MODEL_KNUCKLES, cart_knuckles_model, &cart_knuckles_second,
          cart_race_textures),
    MODEL(CART_MODEL_TAILS, cart_tails_model, &cart_tails_second,
          cart_race_textures),
    MODEL(CART_MODEL_EGGMAN, cart_eggman_model, &cart_eggman_second,
          cart_race_textures),
    MODEL(CART_MODEL_SHADOW, cart_shadow_model, &cart_shadow_second,
          cart_race_textures),
    MODEL(CART_MODEL_ROUGE, cart_rouge_model, &cart_rouge_second,
          cart_race_textures),
};

#define DOWNLOAD_SLOT cart_setup.models[CART_MODEL_DOWNLOAD]
/* the id of a slot that holds no model */
#define NO_MODEL (-1)

#ifdef NONMATCHING
/* this side unpacks the download into a candidate, by the archive's size. it
 * replaces the download slot only where the unpack succeeds. the matching side
 * unpacks into the slot itself */
static void release_download(int kind);

Bool cart_replace_download_model(const void *archive, Uint32 size)
{
    struct cart_model candidate = {0};
    struct cart_model_request request;
    request.object = &candidate.object;
    request.textures = &candidate.textures;
    request.archive = (struct cart_archive *)archive;
    request.archive_size = size;
    request.model_buffer = &candidate.model_buffer;
    request.texture_buffer = &candidate.texture_buffer;
    if (!cart_model_unpack(&request)) {
        return FALSE;
    }
    release_download(CART_MODEL_DOWNLOAD);
    candidate.id = CART_MODEL_DOWNLOAD;
    DOWNLOAD_SLOT = candidate;
    return TRUE;
}

int cart_load_download_model(void)
{
    return cart_replace_download_model(cart_download_model(),
                                       cart_download_model_size());
}
#else
int cart_load_download_model(void)
{
    struct cart_model_request request;
    request.object = &DOWNLOAD_SLOT.object;
    request.textures = &DOWNLOAD_SLOT.textures;
    request.archive = cart_download_model();
    request.model_buffer = &DOWNLOAD_SLOT.model_buffer;
    request.texture_buffer = &DOWNLOAD_SLOT.texture_buffer;
    DOWNLOAD_SLOT.second = NULL;
    /* @hack spelling: an else after the return, where the store and return
     * after the if would do */
    if (!cart_model_unpack(&request)) {
        return FALSE;
    } else {
        DOWNLOAD_SLOT.id = CART_MODEL_DOWNLOAD;
        return TRUE;
    }
}

#endif

/* free the download slot's buffers and empty it. kind goes unread: no other
 * slot owns its buffers */
static void release_download(int kind)
{
    if (DOWNLOAD_SLOT.id != NO_MODEL) {
        if (DOWNLOAD_SLOT.model_buffer) {
            syFree(DOWNLOAD_SLOT.model_buffer);
            DOWNLOAD_SLOT.object = NULL;
            DOWNLOAD_SLOT.model_buffer = NULL;
        }
        if (DOWNLOAD_SLOT.textures) {
#ifdef NONMATCHING
            /* this side releases the textures by texture_discard_pvm, which
             * also empties the list, so a second release is safe */
            texture_discard_pvm(DOWNLOAD_SLOT.textures);
#else
            njReleaseTexture(DOWNLOAD_SLOT.textures);
#endif
            DOWNLOAD_SLOT.textures = NULL;
        }
        if (DOWNLOAD_SLOT.texture_buffer) {
            syFree(DOWNLOAD_SLOT.texture_buffer);
            DOWNLOAD_SLOT.texture_buffer = NULL;
        }
        DOWNLOAD_SLOT.id = NO_MODEL;
    }
}
void cart_release_models(void)
{
    release_download(CART_MODEL_DOWNLOAD);
}
void cart_init_models(void)
{
    int i;
#ifdef NONMATCHING
    /* this side frees a download model that an earlier race left in its slot.
     * the loop below would drop its buffers */
    if (DOWNLOAD_SLOT.id == CART_MODEL_DOWNLOAD) {
        release_download(CART_MODEL_DOWNLOAD);
    }
#endif
    for (i = 0; i < CART_MODELS; i++) {
        if (i < CART_MODEL_DOWNLOAD) {
            cart_setup.models[i] = builtins[i];
        } else {
            cart_setup.models[i].id = NO_MODEL;
        }
    }
}
void cart_load_model_textures(void)
{
    switch (cart_setup.course_mode) {
    case CART_TAILS:
        njLoadTexturePvmFile("tailsBig.pvm", &cart_tails_textures);
        break;
    case CART_ROUGE:
        njLoadTexturePvmFile("rougeBig.pvm", &cart_rouge_textures);
        break;
    default:
        njLoadTexturePvmFile("cartData.pvm", &cart_race_textures);
        break;
    }
}
