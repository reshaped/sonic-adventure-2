/* roc_k56: the K56flex data pump, driven through its registers and its RAM */
#ifndef MODEM_ROC_K56_H
#define MODEM_ROC_K56_H
#include <sg_xpt.h>

/* each of the pump's registers is a byte, and they lie DP_REG_STRIDE bytes
 * apart */
#define DP_REG_STRIDE 4

/* a register of the data pump that the sources read and write whole */
union dp_byte {
    Uint8 byte;
    Uint8 stride[DP_REG_STRIDE];
};

/* SHC assigns each byte's fields from bit 7 down. Little-endian GCC and
 * Clang assign them from bit 0 up. Both layouts address the same pump bits. */

/* a register that holds bits names them in its member bit. unk_bitN is bit
 * N, whose role the sources don't show */

/* the speaker's volume, 0 to 3 */
union dp_volume {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 : 6;
        Uint8 level : 2;
#else
        Uint8 level : 2;
        Uint8 : 6;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* tone_c turns on tone detector C, and self_test sends the test pattern.
 * dp_enable_rdl sets v54pe with r07's rdl_enable, and a remote digital loop
 * clears it until the loop ends. rdl_asked is set while this side's request
 * for a remote digital loop waits for its answer. rdl_far_looped is set while
 * a loop the far end asked for runs */
union dp_r02 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 v54pe : 1;
        Uint8 rdl_asked : 1;
        Uint8 rdl_far_looped : 1;
        Uint8 : 2;
        Uint8 self_test : 1;
        Uint8 tone_c : 1;
        Uint8 : 1;
#else
        Uint8 : 1;
        Uint8 tone_c : 1;
        Uint8 self_test : 1;
        Uint8 : 2;
        Uint8 rdl_far_looped : 1;
        Uint8 rdl_asked : 1;
        Uint8 v54pe : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* guard sends a guard tone, and guard_select picks which */
union dp_r03 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 guard_select : 1;
        Uint8 guard : 1;
        Uint8 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 : 3;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 : 3;
        Uint8 unk_bit3 : 1;
        Uint8 : 1;
        Uint8 guard : 1;
        Uint8 guard_select : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* fifo turns on the transmit FIFO, whose room r0d's tx_room reports */
union dp_r04 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 short_train : 1;
        Uint8 : 3;
        Uint8 fifo : 1;
        Uint8 : 3;
#else
        Uint8 : 3;
        Uint8 fifo : 1;
        Uint8 : 3;
        Uint8 short_train : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

union dp_r05 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 : 3;
        Uint8 ceq : 1;
        Uint8 unk_bit4 : 1;
        Uint8 : 2;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 : 2;
        Uint8 unk_bit4 : 1;
        Uint8 ceq : 1;
        Uint8 : 3;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* the data's format. unk_low is bits 1 and 0. asynchronous data takes
 * unk_low 3, with unk_bit6, unk_bit3 and unk_bit2 clear */
union dp_format {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 unk_low : 2;
        Uint8 unk_bit2 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 : 1;
#else
        Uint8 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit2 : 1;
        Uint8 unk_low : 2;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* rdl_enable lets the far end start a remote digital loop, and rdl starts
 * one. digital_loop and analog_loop run the local loops */
union dp_r07 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 unk_bit0 : 1;
        Uint8 off_hook : 1;
        Uint8 : 1;
        Uint8 analog_loop : 1;
        Uint8 : 1;
        Uint8 digital_loop : 1;
        Uint8 rdl : 1;
        Uint8 rdl_enable : 1;
#else
        Uint8 rdl_enable : 1;
        Uint8 rdl : 1;
        Uint8 digital_loop : 1;
        Uint8 : 1;
        Uint8 analog_loop : 1;
        Uint8 : 1;
        Uint8 off_hook : 1;
        Uint8 unk_bit0 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* async selects asynchronous data. retrain asks the pump to retrain.
 * rdl_request asks the far end for a remote digital loop, rdl_grant grants
 * the far end's request, and rdl_release ends the loop. the pump clears each
 * once it is done */
union dp_r08 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rts : 1;
        Uint8 retrain : 1;
        Uint8 rdl_request : 1;
        Uint8 rdl_grant : 1;
        Uint8 rdl_release : 1;
        Uint8 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 async : 1;
#else
        Uint8 async : 1;
        Uint8 unk_bit6 : 1;
        Uint8 : 1;
        Uint8 rdl_release : 1;
        Uint8 rdl_grant : 1;
        Uint8 rdl_request : 1;
        Uint8 retrain : 1;
        Uint8 rts : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* while dtmf is set the pump dials by tones, and while it is clear by
 * pulses */
union dp_r09 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 unk_bit0 : 1;
        Uint8 : 1;
        Uint8 unk_bit2 : 1;
        Uint8 : 1;
        Uint8 originate : 1;
        Uint8 dtmf : 1;
        Uint8 : 1;
        Uint8 no_answer_tone : 1;
#else
        Uint8 no_answer_tone : 1;
        Uint8 : 1;
        Uint8 dtmf : 1;
        Uint8 originate : 1;
        Uint8 : 1;
        Uint8 unk_bit2 : 1;
        Uint8 : 1;
        Uint8 unk_bit0 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* flag is set while the pump hears V.21 during a high-speed fax modulation,
 * as dp_detect_v21_in_high_speed_fax reports */
union dp_r0a {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 unk_bit0 : 1;
        Uint8 : 2;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 flag : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 flag : 1;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 : 2;
        Uint8 unk_bit0 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* what the tone detectors hear. tone_a, tone_b and tone_c report detectors
 * A, B and C. answer_2100 and answer_2225 report the answer tones of those
 * frequencies in hertz */
union dp_tones {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 : 1;
        Uint8 unk_bit1 : 1;
        Uint8 : 1;
        Uint8 answer_2225 : 1;
        Uint8 answer_2100 : 1;
        Uint8 tone_c : 1;
        Uint8 tone_b : 1;
        Uint8 tone_a : 1;
#else
        Uint8 tone_a : 1;
        Uint8 tone_b : 1;
        Uint8 tone_c : 1;
        Uint8 answer_2100 : 1;
        Uint8 answer_2225 : 1;
        Uint8 : 1;
        Uint8 unk_bit1 : 1;
        Uint8 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* rx_waiting is set while a received byte waits in rx_data's FIFO */
union dp_r0c {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 unk_bit0 : 1;
        Uint8 rx_waiting : 1;
        Uint8 unk_bit2 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit2 : 1;
        Uint8 rx_waiting : 1;
        Uint8 unk_bit0 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* tx_room is set while the transmit FIFO takes another byte */
union dp_r0d {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 : 1;
        Uint8 tx_room : 1;
        Uint8 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 : 2;
#else
        Uint8 : 2;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit3 : 1;
        Uint8 : 1;
        Uint8 tx_room : 1;
        Uint8 : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* retrain and renegotiation report a retrain and a rate renegotiation that
 * the far end starts. bit.rate is the speed as V.34, K56flex and V.90 report
 * it, and old.rate as the other modulations do */
union dp_speed {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rate : 5;
        Uint8 renegotiation : 1;
        Uint8 : 1;
        Uint8 retrain : 1;
#else
        Uint8 retrain : 1;
        Uint8 : 1;
        Uint8 renegotiation : 1;
        Uint8 rate : 5;
#endif
    } bit;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rate : 4;
        Uint8 : 4;
#else
        Uint8 : 4;
        Uint8 rate : 4;
#endif
    } old;
    Uint8 stride[DP_REG_STRIDE];
};

/* the line's state, as dp_get_dcd, dp_get_cts, dp_get_dsr and dp_test_ring
 * report it. looping is set while the pump runs a loop. rdl_signal is set once
 * the far end asks for a remote digital loop, answers this side's request or
 * ends its loop, and the sources clear it */
union dp_status {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rdl_signal : 1;
        Uint8 : 1;
        Uint8 looping : 1;
        Uint8 ring : 1;
        Uint8 dsr : 1;
        Uint8 cts : 1;
        Uint8 unk_bit6 : 1;
        Uint8 dcd : 1;
#else
        Uint8 dcd : 1;
        Uint8 unk_bit6 : 1;
        Uint8 cts : 1;
        Uint8 dsr : 1;
        Uint8 ring : 1;
        Uint8 looping : 1;
        Uint8 : 1;
        Uint8 rdl_signal : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* attenuation is the transmit level's attenuation in dB, 0 to
 * MOST_ATTENUATION. clock is the transmit clock's setting, which
 * dp_enter_local_digital_loop sets to TX_CLOCK_LOOP */
union dp_tx_level {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 clock : 2;
        Uint8 : 2;
        Uint8 attenuation : 4;
#else
        Uint8 attenuation : 4;
        Uint8 : 2;
        Uint8 clock : 2;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};
#define TX_CLOCK_LOOP 3

/* auto_mode lets the handshake choose the modulation. rate_range lets it
 * choose among the rates it was given */
union dp_r15 {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rate_range : 1;
        Uint8 unk_bit1 : 1;
        Uint8 renegotiate : 1;
        Uint8 auto_mode : 1;
        Uint8 unk_bit4 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit5 : 1;
        Uint8 unk_bit4 : 1;
        Uint8 auto_mode : 1;
        Uint8 renegotiate : 1;
        Uint8 unk_bit1 : 1;
        Uint8 rate_range : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* tone_on is set while a DTMF tone sounds. digit_ready is set once digit
 * holds the tone's digit, 0 to 15 */
union dp_dtmf {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 digit : 4;
        Uint8 digit_ready : 1;
        Uint8 : 1;
        Uint8 tone_on : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 tone_on : 1;
        Uint8 : 1;
        Uint8 digit_ready : 1;
        Uint8 digit : 4;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* an access to the pump's RAM. address holds bits 8 to 11 of the RAM's
 * address, and high its bit 14. write makes the access a write. setting busy
 * starts the access, and the pump clears it when the access is done */
union dp_ram_control {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 address : 4;
        Uint8 : 1;
        Uint8 write : 1;
        Uint8 high : 1;
        Uint8 busy : 1;
#else
        Uint8 busy : 1;
        Uint8 high : 1;
        Uint8 write : 1;
        Uint8 : 1;
        Uint8 address : 4;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* rx_full is set while rx_data holds a byte, and tx_empty while tx_data
 * takes one. rx_irq and tx_irq turn on the pump's interrupt for each */
union dp_irq {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 rx_full : 1;
        Uint8 : 1;
        Uint8 rx_irq : 1;
        Uint8 tx_empty : 1;
        Uint8 : 1;
        Uint8 tx_irq : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 tx_irq : 1;
        Uint8 : 1;
        Uint8 tx_empty : 1;
        Uint8 rx_irq : 1;
        Uint8 : 1;
        Uint8 rx_full : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* setting newc makes the pump take a new configuration, and it clears newc
 * once it has. new_status is set on a change of status, and status_irq
 * interrupts on it */
union dp_control {
    Uint8 byte;
    struct {
#ifdef NONMATCHING
        /* this side lists the fields from bit 0 up, the order GCC assigns them
         * in */
        Uint8 newc : 1;
        Uint8 : 1;
        Uint8 unk_bit2 : 1;
        Uint8 new_status : 1;
        Uint8 status_irq : 1;
        Uint8 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 unk_bit7 : 1;
#else
        Uint8 unk_bit7 : 1;
        Uint8 unk_bit6 : 1;
        Uint8 : 1;
        Uint8 status_irq : 1;
        Uint8 new_status : 1;
        Uint8 unk_bit2 : 1;
        Uint8 : 1;
        Uint8 newc : 1;
#endif
    } bit;
    Uint8 stride[DP_REG_STRIDE];
};

/* the data pump's registers, 0x00 to 0x1f. a register named rNN is
 * register 0xNN, whose role no one name covers */
struct dp_registers {
    union dp_byte rx_data;
    union dp_volume volume;
    union dp_r02 r02;
    union dp_r03 r03;
    union dp_r04 r04;
    union dp_r05 r05;
    union dp_format format;
    union dp_r07 r07;
    union dp_r08 r08;
    union dp_r09 r09;
    union dp_r0a r0a;
    union dp_tones tones;
    union dp_r0c r0c;
    union dp_r0d r0d;
    union dp_speed speed;
    union dp_status status;
    union dp_byte tx_data;
    union dp_byte r11;
    /* the configuration: the modulation the pump runs, and its rate */
    union dp_byte conf;
    union dp_tx_level tx_level;
    /* a code the pump reports, such as the id of a RAM module it asks for */
    union dp_byte abcode;
    union dp_r15 r15;
    union dp_byte r16;
    union dp_byte r17;
    /* the RAM data's low byte, and its high byte */
    union dp_byte ram_data;
    union dp_byte ram_data_high;
    union dp_byte r1a;
    union dp_dtmf dtmf;
    /* the RAM address's low byte */
    union dp_byte ram_addr;
    union dp_ram_control ram_control;
    union dp_irq irq;
    union dp_control control;
};

/* the configurations of conf that the sources name. a V.34, K56flex or V.32
 * configuration is its class plus the pump's index of its rate. the class
 * takes the bits of CONF_CLASS, and the index the low four bits. a V.90
 * configuration is CONF_V90 plus its index in the low five bits. with index
 * 0, CONF_V32 and CONF_V34 run the clear down that ends a call */
#define CONF_CLASS 0xf0
#define CONF_V32 0x70
#define CONF_K56 0x90
#define CONF_V34 0xc0
#define CONF_V90 0xe0
/* V.32 bis at 14400 bps */
#define CONF_V32BIS_14400 0x76
/* V.21. its class also holds V.23's configurations, V.21's for fax, and
 * CONF_AUTO */
#define CONF_V21 0xa0
/* the handshake chooses the modulation, from V.34 and V.90 */
#define CONF_AUTO 0xaa
/* the pump sends the tone generator's tone in CONF_TONE, and no tone in
 * CONF_SILENCE. the pump dials in CONF_DIAL */
#define CONF_TONE 0x80
#define CONF_DIAL 0x81
#define CONF_SILENCE 0x83

/* the bits of dp_set_modulation_type's mask, a modulation each. MOD_BELL is
 * Bell 103 and 212A */
#define MOD_BELL 0x01
#define MOD_V21 0x02
#define MOD_V23 0x04
#define MOD_V22 0x08
#define MOD_V22BIS 0x10
#define MOD_V32 0x20
#define MOD_V32BIS 0x40
#define MOD_V34 0x80
#define MOD_K56 0x100
#define MOD_V90 0x200
/* the mask of a configuration dp_get_modulation_type doesn't know */
#define MOD_UNKNOWN 0x8000
/* with MOD_V32, V.32's trellis-coded 9600 bps */
#define MOD_TRELLIS 0x800000UL

/* the codes roc_k56 reports to data_pump_error, each for the wait or the call
 * that fails */
#define DP_ERR_RESET 1100
#define DP_ERR_RESET_TX 1120
#define DP_ERR_IDLE 1160
#define DP_ERR_ASYNC 1220
#define DP_ERR_ABCODE 1240
#define DP_ERR_DOWNLOAD 1250
#define DP_ERR_TEST_NEWC 1260
#define DP_ERR_ANALOG_LOOP 1270
#define DP_ERR_DIGITAL_LOOP 1290
#define DP_ERR_RAM_READ 1300
#define DP_ERR_RAM_WRITE 1310
#define DP_ERR_NEWC 1330
#define DP_ERR_SYNC_CTS_OFF 1340
#define DP_ERR_SYNC_CTS_ON 1350
#define DP_ERR_SYNC_DRAIN 1360
#define DP_ERR_ASYNC_CTS_OFF 1370
#define DP_ERR_ASYNC_CTS_ON 1380
#define DP_ERR_DISABLE_CTS_OFF 1390
#define DP_ERR_DISABLE_CTS_ON 1400
#define DP_ERR_INTERRUPT 1470

/* roc_k56's flags. a flag named unk_B_N is bit N of byte B, whose role the
 * sources don't show */
struct rc_flags {
#ifdef NONMATCHING
    /* this side lists each byte's fields from bit 0 up, the order GCC assigns
     * them in */
    Uint8 : 1;
    Uint8 training_progressed : 1;
    Uint8 renegotiation_retrained : 1;
    Uint8 renegotiation_started : 1;
    Uint8 retrain_started : 1;
    Uint8 renegotiating : 1;
    Uint8 retraining : 1;
    Uint8 unk_0_7 : 1;

