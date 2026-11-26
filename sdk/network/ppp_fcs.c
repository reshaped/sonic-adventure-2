#include <sg_xpt.h>
#include "ppp_framing.h"

/* the PPP frame check's table: CRC-16/X-25's entry for each byte. the CRC is
 * linear over XOR. so a byte's entry is its high nibble's entry XOR its low
 * nibble's. FCS_ROW's argument is the high nibble's, and its constants the low
 * nibbles' */
#define FCS_ROW(high)                                                          \
    (high), (high) ^ 0x1189, (high) ^ 0x2312, (high) ^ 0x329b,                 \
        (high) ^ 0x4624, (high) ^ 0x57ad, (high) ^ 0x6536, (high) ^ 0x74bf,    \
        (high) ^ 0x8c48, (high) ^ 0x9dc1, (high) ^ 0xaf5a, (high) ^ 0xbed3,    \
        (high) ^ 0xca6c, (high) ^ 0xdbe5, (high) ^ 0xe97e, (high) ^ 0xf8f7
const Uint16 PP_fcstab[256] = {
    FCS_ROW(0x0000), FCS_ROW(0x1081), FCS_ROW(0x2102), FCS_ROW(0x3183),
    FCS_ROW(0x4204), FCS_ROW(0x5285), FCS_ROW(0x6306), FCS_ROW(0x7387),
    FCS_ROW(0x8408), FCS_ROW(0x9489), FCS_ROW(0xa50a), FCS_ROW(0xb58b),
    FCS_ROW(0xc60c), FCS_ROW(0xd68d), FCS_ROW(0xe70e), FCS_ROW(0xf78f)};
