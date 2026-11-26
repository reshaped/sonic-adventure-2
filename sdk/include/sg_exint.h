/* Shinobi's interrupts from the devices in the Dreamcast extension box's
 * slots */
#ifndef SG_EXINT_H
#define SG_EXINT_H
#include <sg_chain.h>
#include <sg_xpt.h>

#define SYD_EXTINT_EDEV_MAX_DEVICE 32
#define SYD_EXTINT_EVENT_IML4_LANPCI 5
typedef Sint16 SY_EXTINT_EVENT;
typedef Uint32 SYEXTINT;

extern const Uint32 sygExtIntVer;
extern char *sygExtIntBuild;

void syExtIntInit(void);
void syExtIntFinish(void);
/* call usrIsr with arg whenever the device in slot event, 1 to 31, raises its
 * interrupt. it returns event as the handler's handle. slot 0 is the box's
 * bridge, which takes no handler. priority goes unread */
SYCHAIN syExtIntAddHandler(SY_EXTINT_EVENT event, void (*usrIsr)(void *),
                           Uint32 priority, void *arg);
void syExtIntDeleteHandler(SYEXTINT extint);
SYCHAIN syExIntAddHandler(SY_EXTINT_EVENT event, void (*usrIsr)(void *),
                          Uint32 priority, void *arg);
void syExIntDeleteHandler(SYEXTINT extint);
Uint32 syExtIntGetDevId(SYEXTINT dev);
Uint32 syExtIntGetDevIst(SYEXTINT dev);
void syExtIntSetDevImr(SYEXTINT dev, Uint32 imr);
#endif
