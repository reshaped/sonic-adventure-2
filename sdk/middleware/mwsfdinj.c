/* the player's setup on Ninja (sfdfx_mw.h). mwPlyInitSofdec gives the
 * player Ninja's end-of-render flag and vertex buffer */
#include <mwsfd.h>
#include <nj_global.h>
#include <sfdfx_mw.h>
#include <sg_mwrv2.h>

void mwPlyInitSofdec(MWS_PLY_INIT_SFD *display)
{
    mwPlyInitSfdFx(display);
    mwg_eor_flag_ptr = &_nj_eor_flag_;
    mwRnv2SetVertexBuffer(&_nj_vertex_buffer_);
}

void mwPlyFinishSofdec(void)
{
    mwPlyFinishSfdFx();
}

void mwPlyInitSfdNj2(MWS_PLY_INIT_SFD *display)
{
    mwPlyInitSofdec(display);
}

void mwPlyFinishNj2(void)
{
    mwPlyFinishSofdec();
}
