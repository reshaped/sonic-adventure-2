/* the port's features, which make dc adds to the game: a timing readout,
 * 16:9, and 60 fps where the game holds 30. each is a toggle, off at boot,
 * that a chord on the first pad sets. the game and Ninja reach them here,
 * each call under NONMATCHING. ninja.h includes this header under DREAMCAST
 * and NONMATCHING, for the draws' macros below */
#ifndef DC_PORT_H
#define DC_PORT_H

#include <ninja.h>
#include <sg_xpt.h>

/* the toggles. TOGGLE_READOUT is on while the readout shows a page */
enum port_toggle { TOGGLE_READOUT, TOGGLE_WIDE, TOGGLE_FAST, TOGGLES };
extern int port_toggles[TOGGLES];
/* TRUE once the chord has set a toggle since boot. a save's load leaves such
 * a toggle as it stands */
extern int port_chosen[TOGGLES];
/* read the chord on the first pad: L and R held, with a fresh press of the
 * d-pad. up steps the readout through its pages, right flips 16:9 and down
 * flips 60 fps. from that press until L, R and the d-pad are all up, the
 * game sees none of them. frame_wait_step calls it after its serve of the
 * pads, and frame_begin for frame_wait's */
void port_pad(void);
/* the save keeps 16:9 and 60 fps in SAVE_WIDE and SAVE_FAST of
 * save_data.status, whose other bits the game sets and these two it never
 * reads. port_store writes the two toggles there as a save is stored.
 * port_load gives each toggle the state the save was stored with, on or
 * off, so that none carries from one save to the next. a toggle the chord
 * has set keeps its state instead, and the next save stored takes it */
#define SAVE_WIDE 0x10
#define SAVE_FAST 0x20
void port_store(Uint8 *flags);
void port_load(Uint8 flags);

/* 60 fps. the game holds two kinds of frame at HELD_BLANKS vertical blanks,
 * so at 30 pictures a second: a two-player game's, and those of an event
 * whose first cue asks half rate. such a frame takes SPLIT_STEPS steps,
 * each a blank's worth of play, and draws once, after both. a split frame
 * shows a picture after each: the first step's at the frame's middle blank,
 * and the frame's own at its end. the frame keeps its blanks, its steps and
 * all the game counts once a frame. a held frame of any other count of
 * steps shows its one picture.
 *
 * the first picture changes nothing a step reads. its loop draws it as a
 * pause's frame draws, by the draw it gives present_step. Ninja's view goes
 * back as the last frame's draw left it: the screen, the clips and the
 * matrices, which a step may project by. rand()'s state goes back too, and
 * the game's timer stands still, so stage_run picks the next frame's steps
 * from the times it would read without the picture. two things do change.
 * the frame's second step reads the pads a blank later, since a blank
 * passes before it. and what a draw itself counts moves at each picture, as
 * it moves each blank in a one-player game.
 *
 * the split engages once ENGAGE_FRAMES held frames in a row would each have
 * fit both pictures with room to spare: each step with the frame's draw
 * inside a blank, which is a fifth longer under DISPLAY_PAL. a split frame
 * that runs late ends the split where another ran late within its last
 * SETTLE_FRAMES frames. the split then stays off for SETTLE_FRAMES frames,
 * and for twice as many at each end after, up to WAIT_MOST. a split that
 * lasts WAIT_MOST frames waits SETTLE_FRAMES again. so a scene the console
 * can't draw twice in a frame costs a late frame now and then, and otherwise
 * plays as shipped */
#define HELD_BLANKS 2
#define SPLIT_STEPS 2
#define ENGAGE_FRAMES 3
#define SETTLE_FRAMES 30
#define WAIT_MOST 480
/* a loop's call after each step of a frame it may split, with the steps the
 * frame has taken and the draw that makes its picture. a null draw says the
 * frame may not split. after the first of SPLIT_STEPS steps in a frame of
 * HELD_BLANKS blanks, where the split is engaged, it draws that picture
 * with the readout over it, renders it and flips to it at the next blank.
 * while frame_render_off is TRUE it shows none */
