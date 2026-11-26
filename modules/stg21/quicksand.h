/* Sand Ocean's land blocks, and the quicksand drawn over them */
#ifndef STG21_QUICKSAND_H
#define STG21_QUICKSAND_H

/* the land blocks, a bit each of a land_block_mask. each step, sand_ocean.c's
 * BgClip sets camera 0's mask to the one block that holds the current
 * camera */
enum ocean_block {
    OCEAN_BLOCK_1 = 0x02,
    OCEAN_BLOCK_2 = 0x04,
    OCEAN_BLOCK_3 = 0x08,
    OCEAN_BLOCK_4 = 0x10,
    OCEAN_BLOCK_5 = 0x20,
    OCEAN_BLOCK_6 = 0x40,
    OCEAN_BLOCK_7 = 0x80,
    OCEAN_BLOCK_8 = 0x100
};

/* draw the pieces of quicksand seen from the block in camera 0's
 * land_block_mask. their textures and UVs move on every two of
 * game_state.steps */
void ocean_quicksand_draw(void);

#endif
