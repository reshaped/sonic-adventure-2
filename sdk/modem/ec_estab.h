/* ec_estab: V.42's detection phase, which leads to LAPM or falls back to a
 * link without error control. a fallback can give the terminal what the line
 * sent during the phase */
#ifndef MODEM_EC_ESTAB_H
#define MODEM_EC_ESTAB_H
#include <sg_xpt.h>

/* the phases ec_estab_phase holds. INITIALIZE starts the detection phase.
 * nothing sets WAIT_FALLBACK or DRAIN_LINE, and only DRAIN_LINE leads to
 * FINISH_LINK */
enum establishment_phase {
    INITIALIZE,
    WAIT_TO_ORIGINATE,
    DETECT_PATTERN,
    REPEAT_ANSWER_PATTERN,
    SEND_FLAGS,
    NEGOTIATE_LAPM,
    START_FALLBACK,
    WAIT_FALLBACK,
    DRAIN_LINE,
    FINISH_LINK
};
/* the phase ec_estab runs in */
extern Uint8 ec_estab_phase;
extern Uint8 char_detected, detect_cnt, previous_char;
extern Uint8 mdp_sent, v42_call_no_ODP;
/* the TIM_10MS reading from which a fallback keeps the line's bytes for the
 * terminal. line_estab sets it as the carrier comes */
extern Uint32 invalid_data_timer;
extern Uint8 previous_ois_28;
/* take the detection phase's next step. scm_main_core calls it in
 * ESTABLISH_PHASE */
void ec_estab(void);
#endif
