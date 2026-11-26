/* the items' icons: the icon over an item box, and a row of icons on a
 * player's screen. the row shows the items the player has just got */
#ifndef SHARED_ITEM_ICON_H
#define SHARED_ITEM_ICON_H
#include <ninja.h>
#include <sg_xpt.h>

/* the icons' textures. texture k is the icon of the item kind k, an ITEM_
 * kind of shared/item_box.h. texture ITEM_1UP + c is the 1-up of the
 * character c, a PLAYER_ character up to PLAYER_EGGMAN_MECH. the list holds
 * no 1-up for PLAYER_SUPER_SONIC or PLAYER_SUPER_SHADOW */
extern NJS_TEXLIST item_icon_texlist;

/* draw the icon of kind as a square 1 on a side. its center is pos, in the
 * current matrix's space. ang turns the square about y, and scale scales it.
 * ITEM_1UP draws the 1-up of the character in player slot
 * camera_current_index, which item_icon_texlist must hold, unchecked. while
 * that slot is empty, it draws PLAYER_SONIC's.
 *
 * it returns 1, or 0 for a kind above ITEM_1UP, which draws nothing. kind
 * must not be below 0 on the matching path. The port also returns 0 when
 * the chosen slot is absent or unloaded, before changing blending or the
 * current texture. A successful draw leaves item_icon_texlist selected and
 * source-alpha-over-inverse-source-alpha blending enabled. */
int item_icon_draw(int kind, NJS_POINT3 *pos, Angle ang, Float scale);
/* add the icon of kind to the row on player's screen. the call creates the
 * row where none runs yet. it does nothing for ITEM_EMPTY, or for a player
 * other than 0 and 1. nor does it while game_state.unk_0b is 0. the heap must
 * have room for the row's task and record, unchecked.
 *
 * the row's icons slide into place, hold and shrink away, and the row then
 * ends. an icon added before the end starts the slide again. the row holds 20
 * icons, and a 21st icon writes past the row's record, unchecked. a kind
 * above ITEM_1UP takes a place in the row, but draws nothing. kind must not
 * be below 0, unchecked */
void item_icon_row_add(int player, int kind);

#endif
