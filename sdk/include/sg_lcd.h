/* Shinobi LCD library, as far as the sources use it */
#ifndef SG_LCD_H
#define SG_LCD_H
#include <kaede.h>
#include <sg_xpt.h>

#define PDD_LCDERR_OK 0 /* the write is queued */
/* Kaede, the SDK's peripheral driver, refuses the write */
#define PDD_LCDERR_BUSY (-1)
#define PDD_LCDERR_NO_LCD (-2) /* the port has no VMS LCD */

/* pdVmsLcdWrite's flags: flip the image horizontally, vertically or both.
 * any other flag leaves it as it is */
#define PDD_LCD_FLAG_HFLIP 1
#define PDD_LCD_FLAG_VFLIP 2
#define PDD_LCD_FLAG_HVFLIP 3

/* each port's LCD number, or NO_LCD for none. a port is a Kaede device number.
 * only the ports of the controllers' first two sockets have numbers, 0 to 7 in
 * port order. so pdgLcdNo[PDD_PORT_A1] is 0, and pdgLcdNo[PDD_PORT_D2] is 7 */
extern const Uint8 pdgLcdNo[KDD_DEV_MAX];
#define NO_LCD 255

/* the bytes of an LCD image packed at a bit a pixel: 32 rows of 6 bytes */
#define LCD_SIZE 192

/* 1 when the device at port has an LCD these calls can write, and 0
 * otherwise. such a device has exactly the storage, LCD and timer
 * functions. its KDS_LCDINFO gives one partition of LCD_SIZE-byte blocks, each
 * written in one access. the LCD is KDD_LCD_HV_00 and
 * KDD_LCD_NORMALLYWHITE */
Sint32 pdVmsLcdIsReady(Uint32 port);
/* queue a write of a 48 by 32 pixel image to the LCD at port. each call returns
 * a PDD_LCDERR_ code, and PDD_LCDERR_NO_LCD where pdVmsLcdIsReady returns 0.
 *
 * pdVmsLcdWrite takes an image of a byte a pixel, row by row. a pixel's bit
 * is set where its byte has bit 3 set. the call packs the image, flipped as
 * flag says, into a buffer it keeps for the LCD. it then queues the buffer's
 * write. pdgLcdNo must give port a number, which the call doesn't check.
 *
 * pdVmsLcdWrite1 queues a write of data itself, an image packed at a bit a
 * pixel into LCD_SIZE bytes */
Sint32 pdVmsLcdWrite(Uint32 port, void *data, Uint32 flag);
Sint32 pdVmsLcdWrite1(Uint32 port, void *data);

/* pack the image at src into the LCD_SIZE bytes at dst, as pdVmsLcdWrite packs
 * it. the FlipH, FlipV and FlipHV calls flip it horizontally, vertically or
 * both, and pdVmsLcdBltAsm doesn't */
void pdVmsLcdBltAsm(void *dst, const void *src);
void pdVmsLcdBltAsmFlipH(void *dst, const void *src);
void pdVmsLcdBltAsmFlipV(void *dst, const void *src);
void pdVmsLcdBltAsmFlipHV(void *dst, const void *src);

/* LCD orientations, as pdLcdGetDirection returns them */
#define PDD_LCD_DIRECTION_NORMAL 0
#define PDD_LCD_DIRECTION_FLIP 1
#define PDD_LCD_DIRECTION_LEFT 2
#define PDD_LCD_DIRECTION_RIGHT 3

/* pdLcdInit and pdLcdExit do nothing */
void pdLcdInit(void);
void pdLcdExit(void);
/* which way up the LCD at port faces, a PDD_LCD_DIRECTION_ value. it
 * follows from the connector directions of the controller's socket and of
 * the device */
Sint32 pdLcdGetDirection(Uint32 port);

#endif
