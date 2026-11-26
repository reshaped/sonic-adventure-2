/* the address checker's dump. it prints the frame of registers a master and
 * syIntBrkRteRte pushed, then hardware registers */
#include <sg_xpt.h>
#include <sy_int_chk.h>

/* the words of syIntChkDump's frame, by the names the dump prints them
 * under. syIntBrkRteRte pushes the words below FRAME_FR0. the rest are the
 * frame of the master that called it */
enum frame_word {
    FRAME_DBR = 0,
    FRAME_SGR = 1,
    FRAME_BANKED_R0 = 2,
    FRAME_FR0 = 10,
    FRAME_BANKED_FR0 = 26,
    /* @bug logic: reads every register from FRAME_FPUL up from the wrong
     * word. the master stores at 0x8c15ac18 its stack pointer from before it
     * aligned the frame, in word 42. FPSCR's word follows at 44 where it sat
     * at a multiple of 8, and at 45 otherwise. FPUL's and the rest follow it,
     * as syIntBrkRteRte pops them from 0x8c135266. the dump reads word 42 as
     * FPUL at 0x8c15a8ba */
    FRAME_FPUL = 42,
    FRAME_FPSCR = 43,
    FRAME_R0 = 45,
    FRAME_R8 = 53,
    FRAME_MACL = 60,
    FRAME_MACH = 61,
    FRAME_VBR = 62,
    FRAME_GBR = 63,
    FRAME_PR = 64,
    FRAME_PC = 65,
    FRAME_SR = 66
};

/* the SH-4's exception event register */
#define EXPEVT (*(volatile Uint32 *)0xff000024)

/* the register at address, and the first five registers of the DMA channel
 * at base */
#define REG(address) ((const volatile Uint32 *)(address))
#define DMA_REGS(base)                                                         \
    REG(base), REG((base) + 4), REG((base) + 8), REG((base) + 12),             \
        REG((base) + 16)
/* the hardware registers the dump prints after the frame */
static const volatile Uint32 *const sygIntAdrChk_adress[] = {
    /* the SH-4's MMUCR, CCR, QACR0 and QACR1 */
    REG(0xff000010), REG(0xff00001c), REG(0xff000038), REG(0xff00003c),
    /* its DMA controller's SAR0, DAR0, SAR1, DAR1, CHCR1, SAR2, DMATCR2,
     * CHCR2, SAR3, DAR3, CHCR3 and DMAOR */
    REG(0xffa00000), REG(0xffa00004), REG(0xffa00010), REG(0xffa00014),
    REG(0xffa0001c), REG(0xffa00020), REG(0xffa00028), REG(0xffa0002c),
    REG(0xffa00030), REG(0xffa00034), REG(0xffa0003c), REG(0xffa00040),
    /* the system bus's channel 2, sort, Maple and GD-ROM DMA registers */
    REG(0xa05f6800), REG(0xa05f6804), REG(0xa05f6810), REG(0xa05f6814),
    REG(0xa05f6c04), REG(0xa05f6c10), REG(0xa05f7404), REG(0xa05f7408),
    REG(0xa05f740c),
    /* the G2 bus's DMA channels: the AICA's, the two external ones and the
     * device's. then the PowerVR's DMA channel */
    DMA_REGS(0xa05f7800), DMA_REGS(0xa05f7820), DMA_REGS(0xa05f7840),
    DMA_REGS(0xa05f7860), DMA_REGS(0xa05f7c00),
    /* the PowerVR's render, framebuffer, tile accelerator and YUV
     * registers */
    REG(0xa05f8020), REG(0xa05f802c), REG(0xa05f8044), REG(0xa05f8048),
    REG(0xa05f8050), REG(0xa05f8054), REG(0xa05f8058), REG(0xa05f805c),
    REG(0xa05f8060), REG(0xa05f8064), REG(0xa05f8124), REG(0xa05f8128),
    REG(0xa05f812c), REG(0xa05f8130), REG(0xa05f8134), REG(0xa05f8138),
    REG(0xa05f8148), REG(0xa05f814c), REG(0xa05f8150)};
#define REGISTER_COUNT                                                         \
    ((int)(sizeof(sygIntAdrChk_adress) / sizeof(*sygIntAdrChk_adress)))
/* how many of the frame's words a row of the dump prints, and how many
 * hardware registers */
#define WORDS_PER_ROW 8
#define REGISTERS_PER_ROW 4
/* the interrupt codes of IRL levels 7, 5, 3 and 1 */
#define IRL7 0x300
#define IRL5 0x340
#define IRL3 0x380
#define IRL1 0x3c0
static Uint32 *frame_words;

static void syIntAdrChk_dumpline(const char *title, int first, int count);

