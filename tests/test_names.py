"""The names check: the definitions and uses that justify a symbols.txt row, and the numbers a placeholder names."""

import names
from support import TreeTest

MAIN = """\
file 1ST_READ.BIN
P 0x8c010000 0x8c010100
D 0x8c010100 0x8c010200
src/a.c P 0x8c010000 0x8c010040
src/a.c D 0x8c010100 0x8c010110
"""
STAGE = """\
file SONIC2/STG13.PRS
P 0x8c500000 0x8c500100
modules/b.c P 0x8c500000 0x8c500040
"""
# what each object defines, and what it takes from outside
OBJECTS = {'build/obj/src/a.obj': ({'_a_start': ('P', 0)}, {'_outside'}),
           'build/obj/modules/b.obj': ({'_b_start': ('P', 0)}, {'_shared', '_local'})}


class Names(TreeTest):
    def unused(self, main, stage):
        self.write('src/a.c', '')
        self.write('modules/b.c', '')
        self.image('1st_read', MAIN, main)
        self.image('stg13', STAGE, stage)
        return names.unused(lambda path: OBJECTS[str(path)])

    def test_a_row_is_defined_taken_or_code_of_a_source(self):
        main = ('0x8c010000 a_start\n0x8c010020 a_static\n0x8c010080 shared\n0x8c010090 outside\n')
        self.assertEqual(self.unused(main, '0x8c500000 b_start\n0x8c500080 local\n'), [])

    def test_packed_motion_exports_its_header_and_private_data_owns_no_public_name(self):
        self.write('src/a.c', '')
        self.image('1st_read', MAIN + 'disc D 0x8c010110 0x8c010138\n',
                   '0x8c010120 unused_motion\n0x8c01012c private_points\n')
        self.write('config/1st_read/data.txt', '0x8c010110 unused_motion ninja_motion 28\n'
                                              '0x8c01012c .private_points points 1\n')
        self.assertEqual(names.unused(lambda path: OBJECTS[str(path)]),
                         [('1st_read', 0x8c01012c, 'private_points')])

    def test_a_name_the_data_table_declares_or_imports_is_the_gnu_builds_to_check(self):
        self.image('chao', 'file CHAO.BIN\nD 0x0c500000 0x0c500034\ndisc D 0x0c500000 0x0c500034\n',
                   '0x0c500000 toy\n')
        self.write('config/chao/data.txt', '0x0c500000 toy chunk_object - - -\n')
        self.image('chaostgrace', 'file RACE.BIN\nD 0x0c900000 0x0c900004\ndisc D 0x0c900000 0x0c900004\n',
                   '0x0c500000 toy\n0x0c500000 stale_alias\n0x0c900000 rewards\n')
        path = 'config/chaostgrace/data.txt'
        imported = 'import chao toy\n'
        stale = ('chaostgrace', 0x0c500000, 'stale_alias')
        toy = ('chaostgrace', 0x0c500000, 'toy')
        # only the names are read: a table that can't take the import, a kind no tool knows, or no kind at all
        # is tools/content.py's to refuse
        for rows in ('0x0c900000 rewards chunk_object_ptrs 1\n', '0x0c900000 rewards chunk_model_ptrs 1\n',
                     '0x0c900000 rewards no_such_kind\n', '0x0c900000 rewards\n'):
            with self.subTest(rows=rows):
                self.write(path, rows + imported)
                self.assertEqual(names.unused(), [stale])
        self.write(path, '0x0c900000 rewards chunk_object_ptrs 1\n')
        self.assertEqual(names.unused(), [toy, stale])

    def test_any_other_row_is_unused(self):
        main = '0x8c010000 a_start\n0x8c010090 stale\n0x8c010104 a_static_datum\n'
        self.assertEqual(self.unused(main, '0x8c500000 b_start\n0x8c500080 outside\n'),
                         [('1st_read', 0x8c010090, 'stale'), ('1st_read', 0x8c010104, 'a_static_datum'),
                          ('stg13', 0x8c500080, 'outside')])


class Placeholders(TreeTest):
    def test_a_placeholder_names_its_own_address_or_offset(self):
        self.write('src/a.c', '')
        self.write('modules/b.c', '')
        self.image('1st_read', MAIN, '0x8c010020 fun_8c010020\n0x8c010104 dat_8c010100\n')
        self.image('stg13', STAGE, '0x8c500008 fun_b_0008\n0x8c500010 fun_b_0018\n0x8c500020 fun_ring_0008\n')
        self.assertEqual(names.misnamed(), [('1st_read', 0x8c010104, 'dat_8c010100', '0x8c010100'),
                                            ('stg13', 0x8c500010, 'fun_b_0018', "0x0018 into modules/b.c's code, "
                                                                                '0x8c500018')])

    def test_an_offset_may_lie_in_any_source_of_its_name(self):
        self.write('src/a.c', '')
        self.write('modules/b.c', '')
        self.write('modules/x/b.c', '')
        self.image('1st_read', MAIN)
        self.image('stg13', STAGE + 'modules/x/b.c P 0x8c500040 0x8c500080\n',
                   '0x8c500008 fun_b_0008\n0x8c500044 fun_b_0004\n0x8c500050 fun_b_0020\n')
        self.assertEqual(names.misnamed(), [('stg13', 0x8c500050, 'fun_b_0020', "0x0020 into modules/b.c's code, "
                                             "0x8c500020 or 0x0020 into modules/x/b.c's code, 0x8c500060")])

    def test_an_offset_lies_in_its_own_sources_code(self):
        self.write('src/a.c', '')
        self.write('modules/b.c', '')
        self.write('modules/c.c', '')
        self.write('modules/d.c', '')
        self.image('1st_read', MAIN)
        self.image('stg13', 'file SONIC2/STG13.PRS\nP 0x8c500000 0x8c500100\nD 0x8c500100 0x8c500200\n'
                   'modules/b.c P 0x8c500000 0x8c500040\nmodules/c.c P 0x8c500040 0x8c500080\n'
                   'modules/d.c D 0x8c500100 0x8c500110\n',
                   '0x8c500040 fun_b_0040\n0x8c500044 fun_c_0004\n0x8c500100 fun_d_0000\n')
        self.assertEqual(names.misnamed(), [
            ('stg13', 0x8c500040, 'fun_b_0040', "0x0040 into modules/b.c's code, 0x8c500040, outside it"),
            ('stg13', 0x8c500100, 'fun_d_0000', "0x0000 into modules/d.c's code, and it places none")])
