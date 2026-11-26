/* IRON3, spin_breakable_impl.h's crate with the iron box's pieces. it breaks
 * only for a character with the upgrade that box_breaks names */
#define IRON_CRATE 1
#define CRATE(name) spin_iron_##name
#define PIECE(name) iron_box_##name
#define BREAK_SOUND 0x1011
#define RAW_PIECE_FLAGS 0
#include "shared/spin_breakable_impl.h" /* IWYU pragma: keep -- the code it compiles here */
