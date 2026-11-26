/* the port's readout, its on-screen timing display, and the note that names
 * a toggle as the chord sets it. both draw over each frame's picture. make
 * dc links this into 1ST_READ.BIN after the image's sources */
#include <ninja.h>
#include <sg_xpt.h>
#include <stdio.h>
#include <string.h>

#include "engine/dc/port.h"
#include "system/frame.h"
#include "system/game_state.h"

/* the readout prints in cells of CELL pixels, COLUMNS across. the title-safe
 * area is the middle 80 percent, 4 columns in from each side. a line ends at
 * RIGHT_END, a cell inside it, and TOP_ROW lies a cell inside it too */
#define CELL 16
#define COLUMNS 40
#define RIGHT_END 35
#define LINE_CHARS (RIGHT_END - COLUMNS / 10)
#define TOP_ROW 4
#define TIMES_LINES 4
/* the blanks the note fades in and out over, and the seconds it shows for
 * in all */
#define NOTE_FADE 15
#define NOTE_SECONDS 2
/* the alpha of the dark band under the text, out of 255 */
#define BAND_ALPHA 0xa0
/* the pixels the band reaches past its text, across and down */
#define BAND_PAD_X 8
#define BAND_PAD_Y 4
/* the blanks the readout sums before it writes its lines, half a second */
#define SAMPLE_BLANKS ((Uint32)port_blank_rate() / 2)
/* a frame this many blanks late, a second, held a load or a movie. a movie
 * plays in a loop of its own inside one frame, and shows no picture of the
 * game's */
#define STALL_BLANKS port_blank_rate()
/* game_state's times count 60 to a millisecond under every display: a
 * thousand to a blank at 60 Hz, and 1,200 at 50. a tenth of a millisecond is
 * TIME_TENTH of them, and a second TIME_SECOND */
#define TIME_TENTH 6
#define TIME_SECOND 60000

/* the note's text, its blanks left, and the blanks it has faded in */
static struct {
    char text[16];
    int left;
    int level;
} note;
static enum readout_page readout;
/* the blanks since boot, which port_blank counts */
static volatile Uint32 blanks;
/* the half second the readout sums: its first blank, and its frames, the
 * pictures they showed, the frames the game held at HELD_BLANKS, those that
 * showed two pictures, their steps, those that ran late, and the sums and
 * the peak of game_state's three times */
static struct {
    int started;
    Uint32 blank;
    int frames;
    int pictures;
    int held;
    int split;
    int steps;
    int late;
    Float busy, draw, render;
    Float peak;
} sample;
static char fps_line[sizeof("999 FPS")] = "-- FPS";
static Uint32 fps_color = 0xffffff;
static char times_lines[TIMES_LINES][LINE_CHARS + 1];

void port_blank(void)
{
    blanks++;
}

/* value, or most where value passes it. sprintf checks no length, so a
 * figure that could outgrow its place in a line passes through this */
static Uint32 at_most(Uint32 value, Uint32 most)
{
    return value < most ? value : most;
}

/* a sum of game_state's times as its mean over the half second's frames, in
 * tenths of a millisecond up to 99.9 */
static Uint32 mean_tenths(Float sum)
{
    return at_most((Uint32)(sum / sample.frames) / TIME_TENTH, 999);
}

/* ON where the game applies a mode, HELD where only its toggle asks it, and
 * else OFF */
static const char *mode_name(int asked, int applied)
{
    return applied ? "ON" : asked ? "HELD" : "OFF";
}

/* write the readout's lines for the half second ending at now. the rate is
 * its pictures over its blanks, as pictures a second on this display. 60 fps
 * shows ON where each frame the game held showed two pictures. the times
 * page adds the mean of the frame's time up to its wait, colored by its peak
 * against the frame's blanks: green, amber past 90 percent, red past all of
 * it. then come the modes, the mean steps a frame with the frames that ran
 * late, and the means of the two times stage_run picks its steps from */
static void readout_write(Uint32 now)
{
    static const Uint32 colors[3] = {0x60e080, 0xffc040, 0xff5050};
    Uint32 span = now - sample.blank;
    Uint32 fps = (sample.pictures * port_blank_rate() + span / 2) / span;
    Float budget = (Float)(frame_interval * TIME_SECOND / port_blank_rate());
    Uint32 busy;
    Uint32 draw;
    Uint32 render;
    Uint32 steps;

    sprintf(fps_line, "%2u FPS", (unsigned)at_most(fps, 999));
    if (readout != READOUT_TIMES) {
        return;
    }
    busy = mean_tenths(sample.busy);
    draw = mean_tenths(sample.draw);
    render = mean_tenths(sample.render);
    steps = at_most(sample.steps * 10 / sample.frames, 99);
    fps_color =
        colors[(sample.peak * 10.0F > budget * 9.0F) + (sample.peak > budget)];
    sprintf(times_lines[0], "%s %2u.%u MS", fps_line, (unsigned)(busy / 10),
            (unsigned)(busy % 10));
    sprintf(
        times_lines[1], "16:9 %s  60 FPS %s",
        mode_name(port_toggles[TOGGLE_WIDE], port_wide),
        mode_name(port_toggles[TOGGLE_FAST],
                  port_toggles[TOGGLE_FAST] && sample.split == sample.held));
    sprintf(times_lines[2], "STEPS %u.%u  LATE %u", (unsigned)(steps / 10),
            (unsigned)(steps % 10), (unsigned)at_most(sample.late, 99));
    sprintf(times_lines[3], "DRAW %2u.%u  RENDER %2u.%u", (unsigned)(draw / 10),
            (unsigned)(draw % 10), (unsigned)(render / 10),
            (unsigned)(render % 10));
}

