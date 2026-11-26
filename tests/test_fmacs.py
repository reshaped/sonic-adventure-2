"""tools/fmacs.py: each FMAC SHC writes in the tree's compiled C, and its counts by route, by directory and by the
bin config/parts.txt gives each source."""

import os
import unittest
from unittest import mock

import fmacs
from support import TreeTest, run

COMPILERS = """ROUTES := game ninja
game.release := R13
game.sources := $(shell find src modules -name '*.c')
ninja.release := R11
ninja.sources := $(shell find sdk/ninja -name '*.c')
"""
# the bins: the game's code is play's, but for its draws, and Ninja's is picture's, but for a math member
PARTS = """src/  play
src/draw/  picture
sdk/ninja/  picture
sdk/ninja/njTurn.c  play
modules/  play
"""
# SHC's text as an image's addresses lead it, and as -code=asmcode writes it
PLAYER = """           _player_step:                    ; function: player_step
                                            ; frame size=0
0x8c06c9d4           FLDI1       FR3
0x8c06c9d6           FMAC        FR0,FR3,FR12
           L12:
0x8c06c9d8           FMAC        FR0,FR2,FR4 ; a comment
           _player_draw:                    ; function: player_draw
0x8c06c9e0           FMUL        FR2,FR0
"""
DRAW = """          .EXPORT     _draw_glow
_draw_glow:                      ; function: draw_glow
          FMAC        FR0,FR5,FR3
          RTS
          NOP
"""
# R13 writes a complete inline body followed by its own alignment. Only the
# FMOV in this body differs from generated instruction indentation. The
# handwritten FMAC has the same text as a compiler-generated instruction.
HAND = "        FMOV.S      FR5,FR0\n          FMAC        FR0,FR4,FR2\n"
ALIGN = "          .ALIGN      4\n"
BODIED = """#pragma inline_asm(turn)
static void turn(void)
{
""" + HAND + """}

void ninja_turn(void)
{
    turn();
}
"""
BODIED_TEXT = ("_ninja_turn:                     ; function: ninja_turn\n" +
               HAND + ALIGN + "          FMAC        FR0,FR4,FR2\n")


