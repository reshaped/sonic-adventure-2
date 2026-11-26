#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ecdc_def.h"
#include "modemdef.h"
#include "tci_modm.h"
#include "util_mod.h"

void force_o_cmd(void)
{
    /* A recognized escape command never forces the modem back online. */
    abort();
}

int main(void)
{
    const char *input = "at&c1\r";
    const char expected[] = "AT&C1\r";

    S3_THEN = '\r';
    SREG(5) = '\b';
    BMR(6) = BMR6_COMMAND;
    restart_TIES();
    while (*input) {
        char accepted = TIES_validation((Uint8)*input++);
        assert(accepted == 1);
    }

    assert(ties_phase == 0);
    assert(BMR(21) & 0x20);
    assert(CMD_B_PT == sizeof(expected) - 1);
    assert(memcmp(CMD_BUF, expected, CMD_B_PT) == 0);
    return 0;
}
