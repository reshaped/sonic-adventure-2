/* Kaede's calls of the commands that carry the caller's data words, and the
 * Ex call of each */
#include <kaede.h>
#include <machine.h>
#include <maple_driver.h>
#include <sg_xpt.h>

static const char kd_no_semaphore[] = "***FATAL ERROR:Cannot get semaphore!\n";

Sint32 kdSetGetCondition2(Sint32 device, Sint32 func, const void *data,
                          Sint32 words)
{
    Sint32 seqno = mpdrv_set_getcond2(device, func, data, words);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetGetCondition2Ex(Sint32 device, Sint32 func, const void *data,
                            Sint32 words, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_getcond2_ns(device, func, data, words);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetCameraCommand(Sint32 device, const void *data)
{
    Sint32 seqno = mpdrv_set_cameracommand(device, data);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetCameraCommandEx(Sint32 device, const void *data, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_cameracommand_ns(device, data);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetUniCommand(Sint32 device, Sint32 func, char command,
                       const void *data, Sint32 words)
{
    Sint32 seqno = mpdrv_set_unicommand(device, func, command, data, words);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetUniCommandEx(Sint32 device, Sint32 func, char command,
                         const void *data, Sint32 words, KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_unicommand_ns(device, func, command, data, words);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetUniCommand2(Sint32 device, char command, const void *data,
                        Sint32 words, const void *extra, Sint32 extra_words)
{
    Sint32 seqno =
        mpdrv_set_unicommand2(device, command, data, words, extra, extra_words);

    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}

Sint32 kdSetUniCommand2Ex(Sint32 device, char command, const void *data,
                          Sint32 words, const void *extra, Sint32 extra_words,
                          KD_CALLBACK cbfunc)
{
    Sint32 seqno;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(kd_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    seqno = mpdrv_set_unicommand2_ns(device, command, data, words, extra,
                                     extra_words);
    if (seqno > 0) {
        kdSetCallback(device, cbfunc, seqno);
    }
    *_mp_SemaphoreAddress = 0;
    if (_mp->recv_pending) {
        kdRecvFrame();
    }
    return seqno;
}