class Listing(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('config/compilers.mk', COMPILERS)
        self.write('config/parts.txt', PARTS)
        for path in ('src/player/player.c', 'src/draw/glow.c', 'sdk/ninja/njTurn.c', 'modules/shared/rock.c'):
            self.write(path, BODIED if path.endswith('njTurn.c') else 'void f(void) {}\n')
        self.write('lst/src/player/player.src', PLAYER)
        self.write('lst/src/draw/glow.src', DRAW)
        self.write('lst/sdk/ninja/njTurn.src', BODIED_TEXT)

    @mock.patch.dict(os.environ, {'HUNG': '0.3s'})
    def test_reports_a_stalled_compiler_route_lookup(self):
        self.write('config/compilers.mk', '$(shell sleep 5)\n' + COMPILERS)
        self.assertIn('timed out', self.fails(fmacs.routes))

    def test_rejects_duplicate_and_missing_routes_even_without_a_listing(self):
        path = 'modules/shared/rock.c'
        for config, error in [(f'ninja.sources += {path}', 'listed more than once in routes game ninja'),
                              ('ninja.sources += modules/shared/../shared/rock.c', 'listed more than once in routes game ninja'),
                              (f'game.sources := $(filter-out {path},$(game.sources))', 'no matching compiler route')]:
            with self.subTest(error=error):
                self.write('config/compilers.mk', COMPILERS + config + '\n')
                status, out = run(fmacs.main, 'lst')
                self.assertNotEqual(status, 0, out)
                self.assertIn(f'{path}: {error}', out)

    def test_reads_routes_past_a_make_warning_and_a_file_named_routes(self):
        expected = fmacs.routes()
        self.write('routes', '')
        self.write('config/compilers.mk', '$(warning old route data)\n' + COMPILERS)
        self.assertEqual(fmacs.routes(), expected)

    def test_counts_a_source_at_a_top_under_its_top(self):
        self.write('src/main.c', 'void main(void) {}\n')
        self.write('lst/src/main.src', '_main: ; function: main\n          FMAC        FR0,FR1,FR2\n')
        status, out = run(fmacs.main, 'lst')
        self.assertEqual(status, 0, out)
        self.assertIn('  src: 1 in 1', out.splitlines())

    def test_lists_each_fmac_and_counts_them(self):
        status, out = run(fmacs.main, 'lst')
        self.assertEqual(status, 0, out)
        self.assertEqual(out.splitlines(), [
            'sdk/ninja/njTurn.c: ninja_turn: FMAC FR0,FR4,FR2',
            'src/draw/glow.c: draw_glow: FMAC FR0,FR5,FR3',
            'src/player/player.c: player_step 0x8c06c9d6: FMAC FR0,FR3,FR12',
            'src/player/player.c: player_step 0x8c06c9d8: FMAC FR0,FR2,FR4',
            '4 FMACs in 3 of 3 sources listed; 1 sources unlisted',
            'by route',
            '  game R13: 3 in 2',
            '  ninja R11: 1 in 1',
            'by directory',
            '  sdk/ninja: 1 in 1',
            '  src/draw: 1 in 1',
            '  src/player: 2 in 1',
            'by bin',
            '  picture: 1 in 1',
            '  play: 3 in 2',
        ])

    def test_attributes_complete_copies_including_repeated_calls_and_the_named_body(self):
        routine = '_turn:  ; function: turn\n' + HAND + ALIGN
        calls = BODIED_TEXT + HAND + ALIGN
        unrelated = '_unrelated:  ; function: unrelated\n          FMAC        FR0,FR4,FR2\n'
        self.assertEqual(fmacs.fused(routine + calls + unrelated, BODIED, 'a.src'), [
            ('ninja_turn', None, 'FR0,FR4,FR2'), ('unrelated', None, 'FR0,FR4,FR2')])

    def test_keeps_addresses_when_matching_a_body_and_does_not_take_a_comment_for_a_function(self):
        listing = """_ninja_turn: ; function: ninja_turn
0x8C000002         FMOV.S      FR5,FR0
0x8C000004           FMAC        FR0,FR4,FR2
          .ALIGN      4
          ; FMAC FR0,FR4,FR2
0x8C000008           FMAC        FR0,FR4,FR2 ; function: fake
"""
        self.assertEqual(fmacs.fused(listing, BODIED, 'a.src'), [('ninja_turn', '8C000008', 'FR0,FR4,FR2')])

    def test_compiler_shaped_expansion_is_ambiguous_in_c_but_known_in_its_named_body(self):
        body = HAND.replace('        FMOV', '          FMOV')
        source = BODIED.replace(HAND, body)
        self.assertEqual(fmacs.fused('_turn: ; function: turn\n' + body + ALIGN, source, 'a.src'), [])
        status, message = run(lambda _: fmacs.fused(BODIED_TEXT.replace(HAND, body), source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('a.src:3: ambiguous FMAC: inline body turn in ninja_turn', message)

    def test_rejects_a_distinctive_body_without_its_alignment_in_a_mixed_or_named_function(self):
        for host in ('ninja_turn', 'turn'):
            with self.subTest(host=host):
                listing = f'_{host}: ; function: {host}\n' + HAND
                status, message = run(lambda _: fmacs.fused(listing, BODIED, 'a.src'))
                self.assertEqual(status, 1)
                self.assertIn('incomplete inline body turn', message)

    def test_rejects_a_listing_ending_inside_a_distinctive_expansion(self):
        source = BODIED.replace(HAND, HAND + '        FMOV.S      FR2,FR0\n')
        listing = '_ninja_turn: ; function: ninja_turn\n' + HAND
        status, message = run(lambda _: fmacs.fused(listing, source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('incomplete inline body turn', message)

    def test_a_shorter_body_opening_a_longer_expansion_hides_none_of_its_fmacs(self):
        first, second = '        FMAC        FR0,FR1,FR2\n', '        FMAC        FR0,FR3,FR4\n'
        half = '#pragma inline_asm(half)\nstatic void half(void)\n{\n' + first + '}\n'
        whole = '#pragma inline_asm(whole)\nstatic void whole(void)\n{\n' + first + ALIGN + second + '}\n'
        listing = '_caller: ; function: caller\n' + first + ALIGN + second + ALIGN
        for order, source in (('half first', half + whole), ('whole first', whole + half)):
            with self.subTest(order=order):
                self.assertEqual(fmacs.fused(listing, source, 'a.src'), [])

    def test_identical_compiler_shaped_named_bodies_each_account_for_their_own(self):
        body = '          FMAC        FR0,FR4,FR2\n'
        source = ''.join(f'#pragma inline_asm({name})\nstatic void {name}(void)\n{{\n' + body + '}\n'
                         for name in ('a', 'b'))
        listing = '_a: ; function: a\n' + body + ALIGN + '_b: ; function: b\n' + body + ALIGN
        self.assertEqual(fmacs.fused(listing, source, 'a.src'), [])

    def test_rejects_a_distinctive_expansion_interrupted_by_another_line(self):
        source = BODIED.replace(HAND, HAND.replace('FMOV.S      FR5,FR0', 'FMAC        FR0,FR5,FR0'))
        listing = '_ninja_turn: ; function: ninja_turn\n        FMAC        FR0,FR5,FR0\n          NOP\n'
        status, message = run(lambda _: fmacs.fused(listing, source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('a.src:2: incomplete inline body turn', message)

    def test_rejects_an_unaccounted_fmac_under_a_named_body_without_one(self):
        source = BODIED.replace(HAND, '        FMOV.S      FR5,FR0\n')
        listing = '_turn: ; function: turn\n          FMAC        FR0,FR4,FR2\n'
        status, message = run(lambda _: fmacs.fused(listing, source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('no complete assembly body accounts for the FMAC in turn', message)

    def test_a_section_after_a_tab_or_a_space_ends_the_function(self):
        for section in ('.SECTION P,CODE', '.SECTION\tP,CODE'):
            with self.subTest(section=section):
                listing = f'_f: ; function: f\n          {section}\n          FMAC        FR0,FR1,FR2\n'
                status, message = run(lambda _: fmacs.fused(listing, '', 'a.src'))
                self.assertEqual(status, 1)
                self.assertIn('an FMAC under no function', message)

    def test_rejects_an_unrecognized_inline_pragma(self):
        source = BODIED.replace('#pragma', '# pragma')
        status, message = run(lambda _: fmacs.fused(BODIED_TEXT, source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('unsupported pragma spelling', message)

    def test_recognizes_complete_raw_assembly_blocks_and_rejects_duplicate_candidates(self):
        raw = '          FMAC        FR0,FR4,FR2\n'
        source = '#pragma asm\n' + raw + '#pragma endasm\n'
        self.assertEqual(fmacs.fused(raw, source, 'a.src'), [])
        status, message = run(lambda _: fmacs.fused(raw * 2, source, 'a.src'))
        self.assertEqual(status, 1)
        self.assertIn('ambiguous #pragma asm FMAC', message)

    def test_attaches_wrapped_annotations_only_to_labels(self):
        listing = ('_a_very_long_function_label:\n          ; function: a_very_long_function_label\n'
                   '          ; function: ignored\n          FMAC        FR0,FR1,FR2\n')
        self.assertEqual(fmacs.fused(listing, '', 'a.src'), [('a_very_long_function_label', None, 'FR0,FR1,FR2')])

    def test_fails_on_an_fmac_under_no_function_and_a_directory_with_no_text(self):
        self.write('lst/src/draw/glow.src', '          FMAC        FR0,FR5,FR3\n')
        status, out = run(fmacs.main, 'lst')
        self.assertEqual((status, out.split(': ', 1)[1]), (1, 'lst/src/draw/glow.src: an FMAC under no function'))
        status, out = run(fmacs.main, 'empty')
        self.assertEqual((status, out.split(': ', 1)[1]), (1, 'empty: no text of a source'))

    def test_fails_on_a_listed_source_without_a_bin(self):
        self.write('config/parts.txt', PARTS.replace('src/  play\n', ''))
        status, out = run(fmacs.main, 'lst')
        self.assertEqual((status, out.split(': ', 1)[1]),
                         (1, 'src/player/player.c: no row of config/parts.txt gives its bin'))


if __name__ == '__main__':
    unittest.main()
