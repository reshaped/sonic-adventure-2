"""Chunk lists that a struct of their chunks lays out, written as flat arrays."""

from pathlib import Path

import flatten
from support import TreeTest, run

TYPES = """\
#ifndef SG_XPT_H
#define SG_XPT_H
typedef long Sint32;
typedef short Sint16;
typedef unsigned short Uint16;
typedef unsigned long Uint32;
typedef struct { Sint32 *vlist; Sint16 *plist; float center[3]; float r; } NJS_CNK_MODEL;
#endif
"""
CHUNK = """\
#include <sg_xpt.h>
#define CHUNK_HEAD_WORDS 2
#define CHUNK_HEAD(words) ((words) - CHUNK_HEAD_WORDS)
#define CHUNK_UNUSED 0
struct chunk_vertex_d8 {
    float x, y, z;
    Uint32 color;
};
"""
CHUNK_POLYGON = """\
#include <sg_xpt.h>
struct part_texture {
    Uint16 head, texture;
};
"""
# a polygon list, a vertex list a typedef names, a model, and a pose record that holds a polygon list and a model
# that reaches it
FIXTURE = """\
#include <sg_xpt.h>

#include "draw/chunk.h"
#include "draw/chunk_polygon.h"

struct box_polygons_list {
    struct {
        Uint16 head, words;
    } material1;
    struct part_texture texture1;
    Uint16 end;
};

static struct box_polygons_list box_polygons = {
#include "stg/fixture/box_polygons.inc"
};

typedef struct {
    Sint32 head[2];
    struct chunk_vertex_d8 vertices[1];
    Sint32 end;
} box_vertices_list;

static box_vertices_list box_vertices = {
#include "stg/fixture/box_vertices.inc"
};

static NJS_CNK_MODEL box_model = {
#include "stg/fixture/box_model.inc"
};

struct box_pose {
    NJS_CNK_MODEL mesh;
    struct box_polygons_list polygons;
};

static struct box_pose box_poses[2] = {
#include "stg/fixture/box_poses.inc"
};
"""
FLAT = """\
#include <sg_xpt.h>

static Sint16 box_polygons[5] = {
#include "stg/fixture/box_polygons.inc"
};

static Sint32 box_vertices[7] = {
#include "stg/fixture/box_vertices.inc"
};

static NJS_CNK_MODEL box_model = {
#include "stg/fixture/box_model.inc"
};

struct box_pose {
    NJS_CNK_MODEL mesh;
    Sint16 polygons[5];
};

static struct box_pose box_poses[2] = {
#include "stg/fixture/box_poses.inc"
};
"""
SOURCE = """\
#include "draw/chunk.h"
#include "stg/fixture.h"

NJS_CNK_MODEL *box_mesh(int pose)
{
    return pose < CHUNK_HEAD(0) ? &box_model : &box_poses[pose].mesh;
}
"""
# each pose's model reaches the polygons beside it: a pose takes 36 bytes, and its polygons start 24 into it
BINDINGS = """\
pointer box_model 0x00000000 box_vertices
pointer box_model 0x00000004 box_polygons
pointer box_poses 0x00000004 box_poses+0x18
pointer box_poses 0x00000028 box_poses+0x3c
"""
SPLITS = """\
file SONIC2/STG.PRS
D 0x8c500000 0x8c500100
modules/stg/box.c D 0x8c500000 0x8c500100
"""
FILES = ['modules/stg/fixture.h', 'modules/stg/box.c']