    Uint8 unk_1_0 : 1;
    Uint8 ceq_checked : 1;
    Uint8 v32bis_fallback_checked : 1;
    Uint8 unk_1_3 : 1;
    Uint8 saved_rate_range : 1;
    Uint8 v34_measurement_configured : 1;
    Uint8 training_table_written : 1;
    Uint8 rdl_enabled : 1;

    Uint8 saved_status_irq : 1;
    Uint8 saved_rx_irq : 1;
    Uint8 saved_tx_irq : 1;
    Uint8 tx_irq_deferred : 1;
    Uint8 unk_2_4 : 1;
    Uint8 unk_2_5 : 1;
    Uint8 unk_2_6 : 1;
    Uint8 unk_2_7 : 1;

    Uint8 carrier_seen : 1;
    Uint8 interrupt_on_carrier_loss : 1;
    Uint8 tx_only : 1;
    Uint8 rx_only : 1;
    Uint8 in_interrupt : 1;
    Uint8 saved_fifo : 1;
    Uint8 unk_3_6 : 1;
    Uint8 : 1;

    Uint8 : 8;
#else
    Uint8 unk_0_7 : 1;
    /* retraining or renegotiating tells which of a retrain and a rate
     * renegotiation started last. dp_init_handshake clears both */
    Uint8 retraining : 1;
    Uint8 renegotiating : 1;
    /* this side started the retrain. dp_init_retrain sets it, and MonitorV34RR
     * and MonitorV90RR set it where this side's renegotiation turns into a
     * retrain */
    Uint8 retrain_started : 1;
    /* dp_init_rate_renegotiation started the renegotiation, not the far
     * end */
    Uint8 renegotiation_started : 1;
    /* MonitorV34RR and MonitorV90RR set it where a renegotiation turns into a
     * retrain. InitWaVariables and each renegotiation's start clear it */
    Uint8 renegotiation_retrained : 1;
    /* the training monitors set it once r16 and r17 show the training under
     * way, or V.32's r0c does. a retrain this side started stays pending
     * until it is set */
    Uint8 training_progressed : 1;
    Uint8 : 1;
    /* set from dp_enable_rdl until dp_disable_rdl */
    Uint8 rdl_enabled : 1;
    Uint8 training_table_written : 1;
    Uint8 v34_measurement_configured : 1;
    /* r15's rate_range as dp_init_handshake left it */
    Uint8 saved_rate_range : 1;
    Uint8 unk_1_3 : 1;
    /* set once an originating dp_monitor_handshake has decided whether to
     * fall back to CONF_V32BIS_14400. dp_init_handshake clears it */
    Uint8 v32bis_fallback_checked : 1;
    /* set once dp_monitor_handshake, outside auto mode, has decided whether
     * to clear r05's ceq. InitWaVariables clears it */
    Uint8 ceq_checked : 1;
    Uint8 unk_1_0 : 1;
    Uint8 unk_2_7 : 1;
    Uint8 unk_2_6 : 1;
    Uint8 unk_2_5 : 1;
    Uint8 unk_2_4 : 1;
    /* while set, dp_tx_irqs_enabled reports the transmit interrupt on, and
     * dp_analyzer turns it on once tx_irq_timer passes. nothing sets it */
    Uint8 tx_irq_deferred : 1;
    /* irq's tx_irq and rx_irq, and control's status_irq, as dp_interrupt
     * found them. it restores them as it returns, unless it gave up */
    Uint8 saved_tx_irq : 1;
    Uint8 saved_rx_irq : 1;
    Uint8 saved_status_irq : 1;
    Uint8 : 1;
    Uint8 unk_3_6 : 1;
    /* r04's fifo as dp_enable_data_mode's mode 2 found it. nothing reads it */
    Uint8 saved_fifo : 1;
    /* set from dp_interrupt's start until it returns without giving up, or
     * dp_init_data_pump runs */
    Uint8 in_interrupt : 1;
    /* dp_enable_data_mode's mode 0 at speed 1 sets rx_only where rts is off,
     * and tx_only where it is on. under rx_only, dp_ready_to_tx_data reports
     * no room. under tx_only, dp_enable_data_mode turns tx_irq on in place of
     * rx_irq, and dp_set_idle lets tx_data empty first */
    Uint8 rx_only : 1;
    Uint8 tx_only : 1;
    /* dp_init_handshake sets interrupt_on_carrier_loss to answer a fax in
     * configuration 0x01 or 0x02. while it is set, dp_carrier_detect sets
     * carrier_seen as it reports a carrier, and runs dp_interrupt once when
     * that carrier goes */
    Uint8 interrupt_on_carrier_loss : 1;
    Uint8 carrier_seen : 1;
    Uint8 : 8;
#endif
};

/* roc_k56's K56flex and V.90 flags, named as rc_flags' are */
struct k56_flags {
#ifdef NONMATCHING
    /* this side lists each byte's fields from bit 0 up, the order GCC assigns
     * them in */
    Uint8 unk_0_0 : 1;
    Uint8 unk_0_1 : 1;
    Uint8 fallback_limits_set : 1;
    Uint8 v34_download : 1;
    Uint8 unk_0_4 : 1;
    Uint8 v90_download : 1;
    Uint8 keep_v34_rate_limits : 1;
    Uint8 auto_conf : 1;

