/* Exercise the transmit level's gain table bound and the configuration a
 * retrain or a renegotiation keeps, on the non-matching side. */
#include <assert.h>
#include "roc_k56.c"

static struct dp_registers pump;

/* a pump in fault, so each RAM access returns at once */
Uint8 target_HwFault(void)
{
    return 1;
}

void setup_timer(Uint32 *deadline, Uint32 ticks)
{
    *deadline = ticks;
}

/* a timer that expires at once, so newc's wait ends */
char timer_expired(Uint32 deadline)
{
    (void)deadline;
    return 1;
}

void data_pump_error(Uint32 code)
{
    (void)code;
}

void target_InstallDpIrq(void (*handler)(void))
{
    (void)handler;
}

void InitWaVariables(void)
{
}

void dce_interrupt(void)
{
    assert(0); /* No interrupt is deferred. */
}

/* level passes through a volatile, so no compiler can fold away the read
 * past tx_gains that dp_set_tx_level makes without its guard */
static void check_level(Uint8 level, Uint16 gain)
{
    volatile Uint8 chosen = level;
    dp_set_tx_level(chosen);
    assert(TxLevelGain == gain);
    assert(pump.tx_level.bit.attenuation == MOST_ATTENUATION);
}

/* a level past 43 takes the last gain, 25 dB's */
static void check_levels(void)
{
    check_level(LEVEL_OFFSET + MOST_ATTENUATION + 24, 0x0409);
    check_level(LEVEL_OFFSET + MOST_ATTENUATION + 25, 0x0399);
    check_level(LEVEL_OFFSET + MOST_ATTENUATION + 26, 0x0399);
    check_level(255, 0x0399);
}

/* a K56flex connection's retrain and renegotiation keep its configuration,
 * whose class select_conf has no case for */
static void check_k56_configuration(void)
{
    Uint8 rate;
    for (rate = 1; rate < 16; rate++) {
        pump.conf.byte = CONF_K56 | rate;
        dp_init_retrain(RATE_2400, RATE_2400, 0, 0);
        assert(pump.conf.byte == (CONF_K56 | rate));
        dp_init_rate_renegotiation(0, rate);
        assert(pump.conf.byte == (CONF_K56 | rate));
    }
}

int main(void)
{
    rcdpi = &pump;
    check_levels();
    check_k56_configuration();
    return 0;
}
