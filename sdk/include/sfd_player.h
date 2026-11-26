/* the Sofdec calls that set a handle's error function and its channels, and the
 * library's finish */
#ifndef SFD_PLAYER_H
#define SFD_PLAYER_H
#include <sg_xpt.h>

struct sfd_handle;
extern struct sfd_handle *sfd_hn_last;
Sint32 SFD_Finish(void);
Sint32 SFD_SetErrFn(struct sfd_handle *handle, void (*report)(void *, Sint32),
                    void *object);
Sint32 SFD_SetAudioCh(struct sfd_handle *handle, Sint32 channel);
Sint32 SFD_SetVideoCh(struct sfd_handle *handle, Sint32 channel);
#endif
