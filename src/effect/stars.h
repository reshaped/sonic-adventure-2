/* the crash stars, which burst from a player, and the confu stars, which
 * circle a confused one */
#ifndef EFFECT_STARS_H
#define EFFECT_STARS_H

/* burst six crash stars from the player in slot player of player_work. each
 * is a task in level 3, yellow or orange at random. an empty slot bursts none,
 * and a star the heap can't give ends the burst. the slot's work must have a
 * collision record, unchecked.
 *
 * the stars start at the center of the record's first shape and fly 10 steps
 * along random headings. each star after the first has its heading turned by
 * Ninja's current matrix at the call */
void crash_stars_burst(int player);
/* start three confu stars circling the player in slot player of
 * player_records. each is a task in level 3, yellow or orange at random. a star
 * the heap can't give ends the start. a star ends once the slot is empty, or
 * once the record's confused_steps is 0 or below */
void confu_stars_start(int player);

#endif