    Uint8 unk_1_0 : 1;
    Uint8 post_v8_download : 1;
    Uint8 k56_equalizer_checked : 1;
    Uint8 in_k56_training_window : 1;
    Uint8 v90_modulation_checked : 1;
    Uint8 v90_as_v34 : 1;
    Uint8 k56_as_v34 : 1;
    Uint8 unk_1_7 : 1;

    Uint8 : 2;
    Uint8 training_rate_programmed : 1;
    Uint8 fallback_requested : 1;
    Uint8 measure_eqm : 1;
    Uint8 v34_retrain_counted : 1;
    Uint8 receive_limit_adjusted : 1;
    Uint8 training_control_cleared : 1;

    Uint8 v34_progress_timer_set : 1;
    Uint8 renegotiation_timer_set : 1;
    Uint8 v8_workaround_enabled : 1;
    Uint8 v8_parameter_checked : 1;
    Uint8 k56_capabilities_checked : 1;
    Uint8 b11_timing : 1;
    Uint8 v90_state72_adjusted : 1;
    Uint8 unk_3_7 : 1;

    Uint8 : 6;
    Uint8 v34_stall_command_sent : 1;
    Uint8 v34_stall_recovery_started : 1;
#else
    /* set once dp_set_modulation_type chooses CONF_AUTO, until conf leaves
     * CONF_AUTO's class */
    Uint8 auto_conf : 1;
    Uint8 keep_v34_rate_limits : 1;
    Uint8 v90_download : 1;
    Uint8 unk_0_4 : 1;
    Uint8 v34_download : 1;
    Uint8 fallback_limits_set : 1;
    Uint8 unk_0_1 : 1;
    Uint8 unk_0_0 : 1;
    Uint8 unk_1_7 : 1;
    /* roc_wa's training monitors set k56_as_v34 where a K56flex configuration
     * trains as V.34. they set v90_as_v34 where a V.90 one does.
     * InitWaVariables clears both. while either is set, roc_k56 treats the
     * configuration as a V.34 one */
    Uint8 k56_as_v34 : 1;
    Uint8 v90_as_v34 : 1;
    Uint8 v90_modulation_checked : 1;
    Uint8 in_k56_training_window : 1;
    Uint8 k56_equalizer_checked : 1;
    /* while set, dp_monitor_handshake calls PostV8DownloadRamModule */
    Uint8 post_v8_download : 1;
    Uint8 unk_1_0 : 1;
    Uint8 training_control_cleared : 1;
    Uint8 receive_limit_adjusted : 1;
    Uint8 v34_retrain_counted : 1;
    Uint8 measure_eqm : 1;
    Uint8 fallback_requested : 1;
    Uint8 training_rate_programmed : 1;
    Uint8 : 2;
    Uint8 unk_3_7 : 1;
    Uint8 v90_state72_adjusted : 1;
    /* set as a V.90 connection starts, until dp_analyzer clears RAM 0xb11 3
     * seconds later. a rate renegotiation or InitWaVariables that runs before
     * then clears it */
    Uint8 b11_timing : 1;
    Uint8 k56_capabilities_checked : 1;
    Uint8 v8_parameter_checked : 1;
    Uint8 v8_workaround_enabled : 1;
    /* set once MonitorV34RR or MonitorV90RR starts renegotiation_deadline.
     * each renegotiation's start clears it, and so does MonitorV90RR's
     * retrain */
    Uint8 renegotiation_timer_set : 1;
    Uint8 v34_progress_timer_set : 1;
    Uint8 v34_stall_recovery_started : 1;
    Uint8 v34_stall_command_sent : 1;
    Uint8 : 6;
#endif
};

/* the far end's K56flex capabilities, as dp_set_K56_parameters keeps
 * them */
struct k56flexinfotag {
    Uint16 unk_0;
    Uint8 unk_2;
    Uint8 unk_3;
    Uint8 unk_4;
};

/* rcdpi points at the pump's registers once dp_test has run */
extern volatile struct dp_registers *rcdpi;
extern struct rc_flags rcflags;
extern struct k56_flags k56flags;
/* the handshake's state. it is 0 while there is none to report, and
 * HANDSHAKE_FAILED or HANDSHAKE_CONNECTED once the handshake ends.
 * MonitorV32Train sets other values */
#define HANDSHAKE_FAILED 7
#define HANDSHAKE_CONNECTED 32
extern Uint8 handshake_status;
/* only roc_wa uses Wa5State */
extern Uint8 Wa5State;
/* the speaker's level as dp_set_volume last set it, 0 to 3 */
extern Uint8 volume_state;
/* the highest V.34 rates sent and received, and the highest V.90 rate
 * received, that the last configuration allows. each is the pump's index of
 * the rate, as conf's rate bits hold it. the two K56flex speeds stay 0 */
extern Uint8 MaxV34TransmitSpeed;
extern Uint8 MaxK56TransmitSpeed;
extern Uint8 MaxV34ReceiveSpeed;
extern Uint8 MaxK56ReceiveSpeed;
extern Uint8 MaxV90ReceiveSpeed;
/* dp_snr_ratio's row of grades, 0 to 2: bits 6 and 7 of the parameters
 * dp_set_v34_parameters last took */
extern Uint8 AgressivityLevel;
/* the options dp_set_K56_parameters last took */
extern Uint8 K56options;
/* a gain of 1, in TxLevelGain's units */
#define UNITY_GAIN 0x4000
/* the gain that sends the attenuation past MOST_ATTENUATION, as
 * dp_set_tx_level last set it, or UNITY_GAIN for none */
extern Uint16 TxLevelGain;
/* the V.32 rate sequences of the last connection. MonitorV32Train sets
 * rate_seq_R1, and dp_monitor_handshake and dp_monitor_retrain set the
 * other two */
extern Uint16 rate_seq_R1;
extern Uint16 rate_seq_R2;
extern Uint16 rate_seq_R3;
extern Uint16 e_seq;
/* only roc_wa uses K56_training_cnt */
extern Uint16 K56_training_cnt;

/* reset the pump as dp_init_data_pump does */
void dp_reset_data_pump(void);
/* reset the pump. where it comes up, download its first RAM modules and turn
 * its interrupts off. then clear the tone detectors and the last call's
 * state, and set the pump to originate */
void dp_init_data_pump(Uint8 unused_1, Uint8 unused_2);
/* point rcdpi at the pump's registers, and reset the pump. return 1 where the
 * pump comes up. otherwise report DP_ERR_RESET or DP_ERR_RESET_TX, and return
 * 0 */
char dp_test(void);

void dp_set_originate(void);
void dp_set_answer(void);
void dp_set_ceq(Uint8 on);
/* do nothing */
void dp_set_v13(void);
void dp_disable_v25_answer_tone(void);
/* a level, as dp_set_tx_level, dp_set_dtmf and dp_transmit_PTT take it, is an
 * attenuation in dB plus LEVEL_OFFSET. tx_level's attenuation holds up to
 * MOST_ATTENUATION dB */
#define LEVEL_OFFSET 3
#define MOST_ATTENUATION 15
/* set the transmit attenuation to level less LEVEL_OFFSET dB, or 0 dB for a
 * smaller level. TxLevelGain takes what passes MOST_ATTENUATION. level must
 * be 43 or less: past that, the call reads beyond its table of 25 gains */
void dp_set_tx_level(Uint8 level);
/* a tone of 0 sends no guard tone, and 1 or 2 the first or the second. any
 * other tone sends the one last chosen */
#define GUARD_TONE_NONE 0
#define GUARD_TONE_FIRST 1
#define GUARD_TONE_SECOND 2
void dp_set_guard_tone(Uint8 tone);

/* what dp_answer_tone_filter_test returns, by the tone it hears */
#define HEARD_2100HZ 1
#define HEARD_2225HZ 2
#define HEARD_1300HZ 3
/* return the first of the HEARD_ tones above that the pump hears, or 0 for
 * none. tone detector B hears 2225 Hz and detector C 1300 Hz, as
 * dp_set_handshake_tone_detector tunes them. detector C counts only while it
 * isn't set to 1650 Hz */
Uint8 dp_answer_tone_filter_test(void);
/* send a tone of frequency 1100, 1300 or 2100 Hz. any other frequency leaves
 * the generator's tone as it was */
void dp_start_tone_generator(Uint16 frequency);
/* stop the tone, and leave the pump silent */
void dp_stop_tone_generator(void);

/* dial by tones, at level as dp_set_tx_level takes it, with at most
 * MOST_ATTENUATION dB of attenuation. table 1, 2 or 3 picks the tones' levels,
 * and any other table picks 2 */
void dp_set_dtmf(Uint8 level, Uint8 table);
/* dial by pulses */
void dp_set_pulse(void);
/* set how long each tone sounds, and the gap after it. a unit of time is 2.5
 * of the pump's counts. a count rounds down, and the pump keeps its low 16
 * bits. each time must be below 0x8000 */
void dp_set_dtmf_duration(Uint16 on_time, Uint16 off_time);
/* set the make and the break of each pulse, and the pause after a digit. a
 * unit of time is 2.4 of the pump's counts, and a count rounds down. the pump
 * keeps the low 16 bits of pause's count, and the low 8 bits of the others.
 * each time must be below 0x8000 */
void dp_set_make_break_ratio(Uint16 make, Uint16 brk, Uint16 pause);
/* ready the pump to dial, and return 20 ms later */
void dp_set_dial_mode(void);
/* return 1 while the pump can take the next digit, and 0 otherwise */
char dp_ready_to_dial(void);
/* dial digit, 0 to 15. digits 10 to 13 are A to D, and 14 and 15 are * and
 * # */
void dp_dial_number(Uint8 digit);

/* each rate that the functions below take or store is a rate code. a higher
 * code is a faster rate. V.34's codes run from RATE_V34_MIN to RATE_V34_MAX,
 * and the codes of V.90's receive rates from RATE_V90_RX_MIN to
 * RATE_V90_RX_MAX */
#define RATE_V34_MIN 5
#define RATE_V34_MAX 32
#define RATE_V90_RX_MIN 24
#define RATE_V90_RX_MAX 56
/* the codes of the rates the other modulations start or stop at, by their
 * bits per second */
#define RATE_75 1
#define RATE_300 2
#define RATE_600 3
#define RATE_1200 4
#define RATE_2400 5
#define RATE_4800 6
#define RATE_9600 8
#define RATE_14400 12
#define RATE_32000 30
#define RATE_56000 56

/* configure the pump for the highest modulation of mask that the rates fit.
 * V.34 runs at min_rx_rate or above. it receives at max_rx_rate or below, and
 * sends at max_rate or below. V.90 receives at max_rx_rate or below, and sends
 * from min_rate to max_rate. the other modulations take min_rate and max_rate
 * alone. a max_rate of 0 means RATE_V34_MAX, and a max_rx_rate of 0 means
 * RATE_V90_RX_MAX.
 *
 * MOD_K56 and MOD_V90 count as MOD_V34 unless dp_set_v34_parameters last took
 * V34_ALLOW_K56_V90. a mask that selects no configuration gives CONF_V21, and
 * MOD_K56 selects none. before a carrier, V.34 and V.90 take CONF_AUTO unless
 * the V.25 answer tone is off. while a carrier is up and r15's rate_range is
 * set, the pump keeps the configuration it has */
void dp_set_modulation_type(Uint32 mask, Uint8 min_rate, Uint8 max_rate,
                            Uint8 min_rx_rate, Uint8 max_rx_rate);
/* start the handshake that conf's configuration runs */
void dp_init_handshake(void);
/* what dp_monitor_handshake and dp_monitor_retrain return */
#define TRAIN_PENDING 1
#define TRAIN_RUNNING 2
#define TRAIN_CONNECTED 3
#define TRAIN_RENEGOTIATED 4
#define TRAIN_FAILED 5
/* follow one pass of the handshake. return TRAIN_CONNECTED once the call
 * connects, and TRAIN_FAILED once the handshake or its RAM module's download
 * fails. until then, return TRAIN_RUNNING while handshake_status is set or the
 * module downloads, and TRAIN_PENDING otherwise */
Uint8 dp_monitor_handshake(void);
/* store the connection's modulation in *mask, and its rates in *tx_rate and
 * *rx_rate. a configuration the call doesn't know stores MOD_UNKNOWN in *mask,
 * and 0xff in each rate */
void dp_get_modulation_type(Uint32 *mask, Uint8 *tx_rate, Uint8 *rx_rate);
/* return the rate sequence of the last connection, or for V.90 the far end's
 * capabilities */
Uint32 dp_get_rate_sequence(void);
/* the handshake reads RAM 0x302 and 0x303 as it connects. return 1 where bit
 * 4 of 0x302 was set and bits 5 to 7 of 0x303 held 1. return 0 otherwise */
Uint32 dp_get_v8_status(void);
/* do nothing */
void dp_set_v8_ci(char ci);
/* set V.8's options in the pump's RAM, and rcflags' unk_0_7, from
 * parameters' bits */
void dp_set_v8_parameters(Uint8 parameters);
/* the bit of dp_set_v34_parameters' parameters that keeps MOD_K56 and
 * MOD_V90 in dp_set_modulation_type's mask. the bit also sets bit 6 of RAM
 * 0x13f */
#define V34_ALLOW_K56_V90 0x01
/* set V.34's options in the pump's RAM from parameters' bits, and keep
 * parameters for dp_set_modulation_type. bits 6 and 7 give AgressivityLevel,
 * which must be 0 to 2 */
void dp_set_v34_parameters(Uint32 parameters);

/* return the received signal's level, from the pump's measure of its power.
 * in the configurations the call knows, a weaker signal gives a higher level.
 * in any other, such as CONF_AUTO, the level rises with the power */
Uint16 dp_read_rec_level(void);
/* return the pump's EQM reading, a word that dp_snr_ratio doesn't grade */
Uint16 dp_read_eqm(void);
/* return 1 while status reports both dcd and unk_bit6, and 0 otherwise. in a
 * local digital loop, unk_bit6 alone decides. outside one, configurations
 * CONF_V32, 0xaf, CONF_V34 and 0x40 return 0.
 *
 * while rcflags' interrupt_on_carrier_loss is set, a return of 1 sets
 * carrier_seen. a later return of 0 for want of dcd or unk_bit6 then runs
 * dp_interrupt once and clears carrier_seen */
char dp_carrier_detect(void);
/* while dcd is set, return 1 where dp_read_rec_level returns less than level,
 * and 0 otherwise. while dcd is clear, return dp_filter_test's result */
Uint8 dp_energy_on_line(Uint8 level);
/* return the line's grade, from 0 for the best to 3 for the worst. while
 * tones' unk_bit1 is set, return 3 and clear it. an analog loop and
 * configurations 0xc1 to 0xce, which are V.34's, ignore the bit. Bell 103,
 * V.21 and V.23 grade 0. K56flex and V.90 grade RAM 0x810's word, unless they
 * run V.34. the rest grade RAM 0x26f's byte. AgressivityLevel picks the row of
 * thresholds for each */
Uint8 dp_snr_ratio(void);
/* return the round trip delay in ms for V.34, V.32 and configurations 0x46 to
 * 0x4c. return 0 for others, and where the delay computes below 0 */
Uint16 dp_read_round_trip_delay(void);
/* return 0 */
Uint16 dp_read_near_echo_level(void);
/* return 50 */
Uint16 dp_read_far_echo_level(void);

/* set the pump's ring detector. the call keeps on and off, and nothing reads
 * them */
void dp_set_ring_periods(Uint8 on, Uint8 off);
/* return 1 while the line rings, and 0 otherwise. the call first clears r09's
 * unk_bit2 where that is set */
char dp_test_ring(void);
void dp_off_hook(void);
void dp_on_hook(void);

/* do nothing */
void dp_set_sync_mode(Uint8 mode);
/* set the pump to asynchronous data. where the pump doesn't take the format
 * within 500 ms, report DP_ERR_ASYNC and return */
void dp_set_async_mode(void);
char dp_get_dsr(void);

/* loop the pump's transmitter to its receiver, and return once status reports
 * dsr. each try sets the loop up again, and the call gives up after 2 seconds
 * with DP_ERR_ANALOG_LOOP. for V.34 each try downloads a RAM module. a
 * download that fails reports DP_ERR_ABCODE, DP_ERR_DOWNLOAD or
 * DP_ERR_TEST_NEWC, and the call returns */
void dp_enter_local_analog_loop(void);
/* store the pump's count of errors in *errors. return 1 while cts is on and
 * dp_carrier_detect returns 1, and 0 otherwise */
char dp_monitor_local_analog_loop(Uint16 *errors);
/* let the far end start a remote digital loop */
void dp_enable_rdl(void);
/* stop letting the far end start a remote digital loop */
void dp_disable_rdl(void);
/* ask the far end for a remote digital loop. for V.34 the call asks only while
 * the connection sends and receives at one speed */
void dp_enter_remote_digital_loop(void);
/* what dp_monitor_rdl_response and dp_monitor_remote_rdl return */
#define LOOP_CHANGING 1
#define LOOP_RUNNING 2
/* return LOOP_RUNNING while the far end loops, LOOP_CHANGING while the loop
 * ends, and 0 otherwise. *errors gets the pump's count of errors while this
 * side's loop runs the test pattern, and 0 otherwise */
Uint8 dp_monitor_rdl_response(Uint16 *errors);
/* stop the test pattern and the remote digital loop */
void dp_stop_remote_digital_loop(void);
/* start the test pattern, and return 100 ms later */
void dp_enter_self_test(void);
/* loop the pump's receiver to its transmitter */
void dp_enter_local_digital_loop(void);
/* end the local digital loop. where status' looping stays set for 500 ms,
 * report DP_ERR_DIGITAL_LOOP. tx_level's clock then stays as the loop set
 * it */
void dp_stop_local_digital_loop(void);
/* answer a remote digital loop the far end asks for. return LOOP_CHANGING
 * while the loop starts, LOOP_RUNNING while it runs, and 0 otherwise. return 0
 * before dp_enable_rdl and after dp_disable_rdl */
Uint8 dp_monitor_remote_rdl(void);

/* turn data mode off, and start a fast retrain of V.90. dp_monitor_retrain
 * follows it as a rate renegotiation */
void dp_init_V90_fast_retrain(void);
/* turn data mode off, and retrain the connection's modulation at the rates
 * given, as dp_set_modulation_type takes them */
void dp_init_retrain(Uint8 min_rate, Uint8 max_rate, Uint8 min_rx_rate,
                     Uint8 max_rx_rate);
/* turn data mode off, and renegotiate the connection's rates. for MOD_V32BIS,
 * min_rate and max_rate bound the rates sent and received. for any other
 * modulation they bound the rates received, and the rates sent run from 0 to
 * RATE_V34_MAX. MOD_V32 takes no receive rate, so it ignores them */
void dp_init_rate_renegotiation(Uint8 min_rate, Uint8 max_rate);
/* what dp_detect_retrain returns */
#define DETECT_RETRAIN 1
#define DETECT_RENEGOTIATION 2
/* return DETECT_RETRAIN once the far end starts a retrain, and
 * DETECT_RENEGOTIATION once it starts a rate renegotiation. the call turns
 * data mode off for either. return 0 otherwise */
Uint8 dp_detect_retrain(void);
/* follow one pass of the retrain or the rate renegotiation that runs. once it
 * succeeds, turn data mode on again. return TRAIN_CONNECTED once a retrain
 * ends, TRAIN_RENEGOTIATED once a renegotiation ends, and TRAIN_FAILED once
 * either fails. until then, return TRAIN_PENDING while this side's request is
 * pending, and TRAIN_RUNNING otherwise */
Uint8 dp_monitor_retrain(void);

/* return the byte at address of the pump's RAM for a width of 1. any other
 * width returns the word there. address takes bits 0 to 11 and 14. return 0
 * while target_HwFault returns nonzero. where the pump doesn't end the read
 * within 500 ms, report DP_ERR_RAM_READ and return 0 */
Uint16 read_dp_ram(Uint16 address, Uint8 width);
/* write value's low byte at address, as read_dp_ram takes it. a width of 2
 * writes the whole word. do nothing while target_HwFault returns nonzero.
 * where the pump doesn't end the write within 500 ms, report
 * DP_ERR_RAM_WRITE */
void write_dp_ram(Uint16 address, Uint16 value, Uint8 width);

/* words of the pump's RAM. RAM_EQM holds the pump's EQM reading, and RAM_BAUD
 * the symbol rate's index. RAM_RX_RATE and RAM_TX_RATE hold the indexes of the
 * rates received and sent, and RAM_MAX_RX_RATE and RAM_MAX_TX_RATE the highest
 * index each may take. RAM_V34_RATES holds a mask of the rates a V.34
 * connection may take */
#define RAM_EQM 0x20c
#define RAM_BAUD 0x2e3
#define RAM_RX_RATE 0x2e4
#define RAM_TX_RATE 0x2e5
#define RAM_V34_RATES 0x382
#define RAM_RATE_RANGE_CONTROL 0x3a5
#define RAM_MAX_RX_RATE 0x604
#define RAM_MAX_TX_RATE 0x605

/* write a biquad filter's five coefficient words at RAM base onward */
void init_biquad_cc(Uint16 base, Uint16 b0, Uint16 b1, Uint16 b2, Uint16 a1,
                    Uint16 a2);
/* write the four words of tone detector detector beside its filters. a
 * detector of 0, 1 or 2 is A, B or C. each word goes to the RAM address its
 * parameter names, plus the detector's offset. at_ba0 and at_aa0 take 6 more
 * for each detector past A, and at_ab8 and at_bb8 take 1 more */
void init_biquad_parameter_cc(Uint16 detector, Uint16 at_ba0, Uint16 at_aa0,
                              Uint16 at_ab8, Uint16 at_bb8);
#define TONE_DETECTOR_A 0
#define TONE_DETECTOR_B 1
#define TONE_DETECTOR_C 2
#define TONE_DETECTORS 3
/* the RAM of each tone detector's first filter, as init_biquad_cc takes it.
 * the detector's second filter lies SECOND_FILTER words on */
#define TONE_A_FILTER 0x0aa1
#define TONE_B_FILTER 0x0aa7
#define TONE_C_FILTER 0x0aad
#define SECOND_FILTER 0x0100
/* a pair of filters that detector C's setup sets to pass the line through */
#define TONE_C_PASS_FILTER 0x0ab2
/* clear tonec_at_1650 and tonec_at_2100 */
void init_tonec_cc(void);

/* make the pump take its new configuration, and wait until it has. after
 * 200 ms, report DP_ERR_NEWC and return */
void newc(void);

/* do roc_k56's timed work. in pulse dialing, pass 0 to
 * target_dpNeedsHookControl 100 ms after the pump takes the last digit.
 * outside pulse dialing, pass it 0 at once. while k56flags' b11_timing is set,
 * clear RAM 0xb11 and the flag 3 seconds after a V.90 connection. once
 * rcflags' tx_irq_deferred is set, clear it and r07's unk_bit0, and turn
 * tx_irq on */
void dp_analyzer(void);

/* turn data mode on in mode, and hook dp_interrupt on the pump's interrupt
 * with target_InstallDpIrq. dp_monitor_retrain turns data mode on again with
 * the same mode, option and speed.
 *
 * mode 2 sets format's unk_bit4, and a mode past 2 clears it. outside a fax
 * configuration, the call drops rts and waits for cts to drop before the
 * change. it then restores rts, and waits for cts to return where rts is on.
 * while k56flags' unk_3_7 is set, mode 2 makes the change without those
 * waits. mode 2 then reads and discards each byte waiting in rx_data. its
 * option 1 sets bit 0 of RAM 0xb3, and any other option clears it.
 *
 * each wait gives up after 500 ms. the call then reports the wait's error, and
 * returns without hooking dp_interrupt. mode 2's are DP_ERR_SYNC_CTS_OFF,
 * DP_ERR_SYNC_CTS_ON and DP_ERR_SYNC_DRAIN. a mode past 2's are
 * DP_ERR_ASYNC_CTS_OFF and DP_ERR_ASYNC_CTS_ON */
void dp_enable_data_mode(Uint8 mode, Uint8 option, Uint8 speed);
/* unhook dp_interrupt, and turn the pump's interrupts off. where format's
 * unk_bit4 is set, clear it in any configuration. the call drops rts, waits
 * for cts to drop, clears the bit and restores rts. where rts and cts were both
 * on, then wait for cts to return. a wait that passes 500 ms reports
 * DP_ERR_DISABLE_CTS_OFF or DP_ERR_DISABLE_CTS_ON, and the call returns */
void dp_disable_data_mode(void);
/* turn the transmit interrupt on. while rcflags' in_interrupt is set, set its
 * saved_tx_irq instead, which dp_interrupt restores as it returns */
void dp_enable_tx_irqs(void);
/* turn the transmit interrupt off, as dp_enable_tx_irqs turns it on */
void dp_disable_tx_irqs(void);
/* return 1 while rcflags' unk_1_0, tx_irq_deferred or unk_3_6 is set.
 * otherwise return whether the transmit interrupt that dp_enable_tx_irqs sets
 * is on */
char dp_tx_irqs_enabled(void);
/* store a received byte in *byte and return 1, or return 0 for none */
char dp_get_rx_data_byte(Uint8 *byte);
/* return 1 while the pump takes a byte to send, and 0 otherwise. return 0
 * while rcflags' rx_only is set */
char dp_ready_to_tx_data(void);
void dp_tx_data_byte(Uint8 byte);
/* do nothing */
void dp_fill_tx_fifo(void);
/* return how long the transmit FIFO takes to empty at the connection's speed,
 * in counts of 10 ms */
Uint32 dp_get_tx_fifo_empty_time(void);
/* serve the pump's interrupt through dce_interrupt while the pump has a byte
 * or a status to serve. after 51 passes, report DP_ERR_INTERRUPT and return
 * with the pump's interrupts off, and rcflags' in_interrupt still set */
void dp_interrupt(void);

/* turn data mode off, and start the clear down that ends a V.34, K56flex,
 * V.90 or V.32 connection. another modulation takes none */
void dp_start_sending_clear_down(void);
/* return 0 while the clear down runs, for up to 2 seconds, or 1 second for
 * V.32. return 1 otherwise */
char dp_is_clear_down_sent(void);

/* return the word at address of the pump's RAM */
Uint32 dp_read_ram_location(Uint16 address);
/* return the pump RAM's version, as DspDbGetRamFlexPcmVersion does */
Uint16 dp_report_fw(void);
/* store the modulations the pump runs in *mask, and their highest receive
 * rate in *rate */
void dp_get_avail_mod_type(Uint32 *mask, Uint8 *rate);
/* send the test signal of mode, at level as dp_set_tx_level takes it. modes 0
 * to 15 dial a DTMF digit. guard picks the guard tone of modes 23 and 24, as
 * dp_set_guard_tone takes it */
void dp_transmit_PTT(Uint8 mode, Uint8 guard, Uint8 level);
/* give the speaker level 3 for a volume of 1, 1 for 2, and 2 for 3. any other
 * volume gives level 0 */
void dp_set_volume(Uint8 volume);
/* the speed codes the connection sends and receives at, which
 * ConvertTCISpeedToBps converts to bits per second */
Uint8 dp_get_tx_bit_rate(void);
Uint8 dp_get_rx_bit_rate(void);

#endif
