"""Check the port's features: the chord and its toggles, the save's two bits,
the split frame, the readout's rate, 16:9's squeeze and its window, and the
rules their callers keep (engine/dc/port.h)."""

import ctypes
import re
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host

SOURCES = ('engine/dc/port.c', 'engine/dc/pace.c', 'engine/dc/readout.c', 'engine/dc/wide.c',
           'tests/port/fixture.c')
# shinobi.h's buttons, and port.h's toggles and pivots
UP, DOWN, RIGHT, L, R, A = 1 << 4, 1 << 5, 1 << 7, 1 << 17, 1 << 16, 1 << 2
READOUT, WIDE, FAST = range(3)
WIDE_LEFT = -320.0
# game_state.h's display of 50 blanks a second
DISPLAY_PAL = 2
# Kamui's fog modes: the fog table, and none
KM_FOGTABLE, KM_NOFOG = 0, 2
# readout.c's colors for a frame inside its blanks and for one past them
GREEN, RED = 0x60e080, 0xff5050
# a held frame that fits two pictures: its two steps and its draw, in syTmrGetCount's counts, 13034 to a blank
FITS = (4000, 4000, 5000)
# the directories whose C the game and its libraries build from
GAME = ('src', 'sdk', 'modules')
# who may assign Ninja's x aspect on make dc's side: njInitGlobal gives it
# its 1, and wide.c sets and lends it
ASPECT_WRITERS = {'sdk/ninja/njSystem.c', 'engine/dc/wide.c'}
# who reads the window: wide.c, which keeps it, and the four tests port.h names
WINDOW_READERS = {'engine/dc/wide.c': None, 'src/enemy/enemy.c': 'point_in_upper_screen',
                  'modules/shared/canyon_util.c': 'in_view_box', 'modules/stg28/cave_util.c': 'cave_on_screen',
                  'modules/chao/movement.c': 'garden_visible_box'}
PUT_BACK = {'port_wide_anchor': 'WIDE_MIDDLE', 'port_wide_cover': 'FALSE'}


class Cell(ctypes.Structure):
    """NJS_TEXANIM."""
    _fields_ = [(name, ctypes.c_short) for name in ('sx', 'sy', 'cx', 'cy', 'u1', 'v1', 'u2', 'v2', 'texid', 'attr')]


class Sprite(ctypes.Structure):
    """NJS_SPRITE."""
    _fields_ = [('p', ctypes.c_float * 3), ('sx', ctypes.c_float), ('sy', ctypes.c_float), ('ang', ctypes.c_int32),
                ('tlist', ctypes.c_void_p), ('tanim', ctypes.POINTER(Cell))]


class Vertex(ctypes.Structure):
    """wide.c's sprite_vertex."""
    _fields_ = [('pcw', ctypes.c_uint32), *((name, ctypes.c_float) for name in ('x', 'y', 'z', 'u', 'v', 'base', 'offset'))]


