#include <assert.h>
#include <stddef.h>

#include "roc_k56.h"

int main(void)
{
    struct dp_registers pump = {0};
    struct rc_flags line = {0};
    struct k56_flags training = {0};
    const Uint8 *line_bytes = (const Uint8 *)&line;
    const Uint8 *training_bytes = (const Uint8 *)&training;

    assert(sizeof(pump) == 32 * DP_REG_STRIDE);
    assert(offsetof(struct dp_registers, ram_control) == 0x1d * DP_REG_STRIDE);

    pump.volume.bit.level = 3;
    assert(pump.volume.byte == 0xc0);
    pump.r0d.byte = 0xfd;
    assert(!pump.r0d.bit.tx_room);
    pump.r0d.bit.tx_room = 1;
    assert(pump.r0d.byte == 0xff);
    pump.r15.bit.rate_range = 1;
    assert(pump.r15.byte == 1);
    pump.ram_control.bit.address = 15;
    pump.ram_control.bit.busy = 1;
    assert(pump.ram_control.byte == 0x8f);

    pump.speed.byte = 0xb6;
    assert(pump.speed.bit.retrain && pump.speed.bit.renegotiation);
    assert(pump.speed.bit.rate == 0x16 && pump.speed.old.rate == 6);

    line.saved_tx_irq = 1;
    line.saved_status_irq = 1;
    assert(sizeof(line) == 5 && line_bytes[2] == 5);
    training.measure_eqm = 1;
    training.v34_stall_command_sent = 1;
    assert(sizeof(training) == 5);
    assert(training_bytes[2] == 0x10 && training_bytes[4] == 0x40);
    return 0;
}
