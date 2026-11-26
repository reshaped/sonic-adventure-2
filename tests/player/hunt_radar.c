/* the GCC game's radar over the hunt's own sound and draw: every piece not
 * taken beeps, speaks and shows its gauge, by the distance to it.
 * tests/test_hunt_radar.py takes the functions from src/player/knuckles.c */
#include <assert.h>
#include <math.h>
#include <string.h>

#include <ninja.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "draw/color.h"
#include "player/player.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/hunt.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

struct game_state game_state;
struct hunt *hunt_current;
struct player *player_records[PLAYER_SLOTS];
struct task_work *player_work[PLAYER_SLOTS];
int camera_current_index;
static NJS_TEXTURE_VTX radar_quads[8];

#include "hunt_radar.inc"

#define DRAWS 16

static struct hunt hunt;
static struct task_work works[GAME_PLAYERS];
static struct player records[GAME_PLAYERS];
static int mode = STAGE_PLAY;
static int beeps;
static int voices[GAME_PLAYERS];
static int texture;
/* each quad drawn: its texture and its left edge */
static struct {
    int texture;
    Float left;
} draws[DRAWS];
static int draw_count;

int stage_get_mode(void)
{
    return mode;
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    assert(id == SOUND_BEEP && key == NULL && priority == 0 && volume == 0);
    beeps++;
    return 0;
}

void voice_play_priority_2(int id)
{
    assert(id == VOICE_NEAR_KNUCKLES || id == VOICE_NEAR_ROUGE);
    voices[id == VOICE_NEAR_ROUGE]++;
}

Float njDistanceP2P(NJS_POINT3 *p1, NJS_POINT3 *p2)
{
    Float x = p1->x - p2->x;
    Float y = p1->y - p2->y;
    Float z = p1->z - p2->z;

    return (Float)sqrt((x * x) + (y * y) + (z * z));
}

void njColorBlendingMode(Int target, Int blend)
{
}

Sint32 njSetTexture(NJS_TEXLIST *texlist)
{
    return 1;
}

Sint32 njSetTextureNum(Uint32 n)
{
    texture = (int)n;
    return 1;
}

Sint32 njGetTextureNumG(void)
{
    return texture;
}

void njDrawTexture(NJS_TEXTURE_VTX *polygon, Int count, Int tex, Int flag)
{
    assert(count == 4 && tex == texture && draw_count < DRAWS);
    draws[draw_count].texture = texture;
    draws[draw_count].left = polygon[0].x;
    draw_count++;
}

/* put the three pieces along x at these distances from the origin */
static void place(Float c, Float b, Float a)
{
    int i;

    memset(&hunt, 0, sizeof(hunt));
    for (i = 0; i < HUNT_PIECES; i++) {
        hunt.pieces[i].kind = EMERALD_KIND_B_NORMAL;
        hunt.pieces[i].id = (Sint8)i;
    }
    hunt.pieces[HUNT_SLOT_C].pos.x = c;
    hunt.pieces[HUNT_SLOT_B].pos.x = b;
    hunt.pieces[HUNT_SLOT_A].pos.x = a;
}

/* the beeps of steps 0 to count - 1 */
static int beeps_over(int count)
{
    beeps = 0;
    for (game_state.steps = 0; game_state.steps < count; game_state.steps++) {
        hunt_sound_radar(&hunt);
    }
    return beeps;
}

static void take(int slot, int player)
{
    hunt.pieces[slot].kind = HUNT_PIECE_TAKEN;
    hunt.pieces[slot].id = (Sint8)player;
}

static void draw(void)
{
    draw_count = 0;
    hunt_draw(NULL);
}

