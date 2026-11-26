#include "ask_rom.h"
#include <sg_xpt.h>

const Uint8 ask_dictionary_rom[ASK_DICTIONARY_BYTES] = {
#include "network/ask_rom_data/ask_dictionary_rom.inc"
};
