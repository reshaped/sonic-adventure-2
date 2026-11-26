/* MDCONTWOOD, spin_breakable_impl.h's crate with the wooden box's pieces. a
 * piece thrown without its build flies weightless */
#define IRON_CRATE 0
#define CRATE(name) spin_wood_##name
#define PIECE(name) wood_box_##name
#define BREAK_SOUND 0x1010
#define RAW_PIECE_FLAGS DEBRIS_WEIGHTLESS
#include "shared/spin_breakable_impl.h" /* IWYU pragma: keep -- the code it compiles here */
