/* the state of the modem's error control and compression layer, which
 * ecdc_def.h declares */
#include <sg_xpt.h>
#include "ecdc_def.h"

Uint8 BMRregisters[33];
Uint8 OISregisters[29];
Uint8 scm_main_phase;
Uint16 REQ_LINK;
Uint8 STATUS_BITS;
Uint32 TIM_10MS;
Uint16 DTE_INAC_TIM;
Uint16 DCE_INAC_TIM;
Uint8 fhud_failed;
Uint32 fhud_timer;
Uint32 data_pass_timer;
Uint8 prsc_1;
Uint8 Last_BMR27;
Uint8 Last_BMR29;
Uint8 cmd_parameter_buffer[30];
Uint8 cmd_response_buffer[8];
Uint8 last_response;
Uint8 ecdc_self_test;
Uint16 last_nb_out;
Uint16 n_of_bytes_in;
Uint16 n_of_bytes_out;
struct EcdcStatistics StatisticInfo;
struct EcdcStatus StatusInfo;
