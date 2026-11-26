/* the system's start and end: the hardware, the heap, Ninja, the pads, the
 * clock and the GD file system */
#ifndef SYSTEM_SBINIT_H
#define SYSTEM_SBINIT_H
#include <sg_xpt.h>

/* reload the boot ROM's system code. start the hardware, the heap and Ninja,
 * which takes mode, frame and count. then start the pads, the clock and the GD
 * file system, and enter the disc's SONIC2 directory. an open tray or a changed
 * disc exits to the boot ROM. so does a file system that fails each of its
 * starts */
void sbInitSystem(Int mode, Int frame, Int count);
/* stop what sbInitSystem started, last first, and mask every interrupt */
void sbExitSystem(void);

#endif