class Port(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        directory = tempfile.TemporaryDirectory()
        cls.addClassCleanup(directory.cleanup)
        cls.library = Path(directory.name) / 'port.so'
        compile_host(cls.library, '-std=gnu99', '-O1', '-I.', '-Isrc', *SOURCES, shared=True)

    def setUp(self):
        # each test takes the library afresh, with every toggle off
        self.port = ctypes.CDLL(str(self.library))
        handle = self.port._handle
        self.addCleanup(lambda: ctypes.CDLL(None).dlclose(ctypes.c_void_p(handle)))
        for name in ('port_wide_x', 'port_view_x'):
            getattr(self.port, name).restype = ctypes.c_float
            getattr(self.port, name).argtypes = [ctypes.c_float]
        self.port.fixture_held_frame.argtypes = [ctypes.c_uint32] * 3 + [ctypes.c_int]
        self.port.present_step.argtypes = [ctypes.c_int, ctypes.c_void_p]
        self.toggles = (ctypes.c_int * 3).in_dll(self.port, 'port_toggles')

    def value(self, kind, name):
        return kind.in_dll(self.port, name)

    def scan(self, held):
        """Serve the pad with held down, then give it to the port. What the game then reads as held."""
        self.value(ctypes.c_uint32, 'fixture_held').value = held
        self.port.fixture_serve()
        self.port.port_pad()
        return (ctypes.c_uint32 * 6).in_dll(self.port, 'fixture_pad')[2]

    def prints(self):
        self.port.fixture_draw()
        count = self.value(ctypes.c_int, 'fixture_print_count').value
        lines = (ctypes.c_char * 40 * 16).in_dll(self.port, 'fixture_prints')
        return [lines[i].value.decode() for i in range(count)]

    def held(self, first=FITS[0], second=FITS[1], draw=FITS[2], blanks=2, frames=1):
        """Run held frames of two steps, and return the pictures they showed after their first steps."""
        flips = self.value(ctypes.c_int, 'fixture_flips')
        before = flips.value
        for _ in range(frames):
            self.port.fixture_held_frame(first, second, draw, blanks)
        return flips.value - before

    def test_the_chord_flips_a_toggle_and_hides_its_buttons(self):
        pad = (ctypes.c_uint32 * 6).in_dll(self.port, 'fixture_pad')
        self.assertEqual(self.scan(L | R), L | R)
        self.assertEqual(list(self.toggles), [0, 0, 0])
        self.assertEqual(self.scan(L | R | RIGHT), 0)
        self.assertEqual(list(self.toggles), [0, 1, 0])
        self.assertEqual(pad[4], 0)
        # held, the chord flips nothing more, and stays hidden until all is up
        self.assertEqual(self.scan(L | R | RIGHT), 0)
        self.assertEqual(pad[4], 0)
        self.assertEqual(self.scan(L | A), A)
        self.assertEqual(self.scan(0), 0)
        self.assertEqual(self.scan(L), L)
        self.assertEqual(list(self.toggles), [0, 1, 0])
        self.scan(L | R)
        self.scan(L | R | DOWN)
        self.assertEqual(list(self.toggles), [0, 1, 1])
        self.scan(0)
        self.scan(L | R)
        self.scan(L | R | RIGHT)
        self.assertEqual(list(self.toggles), [0, 0, 1])

    def test_the_d_pad_alone_or_held_first_flips_nothing(self):
        self.assertEqual(self.scan(RIGHT), RIGHT)
        self.assertEqual(self.scan(RIGHT | L), RIGHT | L)
        self.assertEqual(self.scan(RIGHT | L | R), RIGHT | L | R)
        self.assertEqual(list(self.toggles), [0, 0, 0])

    def test_up_steps_the_readout_through_its_pages(self):
        notes = []
        for _ in range(3):
            self.scan(L | R)
            self.scan(L | R | UP)
            self.scan(0)
            self.port.fixture_frame(1)
            notes.append((self.toggles[READOUT], self.prints()[-1]))
        self.assertEqual(notes, [(1, 'READOUT FPS'), (1, 'READOUT TIMES'), (0, 'READOUT OFF')])

    def test_the_readout_counts_pictures_against_blanks(self):
        self.scan(L | R)
        self.scan(L | R | UP)
        self.scan(0)
        for blanks, rate in ((1, '60 FPS'), (2, '30 FPS'), (3, '20 FPS')):
            self.value(ctypes.c_int, 'frame_interval').value = blanks
            for _ in range(90):
                self.port.fixture_frame(blanks)
            self.assertEqual(self.prints()[0], rate)
        # a frame that runs a blank late shows as the rate it made
        self.value(ctypes.c_int, 'frame_interval').value = 1
        for _ in range(90):
            self.port.fixture_frame(2)
        self.assertEqual(self.prints()[0], '30 FPS')
        # a stall under a second counts in the rate, as the pictures it cost
        for _ in range(4):
            self.port.fixture_frame(1)
        self.port.fixture_frame(40)
        self.assertLess(int(self.prints()[0].split()[0]), 20)

    def test_the_readout_starts_again_after_a_load_or_a_movie(self):
        self.scan(L | R)
        self.scan(L | R | UP)
        self.scan(0)
        for _ in range(90):
            self.port.fixture_frame(1)
        self.assertEqual(self.prints()[0], '60 FPS')
        # a frame ten seconds long is a movie's: its blanks stay out of the rate
        self.port.fixture_frame(600)
        self.assertEqual(self.prints()[0], '60 FPS')
        for _ in range(45):
            self.port.fixture_frame(1)
            self.assertEqual(self.prints()[0], '60 FPS')

    def test_the_readout_counts_50_blanks_to_a_second_under_pal(self):
        self.port.fixture_set_display(DISPLAY_PAL)
        self.scan(L | R)
        self.scan(L | R | UP)
        self.scan(0)
        # half a second is 25 blanks: the frame that starts it, and 25 more
        for _ in range(25):
            self.port.fixture_frame(1)
        self.assertEqual(self.prints()[0], '-- FPS')
        self.port.fixture_frame(1)
        self.assertEqual(self.prints()[0], '50 FPS')
        self.value(ctypes.c_int, 'frame_interval').value = 2
        for _ in range(90):
            self.port.fixture_frame(2)
        self.assertEqual(self.prints()[0], '25 FPS')
        # a frame 50 blanks late is a second late, and stays out of the rate
        self.value(ctypes.c_int, 'frame_interval').value = 1
        for _ in range(90):
            self.port.fixture_frame(1)
        for _ in range(4):
            self.port.fixture_frame(1)
        self.port.fixture_frame(51)
        self.assertEqual(self.prints()[0], '50 FPS')
        # on any other display it is under a second, and counts as the pictures it cost
        self.port.fixture_set_display(0)
        for _ in range(90):
            self.port.fixture_frame(1)
        self.assertEqual(self.prints()[0], '60 FPS')
        for _ in range(4):
            self.port.fixture_frame(1)
        self.port.fixture_frame(51)
        self.assertLess(int(self.prints()[0].split()[0]), 20)

    def test_the_note_shows_for_two_seconds_of_the_displays_blanks(self):
        for display, blanks in ((0, 120), (DISPLAY_PAL, 100)):
            with self.subTest(display=display):
                self.port.fixture_set_display(display)
                self.scan(L | R)
                self.scan(L | R | RIGHT)
                self.scan(0)
                for _ in range(blanks - 1):
                    self.port.fixture_frame(1)
                self.assertEqual(len(self.prints()), 1)
                self.port.fixture_frame(1)
                self.assertEqual(self.prints(), [])

    def test_the_times_are_milliseconds_and_the_budget_the_displays_blank(self):
        colors = (ctypes.c_uint32 * 16).in_dll(self.port, 'fixture_print_colors')
        for _ in range(2):
            self.scan(L | R)
            self.scan(L | R | UP)
            self.scan(0)
        # a held frame 35.0 ms long is past two blanks at 60 Hz, and inside nine tenths of two at 50
        for display, line, color in ((0, '30 FPS 35.0 MS', RED), (DISPLAY_PAL, '25 FPS 35.0 MS', GREEN)):
            with self.subTest(display=display):
                self.port.fixture_set_display(display)
                self.held(first=9000, second=9000, draw=9372, frames=80)
                self.assertEqual((self.prints()[0], colors[0] & 0xffffff), (line, color))

    def test_the_split_engages_once_three_held_frames_fit(self):
        self.toggles[FAST] = 1
        self.assertEqual(self.held(frames=3), 0)
        self.assertEqual(self.held(), 1)
        self.assertEqual(self.held(frames=10), 10)

    def test_the_picture_puts_back_what_a_step_reads(self):
        self.toggles[FAST] = 1
        self.held(frames=3)
        seed = self.value(ctypes.c_ulong, '_rnext')
        seed.value = 7
        screen = (ctypes.c_float * 5).in_dll(self.port, '_nj_screen_')
        stack = (ctypes.c_float * 32).in_dll(self.port, 'fixture_stack')
        view = (ctypes.c_float * 16).in_dll(self.port, '_nj_view_matrix_')
        before = (list(screen), list(stack), list(view))
        self.assertEqual(self.held(), 1)
        # the draw ran with the game's timer stopped, and moved the view and rand()'s state
        self.assertEqual([self.value(ctypes.c_int, name).value for name in
                          ('fixture_step_draws', 'fixture_drawn_stopped', 'fixture_timer_runs')], [1, 1, 1])
        self.assertEqual((list(screen), list(stack), list(view)), before)
        self.assertEqual(seed.value, 7)
        self.assertEqual(self.value(ctypes.c_float, '_nj_far_clip_').value, 60000.0)
        self.assertEqual((ctypes.c_float * 4).in_dll(self.port, '_nj_2d_clip_')[0], 0.0)
        top = ctypes.c_void_p.in_dll(self.port, '_nj_current_matrix_ptr_')
        self.assertEqual(top.value, ctypes.addressof(stack))

    def test_a_frame_that_cannot_fit_two_pictures_never_splits(self):
        self.toggles[FAST] = 1
        # the first step and the draw pass a blank less the room of 931 counts; then the second
        self.assertEqual(self.held(first=8000, second=2000, draw=5000, frames=8), 0)
        self.assertEqual(self.held(first=2000, second=8000, draw=5000, frames=8), 0)
        self.assertEqual(self.held(first=6000, second=6000, draw=6104, frames=8), 0)
        self.assertEqual(self.held(first=6000, second=6000, draw=6103, frames=8), 5)
        self.toggles[FAST] = 0
        self.assertEqual(self.held(frames=8), 0)

    def test_a_pal_blank_gives_the_split_its_own_room(self):
        self.toggles[FAST] = 1
        # at 59.94 blanks a second this frame's halves don't fit
        self.assertEqual(self.held(first=7000, second=7000, draw=7509, frames=8), 0)
        # a PAL blank is 15625 counts and its room 1116, so a step and the draw fit in 14509
        self.port.fixture_set_display(DISPLAY_PAL)
        self.assertEqual(self.held(first=7000, second=7000, draw=7510, frames=8), 0)
        self.assertEqual(self.held(first=7000, second=7000, draw=7509, frames=8), 5)

    def test_only_a_held_frame_of_two_steps_splits(self):
        self.toggles[FAST] = 1
        self.held(frames=3)
        interval = self.value(ctypes.c_int, 'frame_interval')
        count = self.value(ctypes.c_int, 'frame_step_count')
        flips = self.value(ctypes.c_int, 'fixture_flips')
        draw = ctypes.cast(self.port.fixture_step_draw, ctypes.c_void_p)
        # a frame of three steps shows one picture, and leaves the split engaged
        count.value = 3
        for done in (1, 2, 3):
            self.port.present_step(done, draw)
        self.port.fixture_frame(2)
        self.assertEqual((flips.value, self.held()), (0, 1))
        # a frame of one blank offers nothing, and a frame its loop refuses ends the split
        interval.value = 1
        self.port.present_step(1, draw)
        self.assertEqual(flips.value, 1)
        interval.value = 2
        count.value = 2
        self.port.present_step(1, None)
        self.port.fixture_frame(2)
        self.assertEqual(self.held(frames=3), 0)
        self.assertEqual(self.held(), 1)
        # no picture shows while the frame's render is off
        self.value(ctypes.c_int32, 'frame_render_off').value = 1
        self.assertEqual(self.held(), 0)

    def test_late_split_frames_end_the_split_for_longer_each_time(self):
        self.toggles[FAST] = 1
        self.held(frames=3)
        for wait in (30, 60, 120):
            # the first late frame is forgiven, and a second within 30 frames ends the split
            self.assertEqual(self.held(blanks=3, frames=2), 2)
            self.assertEqual(self.held(frames=wait + 3), 0)
            self.assertEqual(self.held(), 1)
        # two late frames more than 30 frames apart end nothing
        self.assertEqual(self.held(frames=31), 31)
        self.assertEqual(self.held(blanks=3), 1)
        self.assertEqual(self.held(frames=40), 40)

    def test_the_readout_counts_both_pictures_of_a_split_frame(self):
        for _ in range(2):
            self.scan(L | R)
            self.scan(L | R | UP)
            self.scan(0)
        self.held(frames=40)
        self.assertEqual(self.prints()[:2], ['30 FPS 16.6 MS', '16:9 OFF  60 FPS OFF'])
        # on, 60 fps shows HELD while the frames can't split, and ON once each does
        self.toggles[FAST] = 1
        self.held(first=8000, second=2000, draw=5000, frames=40)
        self.assertEqual(self.prints()[:2], ['30 FPS 19.1 MS', '16:9 OFF  60 FPS HELD'])
        self.held(frames=60)
        self.assertEqual(self.prints()[:2], ['60 FPS 16.6 MS', '16:9 OFF  60 FPS ON'])

    def test_the_save_keeps_16_9_and_60_fps(self):
        flags = ctypes.c_ubyte(0x07)
        self.toggles[WIDE] = 1
        self.port.port_store(ctypes.byref(flags))
        self.assertEqual(flags.value, 0x17)
        self.toggles[WIDE], self.toggles[FAST] = 0, 1
        self.port.port_store(ctypes.byref(flags))
        self.assertEqual(flags.value, 0x27)
        # a load gives each toggle the save's state, on or off
        self.port.port_load(0x10)
        self.assertEqual(list(self.toggles), [0, 1, 0])
        # but for one the chord has set since boot
        self.scan(L | R)
        self.scan(L | R | DOWN)
        self.port.port_load(0x10)
        self.assertEqual(list(self.toggles), [0, 1, 1])
        self.port.port_load(0x00)
        self.assertEqual(list(self.toggles), [0, 0, 1])

    def test_the_readout_keeps_x_under_16_9(self):
        self.scan(L | R)
        self.scan(L | R | UP)
        self.toggles[WIDE] = 1
        self.port.fixture_frame(1)
        self.port.fixture_draw()
        self.assertGreater(self.value(ctypes.c_int, 'fixture_bands').value, 0)
        self.assertEqual(self.value(ctypes.c_int, 'fixture_band_cover').value, 1)
        self.assertEqual(self.value(ctypes.c_int, 'port_wide_cover').value, 0)

    def test_16_9_applies_between_frames(self):
        aspect = self.value(ctypes.c_float, '_nj_scr_aspect_x_')
        self.toggles[WIDE] = 1
        self.assertEqual((aspect.value, self.port.port_wide_x(0.0)), (1.0, 0.0))
        self.port.fixture_frame(1)
        self.assertEqual(aspect.value, 0.75)
        # the renderers' four kept scales take the new aspect
        self.assertEqual(self.value(ctypes.c_int, 'fixture_lights').value, 4)
        self.port.fixture_frame(1)
        self.assertEqual(self.value(ctypes.c_int, 'fixture_lights').value, 4)
        self.toggles[WIDE] = 0
        self.port.fixture_frame(1)
        self.assertEqual((aspect.value, self.port.port_wide_x(0.0)), (1.0, 0.0))

    def test_the_squeeze_pivots_on_the_view_and_the_anchor(self):
        self.toggles[WIDE] = 1
        self.port.fixture_frame(1)
        x = self.port.port_wide_x
        self.assertEqual([x(0.0), x(320.0), x(640.0)], [80.0, 320.0, 560.0])
        anchor = self.value(ctypes.c_float, 'port_wide_anchor')
        anchor.value = WIDE_LEFT
        self.assertEqual([x(0.0), x(64.0)], [0.0, 48.0])
        anchor.value = 0.0
        # a split screen's view squeezes about its own middle
        screen = (ctypes.c_float * 5).in_dll(self.port, '_nj_screen_')
        screen[3] = 160.0
        self.assertEqual([x(0.0), x(160.0), x(320.0)], [40.0, 160.0, 280.0])
        screen[3] = 320.0
        self.value(ctypes.c_int, 'port_wide_cover').value = 1
        self.assertEqual(x(0.0), 0.0)

    def test_the_window_widens_in_play_and_a_demo_plays_pillarboxed(self):
        view = self.port.port_view_x
        aspect = self.value(ctypes.c_float, 'port_view_aspect')
        demo = ctypes.c_byte.in_dll(self.port, 'game_state')
        self.assertEqual((aspect.value, view(0.0)), (1.0, 0.0))
        self.toggles[WIDE] = 1
        self.port.fixture_frame(1)
        # in play a point a sixth of the picture past the 4:3 edge lies on the window's edge
        self.assertEqual(aspect.value, 0.75)
        self.assertAlmostEqual(view(640.0 + (320.0 / 3)), 640.0, places=3)
        self.assertAlmostEqual(self.port.port_wide_x(640.0 + (320.0 / 3)), 640.0, places=3)
        self.assertEqual(self.value(ctypes.c_int, 'port_pillarbox').value, 0)
        # a demo keeps the 4:3 window, and every 2D draw squeezes about the middle
        demo.value = 1
        self.port.fixture_frame(1)
        self.assertEqual((aspect.value, view(700.0)), (1.0, 700.0))
        self.assertEqual(self.value(ctypes.c_int, 'port_pillarbox').value, 1)
        self.value(ctypes.c_float, 'port_wide_anchor').value = WIDE_LEFT
        self.value(ctypes.c_int, 'port_wide_cover').value = 1
        self.assertEqual(self.port.port_wide_x(0.0), 80.0)
        self.value(ctypes.c_float, 'port_wide_anchor').value = 0.0
        self.value(ctypes.c_int, 'port_wide_cover').value = 0
        # the frame's end draws the bands over the picture's sides, and they keep x
        self.value(ctypes.c_int, 'fixture_bands').value = 0
        self.port.fixture_frame(1)
        self.assertEqual(list((ctypes.c_float * 8).in_dll(self.port, 'fixture_band_x'))[:4], [0.0, 80.0, 560.0, 640.0])
        self.assertEqual(self.value(ctypes.c_int, 'fixture_band_cover').value, 1)
        self.assertEqual(self.value(ctypes.c_int, 'port_wide_cover').value, 0)
        # the bands draw with no fog, and the game's fog mode stands after them
        for mode in (KM_FOGTABLE, KM_NOFOG):
            self.port.fixture_set_fog(mode)
            self.port.fixture_frame(1)
            self.assertEqual(self.value(ctypes.c_int, 'fixture_band_fog').value, KM_NOFOG)
            self.assertEqual(self.port.fixture_fog(), mode)
        demo.value = 0
        self.port.fixture_frame(1)
        self.assertEqual((aspect.value, self.value(ctypes.c_int, 'port_pillarbox').value), (0.75, 0))

    def test_a_polygon_covers_from_edge_to_edge(self):
        covers = self.port.port_wide_covers
        covers.argtypes = [ctypes.POINTER(ctypes.c_float), ctypes.c_int, ctypes.c_int]

        def spans(*xs):
            corners = (ctypes.c_float * (2 * len(xs)))(*(v for x in xs for v in (x, 0.0)))
            return bool(covers(corners, len(xs), 2))

        self.assertTrue(spans(0.0, 640.0, 640.0, 0.0))
        self.assertTrue(spans(-1.0, 641.0, 640.5, 0.5))
        self.assertFalse(spans(0.0, 0.0, 1.0, 1.0))
        self.assertFalse(spans(0.0, 320.0, 640.0, 0.0))
        self.assertFalse(spans(16.0, 624.0, 624.0, 16.0))

    def test_a_sprite_squeezes_about_its_point(self):
        draw = self.port.wide_draw_sprite_2d
        draw.argtypes = [ctypes.POINTER(Sprite), ctypes.c_int, ctypes.c_float, ctypes.c_uint32]
        cell = Cell(32, 16, 16, 8, 0, 0, 255, 255, 0, 0)
        sprite = Sprite((640.0, 100.0, 0.0), 2.0, 2.0, 0, None, ctypes.pointer(cell))
        strip = (Vertex * 4).in_dll(self.port, 'fixture_strip')

        def corners():
            return [(round(vertex.x, 3), round(vertex.y, 3)) for vertex in strip]

        # outside the squeeze Ninja's own draw takes the sprite as it stands
        draw(sprite, 0, -1.0, 0)
        drawn = Sprite.in_dll(self.port, 'fixture_sprite')
        self.assertEqual((drawn.p[0], drawn.sx, strip[0].x), (640.0, 2.0, 0.0))
        self.toggles[WIDE] = 1
        self.port.fixture_frame(1)
        draw(sprite, 0, -2.0, 0)
        self.assertEqual(corners(), [(536.0, 84.0), (536.0, 116.0), (584.0, 84.0), (584.0, 116.0)])
        self.assertEqual([strip[i].pcw for i in (0, 3)], [0xe0000000, 0xf0000000])
        self.assertEqual((strip[0].z, strip[0].base, strip[3].u), (0.5, 1.0, 255 / 256))
        # NJD_SPRITE_ANGLE turns the cell, and then its corners squeeze: a quarter turn swaps its sides
        sprite.ang = 0x4000
        draw(sprite, 0, -1.0, 1)
        self.assertEqual(corners(), [(572.0, 68.0), (548.0, 68.0), (572.0, 132.0), (548.0, 132.0)])
        # NJD_SPRITE_HFLIP swaps the cell's u
        draw(sprite, 0, -1.0, 4)
        self.assertEqual((strip[0].u, strip[3].u), (255 / 256, 0.0))

    def test_the_game_projects_into_the_4_3_picture(self):
        project = self.port.wide_project_screen
        point = (ctypes.c_float * 3)(100.0, 0.0, 1.0)
        out = (ctypes.c_float * 2)()
        self.toggles[WIDE] = 1
        self.port.fixture_frame(1)
        project(None, point, out)
        self.assertEqual(self.value(ctypes.c_float, 'fixture_projected_aspect').value, 1.0)
        self.assertEqual(self.value(ctypes.c_float, '_nj_scr_aspect_x_').value, 0.75)
        # the squeeze puts the 2D draw where the picture shows the point
        self.assertEqual((out[0], self.port.port_wide_x(out[0])), (420.0, 395.0))


def matching_side(text):
    """text less what its NONMATCHING guards give make dc alone."""
    kept, depth, skipping = [], 0, None
    for line in text.splitlines():
        word = line.split()[0] if line.split() else ''
        if word in ('#ifdef', '#ifndef', '#if'):
            depth += 1
            if skipping is None and line.split()[:2] == ['#ifdef', 'NONMATCHING']:
                skipping = depth
                continue
        elif word == '#else' and skipping == depth:
            skipping = -depth
            continue
        elif word == '#endif':
            depth -= 1
            if skipping is not None and abs(skipping) == depth + 1:
                skipping = None
                continue
        if skipping is None or skipping < 0:
            kept.append(line)
    return '\n'.join(kept)


def uncommented(text):
    return re.sub(r'/\*.*?\*/', '', text, flags=re.S)


def functions(text):
    """Each function body in text, as (name, body), by the tree's layout: a
    definition's line ends its parameters, and braces in column 0 hold it."""
    return [(found.group(1), found.group(2))
            for found in re.finditer(r'^[^\s#/}][^\n;]*?\b(\w+)\([^;{]*\)\n\{\n(.*?)^\}', text, re.M | re.S)]


class Rules(unittest.TestCase):
    def sources(self, *roots):
        for root in roots:
            for path in sorted((ROOT / root).rglob('*.[ch]')):
                yield path.relative_to(ROOT).as_posix(), path.read_text(encoding='utf-8', errors='replace')

    def test_the_matching_side_names_nothing_of_the_port(self):
        header = uncommented((ROOT / 'engine/dc/port.h').read_text())
        names = set(re.findall(r'\b((?:port|wide|readout|present)_\w+|WIDE_\w+|TOGGLE\w*|READOUT\w*)\b', header))
        names |= {'frame_end', 'frame_begin', 'camera_draw_restart'}
        self.assertIn('port_wide_anchor', names)
        named = re.compile(r'\b(' + '|'.join(sorted(names)) + r')\b')
        for name, text in self.sources(*GAME):
            self.assertIsNone(named.search(uncommented(matching_side(text))),
                              f'{name}: the matching side names the port')

    def test_only_the_port_sets_the_x_aspect(self):
        writers = {name for name, text in self.sources(*GAME, 'engine')
                   if re.search(r'\b_nj_scr_aspect_x_\s*=[^=]', uncommented(text))}
        self.assertEqual(writers, ASPECT_WRITERS)

    def test_only_the_four_tests_read_the_window(self):
        readers = {}
        for name, text in self.sources(*GAME, 'engine'):
            if name.endswith('.c') and re.search(r'\bport_view_(aspect|x)\b', uncommented(text)):
                readers[name] = [function for function, body in functions(uncommented(text))
                                 if re.search(r'\bport_view_(aspect|x)\b', body)]
        self.assertEqual(readers.keys(), WINDOW_READERS.keys())
        for name, function in WINDOW_READERS.items():
            if function:
                self.assertEqual(readers[name], [function])

    def test_a_function_puts_back_the_pivot_and_the_cover(self):
        for name, text in self.sources(*GAME, 'engine'):
            for function, body in functions(text):
                for variable, rest in PUT_BACK.items():
                    sets = re.findall(rf'\b{variable}\s*=\s*([^;=][^;]*);', body)
                    if sets:
                        self.assertEqual(sets[-1].strip(), rest, f'{name}: {function} leaves {variable} set')
                        after = body[body.rindex(f'{variable} = {rest};'):]
                        self.assertIsNone(re.search(rf'\b{variable}\s*=(?!\s*{rest};)', after))


if __name__ == '__main__':
    unittest.main()