void present_step(int done, void (*draw)(void));
/* frame_wait's call once its wait ends. wait is when that wait began, in
 * syTmrGetCount's counts, and late the blanks the frame ran past its own.
 * it engages or ends the split for the frames to come. over a frame that
 * renders it draws pillarbox's bands, the readout and the note. it then
 * applies the 16:9 the toggle asks, so that each frame draws at one
 * aspect */
void frame_end(Uint32 wait, int late);
/* frame_wait's call after its serve of the pads, where the next frame's
 * first step starts. it notes the time and calls port_pad */
void frame_begin(void);
/* frame_vsync's call at each vertical blank. the readout counts pictures
 * against the blanks */
void port_blank(void);
/* the vertical blanks the display shows in a second: PAL_BLANKS under
 * DISPLAY_PAL, and NTSC_BLANKS under any other display. the readout names
 * its rate and its seconds by it, and the split takes a blank's length from
 * it. it goes by game_state.display, as the game's own pacing does. the TV
 * setting's test shows 60 Hz and leaves game_state.display as it stands, so
 * a game under DISPLAY_PAL gets PAL_BLANKS through that test too */
#define NTSC_BLANKS 60
#define PAL_BLANKS 50
int port_blank_rate(void);

/* the readout's pages, which the chord steps through in this order */
enum readout_page { READOUT_OFF, READOUT_FPS, READOUT_TIMES, READOUTS };
/* step the readout to its next page. it returns TRUE where a page then
 * shows */
int readout_step(void);
/* name toggle, a port_toggle, and its new state near the screen's top for
 * two seconds of the display's blanks */
void readout_note(int toggle);
/* add a frame to the readout's half second, and age the note by the frame's
 * blanks. late is as frame_end takes it, and pictures the pictures the
 * frame showed. the rate is the pictures over the half second's blanks, at
 * port_blank_rate() blanks a second: 50 under DISPLAY_PAL where each blank
 * shows a picture, and 60 under any other display. a frame a second late or
 * more held a load or a movie, which the readout draws nothing over: the
 * half second starts again after it, and the rate shown stays until then */
void readout_frame(int late, int pictures);
/* draw the readout's page and the note over the picture. they keep x under
 * 16:9, so a 16:9 screen shows them a third wider */
void readout_draw(void);
/* njPrintC's print of s at loc, in cells size pixels a side and the color c.
 * the print's own size and color stay as the game set them.
 * sdk/ninja/njDebug.c defines it */
void readout_print(Int loc, const char *s, Uint16 size, Uint32 c);

/* 16:9. the picture stays 640 by 480 pixels, and a 16:9 screen stretches it
 * by a third. so the port draws a view a third wider into it:
 * _nj_scr_aspect_x_, which scales the x of everything Ninja's renderers
 * project, goes from 1 to WIDE_ASPECT. the view keeps its height, since
 * njSetCamera's non-matching side leaves the aspect out of the screen's
 * distance.
 *
 * the game's own projections stay in the pixels of the 4:3 picture.
 * njProjectScreen's callers reach wide_project_screen, which projects at an
 * aspect of 1, and screen_project leaves the aspect out. such an x can lie
 * past either edge of the screen.
 *
 * the window is how wide the game's tests of what is on screen take the
 * view to be. port_view_aspect holds it, and four tests read it:
 * point_in_upper_screen, in_view_box, cave_on_screen and
 * garden_visible_box. each answers as SEGA's code would at that aspect. in
 * play the window is the picture's, so what shows in the wide sides acts as
 * in view. while a demo plays or records the window stays 4:3: a replay
 * keeps in step only with its recording's calls to rand(), and some steps
 * call it only for what is in view. the picture then draws pillarboxed.
 * black bands cover its sides, so that it shows the window and no more.
 *
 * a 2D draw places its corners in the 4:3 picture's pixels. under 16:9 it
 * squeezes their x toward its pivot by WIDE_ASPECT, which keeps its shape
 * on a 16:9 screen, and puts a draw the game placed by projection where the
 * picture shows the point. the pivot is the screen's centre, _nj_screen_.cx,
 * which a split screen's view moves to its own middle, plus
 * port_wide_anchor. the 2D draws are njDrawSprite2D, njDrawTexture,
 * njDrawTextureMemList, njDrawPolygon, njDrawPolygon2D and
 * njDrawQuadTexture, and a movie's picture squeezes as they do. the debug
 * print keeps x, and so does njDrawQuadTextureEx, whose quads the particles
 * project themselves.
 *
 * a draw that covers the screen keeps x, so that it still covers it: an
 * untextured polygon whose corners port_wide_covers finds at both edges, or
 * any 2D draw while port_wide_cover is TRUE. under pillarbox every 2D draw
 * squeezes about the screen's centre, whatever the anchor and the cover
 * say, and the bands cover what it leaves */
