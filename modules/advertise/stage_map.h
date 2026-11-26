/* the stage map, the task stageMapExec, which menus_init creates. it draws
 * a grid of the stages over a backdrop, as stage_map_work asks. panels
 * beside the grid show the records of the cursor's stage */
#ifndef ADVERTISE_STAGE_MAP_H
#define ADVERTISE_STAGE_MAP_H
#include <sg_xpt.h>

#include "advertise/menus.h"

/* the grid's cells, and the pages the grid scrolls to */
#define STAGE_MAP_CELLS 33
#define STAGE_MAP_PAGES 4
/* a cell of the grid: its stage, its character, and its column and row.
 * the cell shows the character's icon, but Green Hill's cell and Chao
 * World's show icons of their own */
struct stage_map_cell {
    Sint32 stage;
    Sint32 character;
    Sint32 column;
    Sint32 row;
};
extern struct stage_map_cell stage_map_cells[STAGE_MAP_CELLS];

/* the work, menu_works.work_02 */
extern struct stage_map_work stage_map_work;

/* the directions stage_map_cursor_move takes */
#define STAGE_MAP_UP 0
#define STAGE_MAP_DOWN 1
#define STAGE_MAP_LEFT 2
#define STAGE_MAP_RIGHT 3

/* what a mission of the cursor's cell plays: a stage, the kart race or
 * Chao World. STAGE_MAP_LOCKED is none */
#define STAGE_MAP_LOCKED 0
#define STAGE_MAP_STAGE 1
#define STAGE_MAP_KART 2
#define STAGE_MAP_CHAO 3

/* create the task, stageMapExec, and clear the whole work, its loaded flag
 * too. the map starts hidden, its grid at page 0. the task's destructor
 * releases the textures. the relay's cell shows save_data's records of
 * STAGE_RELAY_SONIC. create copies each mission's play count from
 * STAGE_RELAY_FIRST's records to them */
void stage_map_create(void);
/* clear the work but its loaded flag, and hide the map at once. the grid
 * goes back to page 0, and no cell is lit */
void stage_map_reset(void);
/* load the textures, the PVMs stageMap and stageMapBG, where the work says
 * they aren't loaded */
void stage_map_load(void);
/* release the textures, where the work says they are loaded */
void stage_map_release(void);
/* move the cursor to the nearest lit cell in direction, a STAGE_MAP_
 * direction. up and down keep to the cursor's column. left and right go to
 * the nearest column with a lit cell. in it, they take the row nearest the
 * cursor's. of two rows as near, they take the cell first in
 * stage_map_cells. the move may turn the work's page to show the cell.
 *
 * it returns TRUE where the cursor moved, and FALSE where no lit cell lies
 * that way */
Bool stage_map_cursor_move(Int direction);
/* ready a one-player game of the cursor's cell, and set
 * game_state.next_mode to GAME_MODE_STAGE. it sets the current stage and
 * player 0's character to the cell's. the relay's cell plays
 * STAGE_RELAY_FIRST as Tails's mech. it sets game_state.mission to the
 * work's mission, or to MISSION_FIRST for Green Hill. in Chao World, the
 * work's mission is the choice of character. a mech's choice plays as its
 * pilot, and game_state.mission stays as it was.
 *
 * it returns STAGE_MAP_STAGE for a stage or the kart race, and
 * STAGE_MAP_CHAO for Chao World. where the character chosen for Chao World
 * hasn't visited it, it readies nothing and returns STAGE_MAP_LOCKED */
Int stage_map_start(void);
/* what the cursor's cell plays with the work's mission, as a STAGE_MAP_
 * value. it is STAGE_MAP_LOCKED where save_data hasn't opened the mission.
 * a stage's first mission is open, and each later one once the one before
 * has a rank. Chao World's choice is open once its character has visited */
Int stage_map_mission_open(void);
/* TRUE where cell is lit, as the task's last step while the map showed
 * found it. a cell lights once save_data has a rank in its stage's first
 * mission. Chao World's lights once a character has visited it */
Bool stage_map_cell_lit(Int cell);

#endif