static void sound(void)
{
    /* the beep goes by the nearest piece, which SEGA's radar leaves dim
     * while C is not taken */
    place(600.0F, 200.0F, 50.0F);
    assert(beeps_over(64) == 64 / BEEP_NEAR_STEPS);
    /* the player near A has said its voice once, for A */
    assert(voices[0] == 1 && voices[1] == 0);
    assert(hunt.voices == HEARD_PIECE(0, HUNT_SLOT_A));
    take(HUNT_SLOT_A, 0);
    assert(beeps_over(64) == 64 / BEEP_MIDDLE_STEPS && voices[0] == 1);
    take(HUNT_SLOT_B, 0);
    assert(beeps_over(64) == 0);
    hunt.pieces[HUNT_SLOT_C].pos.x = 400.0F;
    assert(beeps_over(64) == 64 / BEEP_FAR_STEPS);

    /* a one-player game neither beeps nor speaks outside play */
    place(600.0F, 200.0F, 50.0F);
    voices[0] = 0;
    mode = STAGE_FADE_IN;
    assert(beeps_over(64) == 0 && voices[0] == 0 && hunt.voices == 0);
    mode = STAGE_PLAY;
    game_state.mini_event_running = 1;
    assert(beeps_over(64) == 0 && voices[0] == 0);
    game_state.mini_event_running = 0;
    game_state.cleared = 1;
    assert(beeps_over(64) == 0 && voices[0] == 0);
    game_state.cleared = 0;

    /* each piece's voice is said once, as the player comes near it */
    assert(beeps_over(16) == 1 && voices[0] == 1);
    works[0].pos.x = 200.0F;
    assert(beeps_over(16) == 1 && voices[0] == 2);
    assert(hunt.voices ==
           (HEARD_PIECE(0, HUNT_SLOT_A) | HEARD_PIECE(0, HUNT_SLOT_B)));
    works[0].pos.x = 50.0F;
    assert(beeps_over(16) == 1 && voices[0] == 2);
    works[0].pos.x = 0.0F;

    /* a two-player game beeps by the nearer player, and each says its own
     * voice, outside play too */
    place(600.0F, 200.0F, 5000.0F);
    game_state.two_players = 1;
    works[1].pos.x = 150.0F;
    voices[0] = voices[1] = 0;
    assert(beeps_over(64) == 64 / BEEP_NEAR_STEPS);
    assert(voices[0] == 0 && voices[1] == 1);
    assert(hunt.voices == HEARD_PIECE(1, HUNT_SLOT_B));
    mode = STAGE_FADE_IN;
    hunt.voices = 0;
    assert(beeps_over(64) == 0 && voices[1] == 2);
    mode = STAGE_PLAY;
    game_state.two_players = 0;
    works[1].pos.x = 0.0F;
}

static void gauges(void)
{
    /* each piece shows the gauge its own distance gives: C out of range, B
     * near and A close. the two in range draw a ring first */
    place(600.0F, 200.0F, 50.0F);
    game_state.unk_0b = 1;
    draw();
    assert(draw_count == 5);
    assert(draws[0].texture == RADAR_FAR + HUNT_SLOT_C &&
           draws[0].left == GAUGE_LEFT);
    assert(draws[1].texture == TEX_FLASH + HUNT_SLOT_B);
    assert(draws[2].texture == RADAR_NEAR + HUNT_SLOT_B &&
           draws[2].left == GAUGE_LEFT + GAUGE_GAP);
    assert(draws[3].texture == TEX_FLASH + HUNT_SLOT_A);
    assert(draws[4].texture == RADAR_CLOSE + HUNT_SLOT_A &&
           draws[4].left == GAUGE_LEFT + (2 * GAUGE_GAP));
    /* a taken piece shows none, and the others keep their places */
    take(HUNT_SLOT_B, 0);
    hunt.pieces[HUNT_SLOT_C].pos.x = 400.0F;
    draw();
    assert(draw_count == 4);
    assert(draws[1].texture == RADAR_MIDDLE + HUNT_SLOT_C &&
           draws[1].left == GAUGE_LEFT);
    assert(draws[3].texture == RADAR_CLOSE + HUNT_SLOT_A &&
           draws[3].left == GAUGE_LEFT + (2 * GAUGE_GAP));

    /* a two-player game's screen shows its own player's gauges, each at
     * its piece's place */
    place(600.0F, 200.0F, 50.0F);
    game_state.two_players = 1;
    works[1].pos.x = 650.0F;
    camera_current_index = 1;
    draw();
    assert(draw_count == 5);
    assert(draws[1].texture == RADAR_CLOSE + HUNT_SLOT_C &&
           draws[1].left == GAUGE_LEFT_1);
    assert(draws[3].texture == RADAR_MIDDLE + HUNT_SLOT_B &&
           draws[3].left == GAUGE_LEFT_1 + GAUGE_GAP_SPLIT);
    assert(draws[4].texture == RADAR_FAR + HUNT_SLOT_A &&
           draws[4].left == GAUGE_LEFT_1 + (2 * GAUGE_GAP_SPLIT));
    camera_current_index = 0;
    draw();
    assert(draw_count == 5 && draws[0].texture == RADAR_FAR + HUNT_SLOT_C &&
           draws[0].left == GAUGE_LEFT_0);
    /* the other player's piece leaves the screen's player its two gauges.
     * its own two leave it none */
    take(HUNT_SLOT_C, 1);
    draw();
    assert(draw_count == 4 && draws[1].texture == RADAR_NEAR + HUNT_SLOT_B);
    take(HUNT_SLOT_B, 1);
    draw();
    assert(draw_count == 2 && draws[1].texture == RADAR_CLOSE + HUNT_SLOT_A);
    camera_current_index = 1;
    draw();
    assert(draw_count == 0);
    camera_current_index = 0;
    game_state.two_players = 0;
}

int main(void)
{
    hunt_current = &hunt;
    records[0].kind = PLAYER_KNUCKLES;
    records[1].kind = PLAYER_ROUGE;
    player_records[0] = &records[0];
    player_records[1] = &records[1];
    player_work[0] = &works[0];
    player_work[1] = &works[1];
    sound();
    gauges();
    return 0;
}
