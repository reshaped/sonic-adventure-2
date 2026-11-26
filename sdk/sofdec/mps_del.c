/* the parser's start code check (mps.h) */
#include <mps.h>
#include <sg_xpt.h>

Sint32 MPSDEL_CheckDelim(const Uint8 *data)
{
    const Uint8 *cursor;
    Uint32 code;
    Sint32 kind;

    cursor = data;
    code = *cursor++;
    code <<= 8;
    code |= *cursor++;
    code <<= 8;
    code |= *cursor++;
    code <<= 8;
    code |= *cursor;
    switch (code) {
    case MPS_CODE_PACK:
        kind = MPS_PACK;
        break;
    case MPS_CODE_SYSTEM:
        kind = MPS_SYSTEM;
        break;
    case MPS_CODE_END:
        kind = MPS_END;
        break;
    case MPS_CODE_PRIVATE1:
    case MPS_CODE_PRIVATE2:
    case MPS_CODE_PADDING:
        kind = MPS_PACKET;
        break;
    default:
        if ((Sint32)code >= MPS_CODE_AUDIO_FIRST &&
            (Sint32)code <= MPS_CODE_VIDEO_LAST) {
            kind = MPS_PACKET;
        } else {
            kind = MPS_NONE;
        }
        break;
    }
    return kind;
}
