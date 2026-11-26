/* The sound memory calls on a Uint32 or a Sint32 at each end of sound memory.
 * An address below it or past it reaches no bus address. */
#include <assert.h>
#include <stddef.h>

#include "../../sdk/sound/sd_snd_mem_read_sint32.c"
#include "../../sdk/sound/sd_snd_mem_read_uint32.c"
#include "../../sdk/sound/sd_snd_mem_write_sint32.c"
#include "../../sdk/sound/sd_snd_mem_write_uint32.c"

/* the word each read gives, and each write passes */
#define BUS_WORD 0x12345678
/* the last bus address an access reached, and the count of accesses */
static Uint32 reached;
static int accesses;

SYE_G2_ERR syG2Read(void *g2_adr, void *buf_ptr, SYE_G2_DATA_WIDTH data_width,
                    Sint32 data_num, SYE_G2_ADR_MODE src_mode,
                    SYE_G2_ADR_MODE dst_mode)
{
    (void)src_mode;
    (void)dst_mode;
    assert(data_width == SYE_G2_DATA_WIDTH_32BIT && data_num == 1);
    reached = (Uint32)(size_t)g2_adr;
    accesses++;
    *(Uint32 *)buf_ptr = BUS_WORD;
    return SYE_G2_ERR_NOTHING;
}

SYE_G2_ERR syG2Write(void *buf_ptr, void *g2_adr, SYE_G2_DATA_WIDTH data_width,
                     Sint32 data_num, SYE_G2_ADR_MODE src_mode,
                     SYE_G2_ADR_MODE dst_mode)
{
    (void)src_mode;
    (void)dst_mode;
    assert(data_width == SYE_G2_DATA_WIDTH_32BIT && data_num == 1);
    assert(*(Uint32 *)buf_ptr == BUS_WORD);
    reached = (Uint32)(size_t)g2_adr;
    accesses++;
    return SYE_G2_ERR_NOTHING;
}

/* run each of the four calls at address, which reaches the bus only where
 * inside is true */
static void check(Sint32 address, int inside)
{
    Uint32 word = 0;
    Sint32 signed_word = 0;
    int before = accesses;

    assert(sdSndMemReadUint32(address, &word) == SDE_ERR_NOTHING);
    assert(sdSndMemReadSint32(address, &signed_word) == SDE_ERR_NOTHING);
    assert(sdSndMemWriteUint32(address, BUS_WORD) == SDE_ERR_NOTHING);
    assert(sdSndMemWriteSint32(address, BUS_WORD) == SDE_ERR_NOTHING);
    if (inside) {
        assert(accesses == before + 4);
        assert(reached == SD_SND_MEM_BASE + (Uint32)address);
        assert(word == BUS_WORD && signed_word == BUS_WORD);
    } else {
        assert(accesses == before);
        assert(word == 0 && signed_word == 0);
    }
}

int main(void)
{
    check(0, 1);
    check(SD_SND_MEM_SIZE - 4, 1);
    check(SD_SND_MEM_SIZE, 0);
    check(-4, 0);
    return 0;
}
