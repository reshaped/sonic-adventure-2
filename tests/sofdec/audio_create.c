/* SFADXT_Create where SJRBF_Create can't make the audio's joint: the create
 * fails with NO_STREAM, and destroys the player it made first */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <cri_adxt.h>
#include <sfd.h>
#include <sg_xpt.h>
#include <sj.h>

/* the player sfadxt_CreateEx makes, and the players destroyed */
static ADX_TALK made;
static ADXT destroyed;
static Sint32 destroy_count;

Sint32 SFSET_GetCond(struct sfd_handle *handle, Sint32 id)
{
    (void)handle;
    return id == SFD_COND_AUDIO;
}

Sint32 SFLIB_SetErr(struct sfd_handle *handle, Sint32 code)
{
    (void)handle;
    return code;
}

/* no joint is free */
SJ SJRBF_Create(char *buf, Sint32 bsize, Sint32 xsize)
{
    (void)buf;
    (void)bsize;
    (void)xsize;
    return NULL;
}

static Sint32 sfadxt_InitInf(struct sfd_handle *handle,
                             struct sfd_audio_work *audio)
{
    (void)handle;
    (void)audio;
    return 0;
}

static ADXT sfadxt_CreateEx(struct sfd_audio_params *params)
{
    (void)params;
    return &made;
}

static Sint32 sfadxt_DestroySub(ADXT player)
{
    destroyed = player;
    destroy_count++;
    return 0;
}

/* the create's steps past the joint, which a failed joint never reaches */
void ADXT_StartSj(ADXT adxt, SJ sj)
{
    (void)adxt;
    (void)sj;
    abort();
}

static void sfadxt_WaitStart(struct sfd_handle *handle, Bool wait)
{
    (void)handle;
    (void)wait;
    abort();
}

static void sfadxt_SetTimeFn(struct sfd_handle *handle)
{
    (void)handle;
    abort();
}

#include "extracted.inc"

static struct sfd_handle handle;

int main(void)
{
    assert(SFADXT_Create(&handle, 0, 0, 0) == NO_STREAM);
    assert(destroy_count == 1);
    assert(destroyed == &made);
    return 0;
}