class Flatten(TreeTest):
    def setUp(self):
        super().setUp()
        self.image('stg', SPLITS)
        self.write('sdk/include/sg_xpt.h', TYPES)
        self.write('src/draw/chunk.h', CHUNK)
        self.write('src/draw/chunk_polygon.h', CHUNK_POLYGON)
        self.write('modules/stg/fixture.h', FIXTURE)
        self.write('modules/stg/box.c', SOURCE)
        self.write('offsets/modules/stg/box.bindings', BINDINGS)

    def dry_run(self, *files):
        status, output = run(flatten.main, '--dry-run', *(files or FILES))
        self.assertEqual(status, 0, output)
        return output.splitlines()

    def test_prints_each_list_type_with_its_flat_array_and_each_unnamed_helper(self):
        self.assertEqual(self.dry_run(),
                         ['modules/stg/fixture.h:6: struct box_polygons_list: Sint16[5] box_polygons box_poses',
                          'modules/stg/fixture.h:18: box_vertices_list: Sint32[7] box_vertices',
                          '2 types flatten and 0 stay',
                          'no file names CHUNK_UNUSED'])
        self.assertEqual(Path('modules/stg/fixture.h').read_text(), FIXTURE)

    def test_writes_each_list_flat_wherever_its_type_stands(self):
        self.assertEqual(run(flatten.main, *FILES), (0, ''))
        # the record keeps its struct, and the part headers, which nothing names now, go
        self.assertEqual(Path('modules/stg/fixture.h').read_text(), FLAT)
        self.assertEqual(Path('modules/stg/box.c').read_text(), SOURCE)

    def test_writes_an_extern_of_a_list_that_data_txt_lists(self):
        self.write('config/stg/data.txt', '0x8c500100 shared_polygons chunk_polygons 5\n')
        self.write('modules/stg/fixture.h', FIXTURE + 'extern struct box_polygons_list shared_polygons;\n')
        self.assertEqual(run(flatten.main, *FILES), (0, ''))
        self.assertEqual(Path('modules/stg/fixture.h').read_text(), FLAT + 'extern Sint16 shared_polygons[5];\n')

    def test_keeps_a_type_whose_list_code_names(self):
        self.write('modules/stg/box.c', SOURCE + 'Uint16 box_end(void)\n{\n    return box_polygons.end;\n}\n')
        self.assertEqual(self.dry_run()[:2],
                         ['modules/stg/fixture.h:18: box_vertices_list: Sint32[7] box_vertices',
                          'modules/stg/fixture.h:6: struct box_polygons_list: stays: code names box_polygons'])

    def test_keeps_a_type_with_an_object_no_list_field_reaches(self):
        self.write('modules/stg/fixture.h', FIXTURE + 'static struct box_polygons_list spare_polygons = {\n'
                   '#include "stg/fixture/spare_polygons.inc"\n};\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: no vlist or plist reaches '
                      'spare_polygons', self.dry_run())

    def test_keeps_a_type_a_file_outside_the_files_names(self):
        self.assertEqual(self.dry_run('modules/stg/box.c')[:2],
                         ['modules/stg/fixture.h:6: struct box_polygons_list: stays: named outside the files, in '
                          'modules/stg/fixture.h',
                          'modules/stg/fixture.h:18: box_vertices_list: stays: named outside the files, in '
                          'modules/stg/fixture.h'])

    def test_keeps_a_type_named_where_no_declaration_of_it_is(self):
        self.write('modules/stg/box.c', SOURCE + 'int box_size(void)\n{\n'
                   '    return sizeof(struct box_polygons_list);\n}\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: modules/stg/box.c: line 10: the '
                      'type, where no declaration of it is', self.dry_run())

    def test_fails_on_a_type_both_kinds_of_list_reach(self):
        self.write('offsets/modules/stg/box.bindings', BINDINGS.replace('box_model 0x00000000 box_vertices',
                                                                        'box_model 0x00000000 box_polygons'))
        status, output = run(flatten.main, '--dry-run', *FILES)
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'struct box_polygons_list: both a vertex list and a polygon list reach it'))

    def test_keeps_a_type_whose_object_has_an_initializer_the_build_does_not_generate(self):
        # the halves of a brace initializer land in a flat array's words, so its bytes would differ
        self.write('modules/stg/fixture.h',
                   FIXTURE.replace('{\n#include "stg/fixture/box_polygons.inc"\n}', '{{1, 2}}'))
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: the build generates no initializer '
                      'of box_polygons', self.dry_run())

    def test_writes_a_declaration_that_follows_a_function_body(self):
        model = 'static NJS_CNK_MODEL spare_model = {\n#include "stg/fixture/spare_model.inc"\n};\n'
        self.write('offsets/modules/stg/box.bindings', BINDINGS + 'pointer spare_model 0x00000000 spare_vertices\n')
        self.write('modules/stg/fixture.h',
                   FIXTURE + 'static void box_idle(void) {}\nbox_vertices_list spare_vertices;\n' + model)
        self.assertEqual(run(flatten.main, *FILES), (0, ''))
        self.assertEqual(Path('modules/stg/fixture.h').read_text(),
                         FLAT + 'static void box_idle(void) {}\nSint32 spare_vertices[7];\n' + model)

    def test_keeps_a_type_whose_typedef_names_another_type(self):
        self.write('modules/stg/fixture.h', FIXTURE.replace('} box_vertices_list;', '} box_vertices_list, spare_list;'))
        self.assertIn('modules/stg/fixture.h:0: box_vertices_list: stays: modules/stg/fixture.h: line 22: a typedef of '
                      'the type that names more than the type', self.dry_run())

    def test_removes_a_definition_alone_and_leaves_the_rest_of_the_file(self):
        self.write('modules/stg/fixture.h', FIXTURE.replace('} box_vertices_list;', '} box_vertices_list; int keep;')
                   + '\n\n\nint box_tail;\n')
        self.assertEqual(run(flatten.main, *FILES), (0, ''))
        self.assertEqual(Path('modules/stg/fixture.h').read_text(),
                         FLAT.replace('\n\nstatic Sint32 box_vertices', '\n\n int keep;\n\nstatic Sint32 box_vertices')
                         + '\n\n\nint box_tail;\n')

    def test_keeps_a_type_whose_fields_code_reads_through_a_record(self):
        self.write('modules/stg/box.c', SOURCE + 'Uint16 box_pose_end(int pose)\n{\n'
                   '    return box_poses[pose].polygons.end;\n}\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: code reads a field through a member '
                      'that holds it, at modules/stg/box.c:10', self.dry_run())

    def test_keeps_a_type_with_a_declaration_no_read_finds(self):
        # spare.c is one of the files, but no image places it, so nothing says what reaches its object
        self.write('modules/stg/spare.c', '#include "stg/fixture.h"\nstatic struct box_polygons_list spare_polygons;\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: modules/stg/spare.c: line 2: no read '
                      'finds spare_polygons', self.dry_run(*FILES, 'modules/stg/spare.c'))

    def test_keeps_a_type_that_two_definitions_lay_out_differently(self):
        self.image('stg', SPLITS.replace('0x8c500100\n', '0x8c500200\n', 1)
                   + 'modules/stg/other.c D 0x8c500100 0x8c500200\n')
        self.write('modules/stg/other.c', '#include <sg_xpt.h>\n#include "draw/chunk_polygon.h"\n'
                   'struct box_polygons_list {\n    struct part_texture texture1;\n    Uint16 end;\n};\n'
                   'static struct box_polygons_list other_polygons = {\n#include "stg/other/other_polygons.inc"\n};\n'
                   'static NJS_CNK_MODEL other_model = {\n#include "stg/other/other_model.inc"\n};\n')
        self.write('offsets/modules/stg/other.bindings', 'pointer other_model 0x00000004 other_polygons\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: its definitions lay it out as 6 bytes '
                      'aligned to 2 and 10 bytes aligned to 2', self.dry_run(*FILES, 'modules/stg/other.c'))

    def test_credits_an_extern_row_only_to_the_list_at_its_address(self):
        # the pose's polygons start 24 bytes in, where the row doesn't point
        self.write('config/stg/data.txt', '0x8c500100 shared_pose chunk_polygons 5\n')
        self.write('modules/stg/fixture.h', FIXTURE + 'extern struct box_pose shared_pose;\n')
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: stays: no vlist or plist reaches shared_pose',
                      self.dry_run())

    def test_takes_no_reach_from_an_address_past_an_array(self):
        # box_poses+0x60 would be the polygons of a third pose, which box_poses doesn't hold
        self.write('modules/stg/fixture.h', FIXTURE + 'static NJS_CNK_MODEL spare_model = {\n'
                   '#include "stg/fixture/spare_model.inc"\n};\n')
        self.write('offsets/modules/stg/box.bindings', BINDINGS + 'pointer spare_model 0x00000000 box_poses+0x60\n'
                   'pointer spare_model 0x00000004 box_poses+0x60\n')
        self.assertEqual(self.dry_run()[0], 'modules/stg/fixture.h:6: struct box_polygons_list: Sint16[5] box_polygons '
                         'box_poses')

    def test_fails_on_a_type_whose_two_objects_two_kinds_reach(self):
        self.write('config/stg/data.txt', '0x8c500100 shared_polygons chunk_vertices 5\n')
        self.write('modules/stg/fixture.h', FIXTURE + 'extern struct box_polygons_list shared_polygons;\n')
        status, output = run(flatten.main, '--dry-run', *FILES)
        self.assertEqual((status, output.splitlines()[-1].partition(': ')[2]),
                         (1, 'struct box_polygons_list: both a vertex list and a polygon list reach it'))

    def test_keeps_a_type_named_between_two_strings_that_spell_a_comment(self):
        self.write('modules/stg/names.c', 'const char *a = "/*";\nextern box_vertices_list shared_vertices;\n'
                   'const char *b = "*/";\n/* struct box_polygons_list */\n')
        output = self.dry_run()
        self.assertIn('modules/stg/fixture.h:6: struct box_polygons_list: Sint16[5] box_polygons box_poses', output)
        self.assertTrue(any(line.startswith('modules/stg/fixture.h:18: box_vertices_list: stays:') for line in output),
                        output)

    def test_keeps_a_part_header_whose_option_macro_a_conditional_names(self):
        self.write('src/draw/chunk.h', '#ifndef DRAW_CHUNK_H\n#define DRAW_CHUNK_H\n' + CHUNK
                   + '#define CHUNK_OPTION\n#endif\n')
        self.write('modules/stg/fixture.h', FIXTURE + '#ifdef CHUNK_OPTION\n#endif\n')
        self.assertEqual(run(flatten.main, *FILES), (0, ''))
        self.assertEqual(Path('modules/stg/fixture.h').read_text(),
                         FLAT.replace('#include <sg_xpt.h>\n\n',
                                      '#include <sg_xpt.h>\n\n#include "draw/chunk.h"\n\n', 1)
                         + '#ifdef CHUNK_OPTION\n#endif\n')

    def test_decides_the_same_for_an_absolute_path(self):
        self.assertEqual(self.dry_run(*(str(Path(path).resolve()) for path in FILES)), self.dry_run())

    def test_fails_on_a_file_that_is_not_there(self):
        self.assertEqual(run(flatten.main, '--dry-run', 'modules/stg/none.h')[0], 1)

    def test_counts_a_name_one_header_uses_in_the_other_header(self):
        self.write('src/draw/chunk.h', CHUNK.replace('- CHUNK_HEAD_WORDS)', '- CHUNK_HEAD_WORDS - PART_BASE)'))
        self.write('src/draw/chunk_polygon.h', CHUNK_POLYGON + '#define PART_BASE 0\n')
        self.assertEqual(self.dry_run()[-1], 'no file names CHUNK_UNUSED')