void syIntChkDump(Uint16 event, Uint32 *frame)
{
    enum serial_channel channel;
    Uint32 saved_macl;
    int i;

    /* the codes the dump takes for an address checker trap */
    switch (event) {
    case IRL7:
    case IRL5:
    case IRL3:
    case IRL1:
        break;
    default:
        return;
    }
    frame_words = frame;
    channel = syIntAdrChk_init();
    syIntAdrChk_prints("\r\n --- ADDRESS CHECKER TRAP --- \r\n");
    /* @bug logic: prints float bank 1 under FR0-15 and bank 0 under
     * FR0-15(B), backward for a program interrupted with FPSCR.FR clear.
     * each master clears FR, then stores bank 0 into words 26 to 41 and bank
     * 1 into words 10 to 25. syIntMasterISR does so at 0x8c15ac08,
     * 0x8c15ac1e and 0x8c15ac2e, and syIntMasterISR_2 at 0x8c15ad64,
     * 0x8c15ad7a and 0x8c15ad8a */
    syIntAdrChk_dumpline("FR0-7:    ", FRAME_FR0, WORDS_PER_ROW);
    syIntAdrChk_dumpline("FR8-15:   ", FRAME_FR0 + WORDS_PER_ROW,
                         WORDS_PER_ROW);
    syIntAdrChk_dumpline("FR0-7(B): ", FRAME_BANKED_FR0, WORDS_PER_ROW);
    syIntAdrChk_dumpline("FR8-15(B):", FRAME_BANKED_FR0 + WORDS_PER_ROW,
                         WORDS_PER_ROW);
    syIntAdrChk_prints("FPUL: ");
    syIntAdrChk_printh(frame_words[FRAME_FPUL]);
    syIntAdrChk_prints(" FPSCR: ");
    syIntAdrChk_printh(frame_words[FRAME_FPSCR]);
    syIntAdrChk_dumpline("\r\nR0-7:     ", FRAME_R0, WORDS_PER_ROW);
    /* the frame holds no R15, and MACL's word follows R14's. the R8-15 row
     * borrows that word for SGR, which holds R15 as the exception found it */
    saved_macl = frame_words[FRAME_MACL];
    frame_words[FRAME_MACL] = frame_words[FRAME_SGR];
    syIntAdrChk_dumpline("R8-15:    ", FRAME_R8, WORDS_PER_ROW);
    frame_words[FRAME_MACL] = saved_macl;
    syIntAdrChk_dumpline("\r\nR0-7(B):  ", FRAME_BANKED_R0, WORDS_PER_ROW);
    syIntAdrChk_prints("MACL: ");
    syIntAdrChk_printh(frame_words[FRAME_MACL]);
    syIntAdrChk_prints(" MACH: ");
    syIntAdrChk_printh(frame_words[FRAME_MACH]);
    syIntAdrChk_prints("\r\nVBR: ");
    syIntAdrChk_printh(frame_words[FRAME_VBR]);
    syIntAdrChk_prints(" GBR: ");
    syIntAdrChk_printh(frame_words[FRAME_GBR]);
    syIntAdrChk_prints(" DBR: ");
    syIntAdrChk_printh(frame_words[FRAME_DBR]);
    syIntAdrChk_prints("\r\nPR: ");
    syIntAdrChk_printh(frame_words[FRAME_PR]);
    syIntAdrChk_prints(" PC: ");
    syIntAdrChk_printh(frame_words[FRAME_PC]);
    syIntAdrChk_prints(" SR: ");
    syIntAdrChk_printh(frame_words[FRAME_SR]);
    for (i = 0; i < REGISTER_COUNT; i++) {
        if (i % REGISTERS_PER_ROW == 0) {
            syIntAdrChk_prints("\r\n");
        }
        syIntAdrChk_printh((Uint32)sygIntAdrChk_adress[i]);
        syIntAdrChk_prints(":");
        syIntAdrChk_printh(*sygIntAdrChk_adress[i]);
        syIntAdrChk_prints(" ");
    }
    syIntAdrChk_prints("\r\n");
    switch (channel) {
    case CHANNEL_SCIF:
        syIntAdrChk_prints("Please, cancel the interrupt from ADRESS "
                           "CHECKER,\r\n then hit RETURN key.");
        break;
    case CHANNEL_SCI:
    default:
        syIntAdrChk_prints("Please, cancel the interrupt from ADRESS CHECKER.");
        break;
    }
    while (syIntAdrChk_getchar() != '\r') {
    }
    EXPEVT = event;
    syIntAdrChk_finish();
}

static void syIntAdrChk_dumpline(const char *title, int first, int count)
{
    int i;

    syIntAdrChk_prints(title);
    for (i = first; i < first + count; i++) {
        syIntAdrChk_printh(frame_words[i]);
        syIntAdrChk_prints(" ");
    }
    syIntAdrChk_prints("\r\n");
}
