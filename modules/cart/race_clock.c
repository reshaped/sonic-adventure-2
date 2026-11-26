/* the race's clock: its time, each racer's gate, lap and place, and the
 * race's end. no code calls cart_clock_stop or cart_tunnel_sound */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/probe.h"
#include "cart/course.h"
#include "cart/pads.h"
#include "cart/race.h"
#include "cart/track.h"
#include "cart/vehicle.h"
#include "hud/hud.h"
#include "save/save.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/game_state.h"
#include "system/task.h"

/* the race's time counts up to MOST_FRAMES steps */
#define SECONDS_PER_MINUTE 60
#define STEPS_PER_MINUTE (SECONDS_PER_MINUTE * CART_STEPS_PER_SECOND)
#define HUNDREDTHS_PER_SECOND 100
#define HUNDREDTHS_PER_MINUTE (SECONDS_PER_MINUTE * HUNDREDTHS_PER_SECOND)
#define MOST_FRAMES 0x10000000
/* the countdown starts at a minute. only a story race reads it */
#define COUNTDOWN_STEPS STEPS_PER_MINUTE
/* once the race is over, result_steps counts its steps. a finished race
 * records its time at RECORD_STEP, and waits at HOLD_STEP while a versus
 * result is open. at RESULT_STEP it ends the stage, except under
 * MISSION_RINGS. a cleared story race shows its results at RESULT_STEP */
#define RECORD_STEP 60
#define HOLD_STEP 200
#define RESULT_STEP 300
/* a racer's score ranks it. a lap scores LAP_SCORE more than the one before,
 * and lap -LAP_BIAS scores 0. a gate scores GATE_SCORE, and the way to the
 * next gate up to FULL_PROGRESS */
#define LAP_SCORE 1000000
#define LAP_BIAS 10
#define GATE_SCORE 1000
#define FULL_PROGRESS 100.0F
/* crossed_gate's triangle stands on the gate's line at GATE_LOW, and runs
 * GATE_REACH gate lengths past each end. its apex stands at GATE_HIGH over the
 * gate's point 2 */
#define GATE_REACH 3
#define GATE_LOW (-20)
#define GATE_HIGH 50
/* draw_gate's colors for the next two gates, the kart's gate, and the two
 * before it. each quad stands from DRAWN_DROP below the kart, DRAWN_HEIGHT
 * high. nothing sets draw_gates, so no gate is drawn */
#define AHEAD_COLOR 0x8000ff00
#define FAR_AHEAD_COLOR 0x800000ff
#define GATE_COLOR 0x84400044
#define BEHIND_COLOR 0x80ff0000
#define FAR_BEHIND_COLOR 0x80008080
#define DRAWN_DROP 20
#define DRAWN_HEIGHT 50
/* the MIDI ports' effect levels in a tunnel and out of one */
#define TUNNEL_FX_LEVEL 0
#define OPEN_FX_LEVEL (-127)
/* how long a time over waits to end the stage, and each story race's voice
 * for it */
#define TIME_OVER_STEPS 180
#define TAILS_TIME_OVER_VOICE 0x706
#define ROUGE_TIME_OVER_VOICE 0x70d

static Bool stopping = 1;
static Bool draw_gates = 0;
Bool cart_clock_held = 1;
Bool cart_finished = 0;
struct cart_gate_point cart_gates[CART_GATES][CART_GATE_POINTS];
Sint32 cart_gate_sections[CART_GATES];
Sint32 cart_gate_count;
static Sint32 finished_steps, result_steps;
static void clock_step(struct task *task);
static void clock_draw(struct task *task);
static void clock_destroy(struct task *task);
static Bool crossed_gate(struct task *task, int gate);
static void draw_gate(Uint32 color, Float height, int gate);
static void rank_by_progress(const Float *values, int count, int *order);

#ifdef NONMATCHING
/* a download course has a best time in one of the save's slots. any other
 * course has one where it is among the SAVE_KART_RECORDS courses */
Bool cart_time_record_available(void)
{
    if (cart_setup.course_mode == CART_DOWNLOAD) {
        return (Uint32)download_chosen.slot < DOWNLOAD_SLOTS;
    }
    return SAVE_KART_COURSE_VALID(game_state.kart_course);
}
#endif

