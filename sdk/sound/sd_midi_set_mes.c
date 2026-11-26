#include <sd_work.h>
#include <sg_sd.h>
#include <sg_xpt.h>
#include <stdarg.h>

/* a status byte has its top bit set. its high four bits are the message's
 * kind, and its low four the channel. a data byte's top bit is clear */
#define STATUS_BIT 0x80
#define STATUS_KIND 0xf0
#define STATUS_CHANNEL 0x0f
#define STATUS_KIND_SHIFT 4
/* the kind of a system message, which names no channel */
#define KIND_SYSTEM 0xf0
/* a kind's number is bits 4 to 6 of its status byte. this masks them once
 * the byte is shifted down by STATUS_KIND_SHIFT */
#define KIND_NUMBER_BITS 0x07
/* the shift that puts the port's number above the channel's four bits in
 * m_Member[2] */
#define PORT_SHIFT 4

/* a command for the message. sdMidiSetMes only packs the message, and
 * sdMidiSendMes sends it */
static struct sd_host_cmd midi_set_mes_cmd;

SDE_ERR sdMidiSetMes(SDMIDI handle, SDS_MIDI_MES *midi_mes_ptr,
                     const Uint8 midi_mes, ...)
{
    SDE_ERR result = SDE_ERR_NOTHING;
    va_list args;
    Sint8 port;
    Sint8 data1;
    Sint8 data2;

    if (sdgWrkPtr == NULL) {
        result = SDE_ERR_NO_INIT;
        goto exit;
    }
    if (handle == NULL) {
        result = SDE_ERR_HANDLE_NULL;
        goto exit;
    }
    /* @hack spelling: port, a local for the port's number, where ((struct
     * sd_port_head *)handle)->port in the message's third byte would do */
    port = ((struct sd_port_head *)handle)->port;
    if ((midi_mes & STATUS_KIND) != KIND_SYSTEM) {
        /* standard C leaves va_start undefined after a parameter that
         * promotes, as the Uint8 midi_mes does. SHC passes midi_mes on the
         * stack in four bytes, as it does an int. so the ints that follow it
         * lie where va_start looks */
        va_start(args, midi_mes); /* NOLINT(clang-diagnostic-varargs) */
        data1 = va_arg(args, int);
        switch (midi_mes & STATUS_KIND) {
        case SDD_MIDI_MES_PRG_CHG:
        case SDD_MIDI_MES_MONO_PRES:
            data2 = 0;
            break;
        default:
            data2 = va_arg(args, int);
            break;
        }
        va_end(args);
        /* @hack spelling: three ifs, one for each of midi_mes, data1 and
         * data2, where one if over their tests joined by || would do */
        if ((midi_mes & STATUS_BIT) == 0) {
            result = SDE_ERR_PRM_OVER_RANGE;
            goto exit;
        }
        if ((data1 & STATUS_BIT) != 0) {
            result = SDE_ERR_PRM_OVER_RANGE;
            goto exit;
        }
        if ((data2 & STATUS_BIT) != 0) {
            result = SDE_ERR_PRM_OVER_RANGE;
            goto exit;
        }
        midi_mes_ptr->m_Member[0] = data2;
        midi_mes_ptr->m_Member[1] = data1;
        midi_mes_ptr->m_Member[2] =
            (port << PORT_SHIFT) | (midi_mes & STATUS_CHANNEL);
        midi_mes_ptr->m_Member[3] =
            (midi_mes >> STATUS_KIND_SHIFT) & KIND_NUMBER_BITS;
    } else {
        midi_mes_ptr->m_Member[0] = 0;
        midi_mes_ptr->m_Member[1] = 0;
        midi_mes_ptr->m_Member[2] = 0;
        midi_mes_ptr->m_Member[3] = 0;
    }
exit:
    if (result != SDE_ERR_NOTHING) {
        sdSysErrTrap((void *)sdMidiSetMes, result);
    }
    return result;
}