#define WIDE_ASPECT 0.75F
/* the pivots port_wide_anchor takes, in pixels from the screen's centre: the
 * centre itself, and the whole screen's left and right edges */
#define WIDE_MIDDLE 0.0F
#define WIDE_LEFT (-320.0F)
#define WIDE_RIGHT 320.0F
/* a function that sets either puts back WIDE_MIDDLE and FALSE before it
 * returns */
extern Float port_wide_anchor;
extern Int port_wide_cover;
/* TRUE while the picture draws at WIDE_ASPECT */
extern Int port_wide;
/* TRUE under pillarbox, until wide_end */
extern Int port_pillarbox;
/* the window's aspect, 1 or WIDE_ASPECT, which wide_apply sets between
 * frames */
extern Float port_view_aspect;
/* x in the 4:3 picture's pixels, moved to where the window takes it: its
 * offset from _nj_screen_.cx scales by port_view_aspect. a test then holds
 * it against the bounds SEGA's code holds x against */
Float port_view_x(Float x);
/* make the picture's aspect, the window and pillarbox what the toggle and
 * the game's demo ask, and give the renderers' kept copies of the screen's
 * scale a new aspect. frame_end calls it between frames, so that each frame
 * draws at one aspect and plays in one window */
void wide_apply(void);
/* end the frame that wide_apply's state drew. under pillarbox it draws the
 * bands and ends pillarbox, so that the readout drawn after keeps x */
void wide_end(void);
/* x as a 2D draw squeezes it about its pivot, or x itself where the draws
 * keep x */
Float port_wide_x(Float x);
/* TRUE where the 2D draws squeeze: under 16:9 while port_wide_cover is
 * FALSE, and under pillarbox */
Int port_wide_squeezes(void);
/* TRUE outside pillarbox where count corners span the whole screen's width,
 * so that an untextured polygon keeps x. each corner's x lies within a pixel
 * of 0 or of the width, and corners lie at both. the first corner's x is at
 * x, and each next one stride floats on */
Int port_wide_covers(const Float *x, Int count, Int stride);
/* njDrawSprite2D under the squeeze: the cell's point lands where port_wide_x
 * puts p.x, and each corner's x scales about that point by WIDE_ASPECT.
 * sdk/ninja/njSprite.src holds the draw itself, which both builds assemble
 * as it stands and which draws every sprite outside the squeeze. so the
 * game's calls reach this by the macro */
void wide_draw_sprite_2d(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr);
#define njDrawSprite2D(sp, n, pri, attr) wide_draw_sprite_2d(sp, n, pri, attr)
/* njProjectScreen at an x aspect of 1, whatever the picture's.
 * sdk/ninja/njProjectScreen.c holds the projection itself, an inline body
 * the port runs as it stands. so the game's calls reach this by the macro */
void wide_project_screen(NJS_MATRIX *m, NJS_POINT3 *p3, NJS_POINT2 *p2);
#define njProjectScreen(m, p3, p2) wide_project_screen(m, p3, p2)
/* njPtclDrawSprite scales a sprite's centre by the x aspect, and leaves it
 * out of the sprite's width. so the game's calls give the width the aspect
 * here, which is 1 but under 16:9. sdk/ninja/njPtclSprite.c holds the draw,
 * an inline body the port runs as it stands */
#define njPtclDrawSprite(p, n, w, h)                                           \
    njPtclDrawSprite(p, n, (w) * _nj_scr_aspect_x_, h)

#endif
