/* mwlRnaStartTrans on a renderer whose second channel has a null joint, as
 * ahx_decinfo leaves a stereo handle for an AHX of two channels. it reports
 * the joint, starts no copy, and gives back the chunk it took from the first
 * joint */
#include <assert.h>
#include <string.h>

#include "mw_rna.c"

static Sint32 reports;

void mwRnaErrCallFunc(const char *format, ...)
{
    (void)format;
    reports++;
}

Uint32 syTmrGetCount(void)
{
    return 0;
}

/* the first channel's joint: it hands out up to its bytes, and counts what
 * comes back */
static char bytes[4096];
static Sint32 gets;
static Sint32 ungets;
static SJCK given_back;

static void get_chunk(SJ sj, Sint32 id, Sint32 nbyte, SJCK *ck)
{
    (void)sj;
    (void)id;
    gets++;
    ck->data = bytes;
    ck->len = nbyte < (Sint32)sizeof(bytes) ? nbyte : (Sint32)sizeof(bytes);
}

static void unget_chunk(SJ sj, Sint32 id, SJCK *ck)
{
    (void)sj;
    (void)id;
    ungets++;
    given_back = *ck;
}

static SJ_IF joint_if = {
    .GetChunk = get_chunk,
    .UngetChunk = unget_chunk,
};
static SJ_OBJ joint = {&joint_if};

int main(void)
{
    struct _adxrna_obj rna;

    memset(&rna, 0, sizeof(rna));
    rna.used = TRUE;
    rna.nch = 2;
    rna.maxnch = 2;
    rna.bps = 16;
    rna.bufsize = 4096;
    rna.sj[0] = &joint;
    rna.sj[1] = NULL;
    assert(mwlRnaStartTrans(&rna) == 0);
    assert(reports > 0);
    assert(gets == 1);
    assert(ungets == 1);
    assert(given_back.data == bytes);
    assert(rna.trnsing == FALSE);
    return 0;
}
