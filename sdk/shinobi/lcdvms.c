/* VMS LCD image writes */
#include <kaede.h>
#include <sg_lcd.h>
#include <sg_xpt.h>

/* how many LCDs pdgLcdNo numbers */
#define LCDS 8
/* a packed image's size in 4-byte words, as kdSetBlockWrite takes it */
#define LCD_WORDS (LCD_SIZE / 4)
/* a VMS's function bits: storage, LCD and timer */
#define VMS_FUNCTIONS (KDD_FUNC_STORAGE | KDD_FUNC_LCD | KDD_FUNC_TIMER)

/* the member exports these names, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* each LCD's image as pdVmsLcdWrite last packed it */
Uint8 pdgVmsWork[LCDS][LCD_SIZE];

void pdVmsLcdBlt(void *dst, const void *src, Uint32 flag);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 pdVmsLcdWrite(Uint32 port, void *data, Uint32 flag)
{
    Uint8 *image;
    Sint32 err;

    if (!pdVmsLcdIsReady(port)) {
        return PDD_LCDERR_NO_LCD;
    }
    image = pdgVmsWork[pdgLcdNo[port]];
    pdVmsLcdBlt(image, data, flag);
    /* @hack spelling: kdSetBlockWrite's result through err, not tested in
     * place */
    err = kdSetBlockWrite(port, KDD_FUNC_LCD, 0, 0, 0, image, LCD_WORDS);
    if (err < 0) {
        return PDD_LCDERR_BUSY;
    }
    return PDD_LCDERR_OK;
}

Sint32 pdVmsLcdWrite1(Uint32 port, void *data)
{
    Sint32 err;

    if (!pdVmsLcdIsReady(port)) {
        return PDD_LCDERR_NO_LCD;
    }
    /* @hack spelling: kdSetBlockWrite's result through err, not tested in
     * place */
    err = kdSetBlockWrite(port, KDD_FUNC_LCD, 0, 0, 0, data, LCD_WORDS);
    if (err < 0) {
        return PDD_LCDERR_BUSY;
    }
    return PDD_LCDERR_OK;
}

Sint32 pdVmsLcdIsReady(Uint32 port)
{
    const KDS_DEVICEINFO *info;
    KDS_LCDINFO lcd;
    Sint32 err;

    info = kdGetDeviceInfo(port);
    /* @hack spelling: kdDevGetLCDInfo's result through err, not tested in
     * place */
    err = kdDevGetLCDInfo(&lcd, info);
    if (err < 0) {
        return 0;
    }
    if (info->function != VMS_FUNCTIONS) {
        return 0;
    }
    if (lcd.partition != 1) {
        return 0;
    }
    if (lcd.blocksize != LCD_SIZE) {
        return 0;
    }
    if (lcd.writecount != 1) {
        return 0;
    }
    if (lcd.direction != KDD_LCD_HV_00) {
        return 0;
    }
    if (lcd.normally != KDD_LCD_NORMALLYWHITE) {
        return 0;
    }
    return 1;
}

/* pack the image at src into dst, flipped as the PDD_LCD_FLAG_ value flag
 * says */
void pdVmsLcdBlt(void *dst, const void *src, Uint32 flag)
{
    switch (flag) {
    case PDD_LCD_FLAG_HFLIP:
        pdVmsLcdBltAsmFlipH(dst, src);
        break;
    case PDD_LCD_FLAG_VFLIP:
        pdVmsLcdBltAsmFlipV(dst, src);
        break;
    case PDD_LCD_FLAG_HVFLIP:
        pdVmsLcdBltAsmFlipHV(dst, src);
        break;
    default:
        pdVmsLcdBltAsm(dst, src);
        break;
    }
}
