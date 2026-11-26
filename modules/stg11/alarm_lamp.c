/* the alarm lamp of stg11/alarm_lamp.h: shared/alarm_lamp.c, compiled again
 * under Iron Gate's names. its head draws with Ninja's fog off */
#include "stg11/alarm_lamp.h"

/* NOLINTBEGIN(readability-identifier-naming) */
#define alarm_lamp_init iron_gate_alarm_lamp_init
#define alarm_lamp_texlists iron_gate_alarm_lamp_texlists
/* NOLINTEND(readability-identifier-naming) */
#define ALARM_LAMP_BEFORE_HEAD() njFogDisable()
#define ALARM_LAMP_AFTER_HEAD() njFogEnable()

#include "shared/alarm_lamp.c" /* NOLINT(bugprone-suspicious-include) */
