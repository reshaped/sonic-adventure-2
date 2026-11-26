/* the Maple driver's commands that carry the caller's data words */
#include <kaede.h>
#include <machine.h>
#include <maple_driver.h>
#include <sg_xpt.h>

static const char maple_no_semaphore[] =
    "***FATAL ERROR:Cannot get semaphore!\n";

Sint32 mpdrv_set_getcond2(Sint32 device, Sint32 func, const void *data,
                          Sint32 words)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_getcond2_ns(device, func, data, words);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_getcond2_ns(Sint32 device, Sint32 func, const void *data,
                             Sint32 words)
{
    /* @hack spelling: func through a copy, where &func would do */
    Sint32 function_word = func;

    return mpdrv_setcmd(device, MPDRV_CMD_GETCOND, &function_word, 1, data,
                        words);
}

Sint32 mpdrv_set_cameracommand(Sint32 device, const void *data)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_cameracommand_ns(device, data);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_cameracommand_ns(Sint32 device, const void *data)
{
    Sint32 function_word = KDD_FUNC_CAMERA;

    return mpdrv_setcmd(device, MPDRV_CMD_CAMERA, &function_word, 1, data, 1);
}

Sint32 mpdrv_set_unicommand(Sint32 device, Sint32 func, Uint8 command,
                            const void *data, Sint32 words)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_unicommand_ns(device, func, command, data, words);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_unicommand_ns(Sint32 device, Sint32 func, Uint8 command,
                               const void *data, Sint32 words)
{
    /* @hack spelling: func through a copy, where &func would do */
    Sint32 function_word = func;

    return mpdrv_setcmd(device, command, &function_word, 1, data, words);
}

Sint32 mpdrv_set_unicommand2(Sint32 device, Uint8 command, const void *data,
                             Sint32 words, const void *extra,
                             Sint32 extra_words)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_unicommand2_ns(device, command, data, words, extra,
                                      extra_words);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_unicommand2_ns(Sint32 device, Uint8 command, const void *data,
                                Sint32 words, const void *extra,
                                Sint32 extra_words)
{
    /* @hack spelling: command through a copy, where command would do */
    Uint8 code = command;

    return mpdrv_setcmd(device, code, data, words, extra, extra_words);
}