void cart_clock_start(void)
{
    struct task *task;
    task = task_create(TASK_WORK, 2, clock_step, "startGoalExec");
    task->draw = clock_draw;
    task->destructor = clock_destroy;
    stopping = 0;
    /* @hack redundant: a first clear of finished_steps, which the call
     * repeats at its end */
    finished_steps = 0;
    cart_story_clear = 0;
    cart_race.frames = 0;
    cart_race.time.minute = 0;
    cart_race.time.second = 0;
    cart_race.time.hundredths = 0;
    cart_race.countdown = COUNTDOWN_STEPS;
    cart_race.player[0].lap[0].minute = CART_NO_TIME;
    cart_race.player[0].lap[1].minute = CART_NO_TIME;
    cart_race.player[0].lap[2].minute = CART_NO_TIME;
    cart_race.player[1].lap[0].minute = CART_NO_TIME;
    cart_race.player[1].lap[1].minute = CART_NO_TIME;
    cart_race.player[1].lap[2].minute = CART_NO_TIME;
    stage_clock_restore();
    finished_steps = 0;
    result_steps = 0;
}
void cart_clock_stop(void)
{
    stopping = 1;
}

#define HUNDREDTHS(time)                                                       \
    (((time).minute * HUNDREDTHS_PER_MINUTE) +                                 \
     ((time).second * HUNDREDTHS_PER_SECOND) + (time).hundredths)
static void lap_difference(struct save_time *out, struct save_time *end,
                           struct save_time *start)
{
    int elapsed;
    elapsed = HUNDREDTHS(*end) - HUNDREDTHS(*start);
    out->minute = elapsed / HUNDREDTHS_PER_MINUTE;
    out->second = elapsed / HUNDREDTHS_PER_SECOND % SECONDS_PER_MINUTE;
    out->hundredths = elapsed % HUNDREDTHS_PER_SECOND;
}

#define DOWNLOAD_BEST save_data.download_best_times[download_chosen.slot]
#define LAPS(slot) cart_race.player[slot].lap

/* @bug logic: ADVANCE_GATES counts a lap where it takes gate past the last,
 * but leaves gate unwrapped (0x8c505e00). until the kart's next crossing
 * wraps gate and counts the lap again, clock_step's ranking reads cart_gates
 * by it, past the course's last gate. draw_lights and the tunnel tests read
 * cart_gate_sections by it too */
#ifdef NONMATCHING
/* this side repairs the bug tagged above: gate wraps as the lap counts */
#define WRAP_FORWARD(kart) ((kart)->gate %= cart_gate_count)
#else
#define WRAP_FORWARD(kart) ((void)0)
#endif
#define ADVANCE_GATES(kart, by)                                                \
    do {                                                                       \
        (kart)->gate += (by);                                                  \
        if ((kart)->gate >= cart_gate_count) {                                 \
            WRAP_FORWARD(kart);                                                \
            (kart)->lap++;                                                     \
        }                                                                      \
    } while (0)
#define BACK_GATES(kart, mode, by)                                             \
    do {                                                                       \
        (kart)->gate -= (by);                                                  \
        if ((kart)->gate < 0) {                                                \
            (kart)->gate = (kart)->gate + cart_gate_count;                     \
            (mode) = cart_setup.course_mode;                                   \
            if ((mode) == CART_TAILS || (mode) == CART_ROUGE) {                \
                (kart)->gate = 0;                                              \
            } else {                                                           \
                (kart)->lap--;                                                 \
            }                                                                  \
        }                                                                      \
    } while (0)