void readout_frame(int late, int pictures)
{
    Uint32 now = blanks;
    int spent = frame_interval + late;
    int most;

    note.left = note.left > spent ? note.left - spent : 0;
    most = note.left > NOTE_FADE ? NOTE_FADE : note.left;
    note.level += spent;
    if (note.level > most) {
        note.level = most;
    }
    if (readout == READOUT_OFF) {
        return;
    }
    /* the half second starts again after a load or a movie, so that the
     * rate is of the frames the game paces */
    if (late >= STALL_BLANKS) {
        sample.started = FALSE;
        return;
    }
    if (!sample.started) {
        memset(&sample, 0, sizeof(sample));
        sample.started = TRUE;
        sample.blank = now;
        return;
    }
    sample.frames++;
    sample.pictures += pictures;
    sample.held += frame_interval == HELD_BLANKS;
    sample.split += pictures > 1;
    sample.steps += frame_step_count;
    sample.late += late != 0;
    sample.busy += game_state.frame_time;
    sample.draw += game_state.draw_time;
    sample.render += game_state.render_time;
    if (game_state.frame_time > sample.peak) {
        sample.peak = game_state.frame_time;
    }
    if (now - sample.blank >= SAMPLE_BLANKS) {
        readout_write(now);
        sample.started = FALSE;
    }
}

/* a dark band of the alpha given, out of 255, from cell (col, row) across
 * width cells and down lines. it keeps x under 16:9, as the prints do */
static void band_draw(int col, int row, int width, int lines, int alpha)
{
    NJS_POINT2 points[4];
    NJS_COLOR colors[4];
    NJS_POINT2COL band;
    int i;

    points[0].x = points[3].x = (Float)(col * CELL - BAND_PAD_X);
    points[1].x = points[2].x = (Float)((col + width) * CELL + BAND_PAD_X);
    points[0].y = points[1].y = (Float)(row * CELL - BAND_PAD_Y);
    points[2].y = points[3].y = (Float)((row + lines) * CELL + BAND_PAD_Y);
    for (i = 0; i < 4; i++) {
        colors[i].color = (Uint32)alpha << 24;
    }
    band.p = points;
    band.col = colors;
    band.tex = NULL;
    band.num = 4;
    port_wide_cover = TRUE;
    njDrawPolygon2D(&band, 4, -1.0F, NJD_FILL | NJD_TRANSPARENT);
    port_wide_cover = FALSE;
}

/* text at col and row in the rgb color given. alpha, out of 255, fades it */
static void text_draw(int col, int row, const char *text, Uint32 color,
                      int alpha)
{
    readout_print(NJM_LOCATION(col, row), text, CELL,
                  (Uint32)alpha << 24 | color);
}

/* draw the times page at the top right on one dark band */
static void times_draw(void)
{
    int width = 0;
    int length;
    int i;

    for (i = 0; i < TIMES_LINES; i++) {
        length = (int)strlen(times_lines[i]);
        width = length > width ? length : width;
    }
    band_draw(RIGHT_END - width, TOP_ROW, width, TIMES_LINES, BAND_ALPHA);
    for (i = 0; i < TIMES_LINES; i++) {
        text_draw(RIGHT_END - (int)strlen(times_lines[i]), TOP_ROW + i,
                  times_lines[i], i == 0 ? fps_color : 0xffffff, 255);
    }
}

void readout_draw(void)
{
    int length;
    int lines = 0;
    int alpha;

    if (readout == READOUT_FPS) {
        length = (int)strlen(fps_line);
        lines = 1;
        band_draw(RIGHT_END - length, TOP_ROW, length, 1, BAND_ALPHA);
        text_draw(RIGHT_END - length, TOP_ROW, fps_line, 0xffffff, 255);
    } else if (readout == READOUT_TIMES) {
        lines = TIMES_LINES;
        times_draw();
    }
    if (note.level > 0) {
        length = (int)strlen(note.text);
        alpha = note.level * 255 / NOTE_FADE;
        band_draw((COLUMNS - length) / 2, TOP_ROW + lines + 1, length, 1,
                  alpha * BAND_ALPHA / 255);
        text_draw((COLUMNS - length) / 2, TOP_ROW + lines + 1, note.text,
                  0xffffff, alpha);
    }
}

int readout_step(void)
{
    int i;

    readout = readout == READOUT_TIMES ? READOUT_OFF : readout + 1;
    sample.started = FALSE;
    strcpy(fps_line, "-- FPS");
    fps_color = 0xffffff;
    strcpy(times_lines[0], fps_line);
    for (i = 1; i < TIMES_LINES; i++) {
        times_lines[i][0] = 0;
    }
    return readout != READOUT_OFF;
}

void readout_note(int toggle)
{
    static const char *const names[TOGGLES] = {"READOUT", "16:9", "60 FPS"};
    static const char *const pages[READOUTS] = {"OFF", "FPS", "TIMES"};

    sprintf(note.text, "%s %s", names[toggle],
            toggle == TOGGLE_READOUT ? pages[readout]
            : port_toggles[toggle]   ? "ON"
                                     : "OFF");
    note.left = NOTE_SECONDS * port_blank_rate();
}
