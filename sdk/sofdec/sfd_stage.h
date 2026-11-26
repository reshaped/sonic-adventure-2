/* a transfer's stage functions, one for each operation of its table, even
 * where two do the same. SFD_STAGE_NOOP defines one that does nothing and
 * returns 0. SFD_STAGE_REJECT defines one that sets code as handle's error
 * by SFLIB_SetErr, and returns what that returns */
#ifndef SFD_STAGE_H
#define SFD_STAGE_H
#include <sfd.h>
#include <sg_xpt.h>

#define SFD_STAGE_NOOP(name)                                                   \
    Sint32 name(struct sfd_handle *handle, Sint32 first, Sint32 second,        \
                Sint32 third)                                                  \
    {                                                                          \
        return 0;                                                              \
    }

#define SFD_STAGE_REJECT(name, code)                                           \
    static Sint32 name(struct sfd_handle *handle, Sint32 first, Sint32 second, \
                       Sint32 third)                                           \
    {                                                                          \
        return SFLIB_SetErr(handle, code);                                     \
    }

#endif