static void clock_step(struct task *task)
{
    Float score[CART_RACERS];
    int order[CART_RACERS];
    struct cart_vehicle *players[CART_PLAYERS];
    NJS_VECTOR distance[2];
    struct task_work *work;
    struct cart_vehicle *kart;
    int i;
    int count;
    int gate;
    int next;
    int best;
    int total;
    int mode;
    Float progress;
    struct cart_vehicle **second;

    for (i = 0; i < CART_PLAYERS; i++) {
        cart_race.player[i].task = cart_racers[i];
    }
    players[0] = cart_race.player[0].task->motion.p;
    if (game_state.two_players) {
        players[1] = cart_race.player[1].task->motion.p;
    }
    if (stopping) {
        /* @hack redundant: a test of task's work and motion under if (0) */
        if (0) {
            if (task->work && task->motion.p) {
            }
        }
        task_destroy(task);
        return;
    }
    if (cart_story_clear) {
        stage_clock_stop();
        result_steps++;
        if (result_steps == RESULT_STEP) {
            results_start(0);
        }
        cart_result_done = 1;
        game_state.pause_disabled = 1;
    } else if (cart_finished) {
        stage_clock_stop();
        result_steps++;
        if (result_steps == RESULT_STEP) {
            cart_result_done = 1;
            if (game_state.mission != MISSION_RINGS) {
                stage_end(STAGE_END_QUIT);
            }
        } else if (result_steps == HOLD_STEP) {
            if (cart_result_open) {
                result_steps--;
            }
        } else if (result_steps == RECORD_STEP) {
            if (cart_racers[0]) {
                struct cart_vehicle *first;
                first = cart_racers[0]->motion.p;
#ifdef NONMATCHING
                /* this side takes the plain test the hack tags name. it records
                 * a time only where cart_time_record_available finds the
                 * course's record */
                if (first->place == 1 && !game_state.two_players &&
                    cart_time_record_available()) {
#else
                /* @hack spelling: the test inverted, with an empty arm, where
                 * first->place == 1 && !two_players would do */
                /* @hack spelling: two_players read as game_state_two_players,
                 * its second name */
                if (first->place != 1 || game_state_two_players) {
                } else {
#endif
                    if (cart_setup.course_mode == CART_DOWNLOAD) {
                        if (game_state.kart_character_karts != 1) {
                            best = HUNDREDTHS(DOWNLOAD_BEST);
                            total = HUNDREDTHS(cart_race.time);
                            if (total < best) {
                                DOWNLOAD_BEST.minute = cart_race.time.minute;
                                DOWNLOAD_BEST.second = cart_race.time.second;
                                DOWNLOAD_BEST.hundredths =
                                    cart_race.time.hundredths;
                                if ((game_state.card_warning = save_check(0)) ==
                                    0) {
                                    save_write();
                                }
                            }
                        }
                    } else if (cart_setup.course_mode >= CART_BEGINNER) {
                        save_data.kart_records[game_state.kart_course].emblem =
                            1;
                        save_record_kart_time(0, cart_race.time.minute,
                                              cart_race.time.second,
                                              cart_race.time.hundredths);
                        if ((game_state.card_warning = save_check(0)) == 0) {
                            save_write();
                        }
                    }
                }
            }
        }
        game_state.pause_disabled = 1;
    } else if (!game_state.unk_09 || game_state.cleared) {
        cart_race.countdown--;
    }
    if (!cart_clock_held && !cart_finished) {
        cart_race.frames++;
        if (cart_race.frames > MOST_FRAMES) {
            cart_race.frames = MOST_FRAMES;
        }
        cart_race.time.minute = cart_race.frames / STEPS_PER_MINUTE;
        cart_race.time.second =
            cart_race.frames / CART_STEPS_PER_SECOND % SECONDS_PER_MINUTE;
        cart_race.time.hundredths = cart_race.frames % CART_STEPS_PER_SECOND *
                                    HUNDREDTHS_PER_SECOND /
                                    CART_STEPS_PER_SECOND;
        if (cart_setup.play_mode) {
            /* a story race's lap counts its checkpoints. the lap slot of the
             * checkpoints passed takes the race's whole time. lap never falls
             * below 0 there: BACK_GATES and the respawn take no lap from a
             * story race */
#ifdef NONMATCHING
            /* this side stores no time for a negative lap, which would write
             * before LAPS(0) */
            if (players[0]->lap >= 0 && players[0]->lap < CART_LAPS) {
#else
            if (players[0]->lap < CART_LAPS) {
#endif
                LAPS(0)[players[0]->lap] = cart_race.time;
            }
        } else {
            /* a kart race's lap slot takes the time since the laps before
             * it */
            if (players[0]->furthest_lap < CART_LAPS) {
                if (players[0]->furthest_lap) {
                    if (players[0]->furthest_lap == 2) {
                        lap_difference(&LAPS(0)[players[0]->furthest_lap],
                                       &cart_race.time, &LAPS(0)[0]);
                        lap_difference(&LAPS(0)[players[0]->furthest_lap],
                                       &LAPS(0)[players[0]->furthest_lap],
                                       &LAPS(0)[1]);
                    } else {
                        lap_difference(&LAPS(0)[players[0]->furthest_lap],
                                       &cart_race.time, &LAPS(0)[0]);
                    }
                } else {
                    LAPS(0)[players[0]->furthest_lap] = cart_race.time;
                }
            }
            if (game_state.two_players) {
#ifdef NONMATCHING
                /* this side reads the second kart again: players[1] holds it
                 * only where two_players held as the step began */
                players[1] = cart_race.player[1].task->motion.p;
#endif
                /* @hack spelling: second, a pointer to players[1], where
                 * players[1] itself would do */
                second = &players[1];
                /* @hack redundant: a test of second and *second under if (0) */
                if (0) {
                    if (second && *second) {
                    }
                }
                if ((*second)->furthest_lap < CART_LAPS) {
                    LAPS(1)[(*second)->furthest_lap] = cart_race.time;
                }
            }
        }
    }
    for (i = 0; i < CART_RACERS; i++) {
        if (!cart_racers[i]) {
            break;
        }
        work = cart_racers[i]->work;
        kart = cart_racers[i]->motion.p;
        gate = kart->gate;
        if (crossed_gate(cart_racers[i], (gate + 1) % cart_gate_count)) {
            kart->gate++;
            if (kart->gate >= cart_gate_count) {
                kart->gate = 0;
                kart->lap++;
            }
        } else if (crossed_gate(cart_racers[i], (gate + 2) % cart_gate_count)) {
            ADVANCE_GATES(kart, 2);
        } else if (crossed_gate(cart_racers[i], (gate + 3) % cart_gate_count)) {
            ADVANCE_GATES(kart, 3);
        } else if (kart->start.player_controlled) {
            if (draw_gates) {
                draw_gate(AHEAD_COLOR, work->pos.y,
                          (cart_gate_count + gate + 1) % cart_gate_count);
                draw_gate(FAR_AHEAD_COLOR, work->pos.y,
                          (cart_gate_count + gate + 2) % cart_gate_count);
                draw_gate(GATE_COLOR, work->pos.y,
                          (cart_gate_count + gate) % cart_gate_count);
                draw_gate(BEHIND_COLOR, work->pos.y,
                          (cart_gate_count + gate - 1) % cart_gate_count);
                draw_gate(FAR_BEHIND_COLOR, work->pos.y,
                          (cart_gate_count + gate - 2) % cart_gate_count);
            }
            if (crossed_gate(cart_racers[i],
                             (cart_gate_count + gate - 1) % cart_gate_count)) {
                BACK_GATES(kart, mode, 2);
            } else if (crossed_gate(cart_racers[i],
                                    (cart_gate_count + gate - 2) %
                                        cart_gate_count)) {
                BACK_GATES(kart, mode, 3);
            }
        }
        /* @hack redundant: a read of work->mode under if (0) */
        if (0) {
            (void)work->mode;
        }
        if (kart->furthest_lap < kart->lap) {
            kart->furthest_lap = kart->lap;
        }
    }
    if (cart_finished) {
        finished_steps++;
        /* @hack redundant: a test of finished_steps with an empty arm */
        if (finished_steps > RESULT_STEP) {
        }
        return;
    }
    count = finished_steps = 0;
    for (i = 0; i < CART_RACERS; i++) {
        if (!cart_racers[i]) {
            break;
        }
        count++;
        kart = cart_racers[i]->motion.p;
        work = cart_racers[i]->work;
        next = kart->gate + 1;
        if (next >= cart_gate_count) {
            next = 0;
        }
        distance[0].x = cart_gates[kart->gate][1].x - cart_gates[next][1].x;
        /* @hack redundant: a read of the next gate's x that nothing uses */
        (void)cart_gates[next][1].x;
        distance[0].y = 0.0F;
        distance[0].z = cart_gates[kart->gate][1].z - cart_gates[next][1].z;
        distance[1].x = work->pos.x - cart_gates[next][1].x;
        distance[1].y = 0.0F;
        distance[1].z = work->pos.z - cart_gates[next][1].z;
        progress = FULL_PROGRESS - (njScalor(&distance[1]) * FULL_PROGRESS /
                                    njScalor(&distance[0]));
        if (progress < 0.0F) {
            progress = 0.0F;
        }
        score[i] = ((kart->lap + LAP_BIAS) * LAP_SCORE) +
                   (kart->gate * GATE_SCORE) + progress;
    }
    /* @hack spelling: i for the count of racers, which it equals here, where
     * count would do */
    rank_by_progress(score, i, order);
    /* @hack redundant: a test of count under if (0) */
    if (0) {
        if (count) {
        }
    }
    for (i = 0; i < count; i++) {
        kart = cart_racers[order[i]]->motion.p;
        kart->place = count - i;
        if (kart->lap >= CART_LAPS) {
            if (game_state.two_players) {
                if (kart->start.player_controlled) {
                    cart_finished = 1;
                    cart_finish();
                    cart_show_versus_result();
                }
            } else {
                cart_finished = 1;
                cart_finish();
            }
        }
    }
}

static void clock_draw(struct task *task)
{
}
static void clock_destroy(struct task *task)
{
}

int cart_highest_lap(void)
{
    struct cart_vehicle *kart;
    int lap;
    int i;
    lap = -1;
    for (i = 0; i < CART_RACERS; i++) {
        /* @bug memory: reads each slot of cart_racers untested (0x8c5061f8),
         * where clock_step stops at the first null (0x8c505d5c). a call while
         * fewer than CART_RACERS karts race reads through null */
        kart = cart_racers[i]->motion.p;
        if (lap < kart->lap) {
            lap = kart->lap;
        }
    }
    return lap;
}

/* whether the path of task's kart over its last step, at y 0, crosses the
 * triangle of gate */
static Bool crossed_gate(struct task *task, int gate)
{
    /* @hack type: lines holds three lines, where the two it uses would do */
    NJS_LINE lines[3];
    NJS_POINT3 corners[3];
    NJS_POINT3 crossing;
    NJS_PLANE plane;
    struct cart_vehicle *kart;
    struct task_work *work;
    work = task->work;
    kart = task->motion.p;
    if (!kart) {
        return FALSE;
    }
    lines[0].px = cart_gates[gate][0].x;
    lines[0].py = 0;
    lines[0].pz = cart_gates[gate][0].z;
    lines[0].vx = cart_gates[gate][3].x - cart_gates[gate][0].x;
    lines[0].vy = 0;
    lines[0].vz = cart_gates[gate][3].z - cart_gates[gate][0].z;
    lines[1].px = work->pos.x;
    lines[1].py = 0;
    lines[1].pz = work->pos.z;
    lines[1].vx = kart->previous.pos.x - work->pos.x;
    lines[1].vy = 0;
    lines[1].vz = kart->previous.pos.z - work->pos.z;
    if (lines[1].vx == 0 && lines[1].vz == 0) {
        return FALSE;
    }
    corners[0].x = cart_gates[gate][0].x - (lines[0].vx * GATE_REACH);
    corners[0].y = GATE_LOW;
    corners[0].z = cart_gates[gate][0].z - (lines[0].vz * GATE_REACH);
    corners[1].x = cart_gates[gate][3].x + (lines[0].vx * GATE_REACH);
    corners[1].y = GATE_LOW;
    corners[1].z = cart_gates[gate][3].z + (lines[0].vz * GATE_REACH);
    corners[2].x = cart_gates[gate][2].x;
    corners[2].y = GATE_HIGH;
    corners[2].z = cart_gates[gate][2].z;
    plane.px = corners[0].x;
    plane.py = corners[0].y;
    plane.pz = corners[0].z;
    njGetPlaneNormal(corners, (NJS_VECTOR *)&plane.vx);
    njDistanceL2PL(&lines[1], &plane, &crossing);
    njUnitVector((NJS_VECTOR *)&plane.vx);
    return camera_probe_on_triangle(corners, &crossing, &lines[1]);
}
static void draw_gate(Uint32 color, Float height, int gate)
{
    NJS_POINT3COL polygon;
    NJS_POINT3 points[4];
    NJS_COLOR colors[4];
    int i;
    polygon.p = points;
    polygon.col = colors;
    polygon.tex = 0;
    polygon.num = 4;
    points[0].x = cart_gates[gate][0].x;
    points[0].y = height - DRAWN_DROP;
    points[0].z = cart_gates[gate][0].z;
    points[3].x = cart_gates[gate][3].x;
    points[3].y = height - DRAWN_DROP;
    points[3].z = cart_gates[gate][3].z;
    points[1] = points[0];
    points[2] = points[3];
    points[1].y += DRAWN_HEIGHT;
    points[2].y += DRAWN_HEIGHT;
    for (i = 0; i < 4; i++) {
        colors[i].color = color;
    }
    njDrawPolygon3D(&polygon, 4, NJD_FILL | NJD_TRANSPARENT);
}
static void rank_by_progress(const Float *values, int count, int *order)
{
#ifdef NONMATCHING
    /* this side sorts on the stack: count is at most CART_RACERS */
    Float sorted[CART_RACERS];
#else
    Float *sorted;
#endif
    Float score;
    int index;
    int gap;
    int i;
    int j;
#ifdef NONMATCHING
#else
    sorted = syMalloc(count * sizeof(*sorted));
#endif
    for (i = 0; i < count; i++) {
        order[i] = i;
        sorted[i] = values[i];
    }
    for (gap = count / 2; gap > 0; gap /= 2) {
        for (i = gap; i < count; i++) {
            for (j = i - gap; j >= 0 && sorted[j] > sorted[j + gap]; j -= gap) {
                score = sorted[j];
                index = order[j];
                sorted[j] = sorted[j + gap];
                order[j] = order[j + gap];
                sorted[j + gap] = score;
                order[j + gap] = index;
            }
        }
    }
#ifdef NONMATCHING
#else
    syFree(sorted);
#endif
}
Bool cart_leader_tunnel_sound(void)
{
    struct cart_vehicle *kart;
    int piece;
    kart = cart_racers[0]->motion.p;
    piece = cart_setup.pieces[cart_gate_sections[kart->gate]];
    if (piece >= CART_FIRST_TUNNEL && piece <= CART_LAST_TUNNEL) {
        sound_hold_fx_level(TUNNEL_FX_LEVEL);
        return TRUE;
    }
    sound_hold_fx_level(OPEN_FX_LEVEL);
    return FALSE;
}

Bool cart_tunnel_sound(struct task *task)
{
    struct cart_vehicle *kart;
    int piece;
    kart = task->motion.p;
    piece = cart_setup.pieces[cart_gate_sections[kart->gate]];
    if (piece >= CART_FIRST_TUNNEL && piece <= CART_LAST_TUNNEL) {
        sound_hold_fx_level(TUNNEL_FX_LEVEL);
        return TRUE;
    }
    sound_hold_fx_level(OPEN_FX_LEVEL);
    return FALSE;
}
#ifdef NONMATCHING
/* game_state.lives[0] by its own spelling: this side's objects don't define
 * cart_lives_0, which only symbols.txt places */
#define PLAYER_LIVES game_state.lives[0]
#else
/* @hack spelling: game_state.lives[0] read as cart_lives_0, its second
 * name */
extern Sint16 cart_lives_0;
#define PLAYER_LIVES cart_lives_0
#endif
static void time_over_step(struct task *task)
{
    task->work->timer++;
    if (task->work->timer <= TIME_OVER_STEPS) {
        return;
    } else if (PLAYER_LIVES - 1 < 0) {
        stage_end(STAGE_END_QUIT);
    } else {
        stage_end(STAGE_END_RESTART);
    }
}
static void time_over_draw(struct task *task)
{
}
static void time_over_destroy(struct task *task)
{
}
void cart_time_over(void)
{
    struct task *task;
    task = task_create(TASK_WORK, 2, time_over_step, "CartTimeOverExec");
    task->draw = time_over_draw;
    task->destructor = time_over_destroy;
    if (cart_race.countdown <= 0) {
        switch (cart_setup.course_mode) {
        case CART_TAILS:
            voice_play_priority_2(TAILS_TIME_OVER_VOICE);
            break;
        case CART_ROUGE:
            voice_play_priority_2(ROUGE_TIME_OVER_VOICE);
            break;
        }
        task->work->timer = 0;
    } else {
        task->work->timer = TIME_OVER_STEPS - CART_STEPS_PER_SECOND;
    }
}
