/* ASK's dictionary file, which the FAskRom_ functions read, and its build
 * string, which ntgDicBuild points at */
#ifndef ASK_ROM_H
#define ASK_ROM_H
#include <sg_xpt.h>
#define ASK_DICTIONARY_BYTES (332UL * 1024)
extern const Uint8 ask_dictionary_rom[ASK_DICTIONARY_BYTES];
extern char ask_dictionary_build_text[];
#endif
