"""Hold a GCC image to the memory contract: the RAM each owner holds, and
which owners are live together.

    python3 tools/dc_memory.py IMAGE ROOT

RAM's 16 MB appear at 0x0c000000 and at 0x8c000000. The contract places
each owner at its 0x0c address, so two owners that reach the same RAM
through the two views overlap. An owner runs from its start up to its end,
which no byte of it holds.

The owners that are not images take their addresses from the sources that
fix them, as make dc's GCC compiles each:

- src/system/areas.h gives WORK_AREA, up to WORK_AREA_END. It gives
  DEMO_AREA, DEMO_AREA_BYTES long, the capacity the checked reads of a file
  into it pass. A stage's demo file is the exception: it is DEMO_FILE_BYTES
  long from DEMO_AREA, the size loader_prepare_stage's read holds it to.
  areas.h gives the Chao archives, CHAO_MODEL_BYTES at CHAO_MODEL_SLOT and
  CHAO_MOTION_BYTES at CHAO_MOTION_SLOT.
- areas.h gives MESSAGE_AREA, SCRIPT_AREA, REQUEST_AREA and the five camera
  areas, which lie end to end up to CAMLIN_AREA_END. Each runs up to the
  next one's start, as a mini event's reads and the camera tables take it.
  loader.c reads player 1's camera files with player 0's areas' sizes as
  their capacity, so CAMSET_AREA_1 and CAMLIN_AREA_1 each hold at least
  that many bytes.
- areas.h gives the race's land file, from CHAO_RACE_LAND_AREA, and the
  lobby's, from CHAO_LOBBY_LAND_AREA. Each runs up to CHAO_LAND_BUILD_AREA,
  the capacity its read in scene.c or lobby.c takes. The two compile their
  land from CHAO_LAND_BUILD_AREA up to DEMO_AREA, where world_build_land
  stops.
- areas.h gives CONFIG_WORK, the work syCfgInit keeps until syCfgExit. It
  must be as long as flash bank 2, whose size only the boot ROM gives, so
  the contract runs it up to the nearest owner that starts past it.
- sdk/shinobi/strt2.c gives the stack, from STACK_BOTTOM up to STACK_TOP.
  src/system/sbinit.c gives the heap, the HEAP_SIZE bytes before
  MAIN_MEMORY_END.
- modules/chao/garden.c gives CHAO_AREA_SLOT, where each Chao area loads.
  Its area_files[] gives each area's limit, the address the area must end
  by.

An image's ELF gives the image, from the lowest start of its LOAD segments
to the highest end. CHAO's archives are no part of CHAO: their segments lie
where areas.h places the archives. An image that defines end compiles its
land from there, rounded up to 32. In the 0x8c view, where the stages link,
world_build_land stops before DEMO_AREA, and the land is an owner,
STAGE_MODEL_BUILD_AREA, up to there. In the 0x0c view, where the Chao
areas link, that bound never holds (world.c's @bug), so no owner holds the
land.

A state is a set of owners live together. Each holds the main executable,
the stack and the heap. No source lists the states: the contract writes
them as it reads the loaders.

- boot: src/main.c's system_boot_init hands syCfgInit CONFIG_WORK, and
  calls syCfgExit before any file is read. The state holds CONFIG_WORK.
- each image at STAGE_SLOT but CHAO: loader_prepare_stage and
  loader_start_program give such an image the bytes up to DEMO_AREA. A
  stage's mini events and camera files fill the areas from there. The
  state holds WORK_AREA, the image and its land, DEMO_AREA and each area
  past it. No code of the main executable names TITLE.PRS, so its state is
  an inference from its link at STAGE_SLOT.
- a stage in an attract demo: loader_prepare_stage reads demo<stage>.prs
  beside a stage below STAGE_STG_END, whose image is stg<stage>. The disc's
  demo files are dumps of RAM that run on from DEMO_AREA over a mini
  event's three areas, MESSAGE_AREA, SCRIPT_AREA and REQUEST_AREA. The
  file and a mini event's files hold that RAM by turns. mini_event_load
  loads DEMO_AREA, MESSAGE_AREA and SCRIPT_AREA again before an event
  starts, and an event reads nothing of REQUEST_AREA. The state is the
  stage's own, with the demo file in place of DEMO_AREA and those three
  areas.
- the garden: loader_prepare_stage reads CHAO to STAGE_SLOT, below the
  archives. The state holds WORK_AREA, CHAO and its archives, and
  MESSAGE_AREA and each area past it, where the garden reads its camera
  files. It leaves out DEMO_AREA, since area_files[] lets five Chao areas
  run up to MESSAGE_AREA.
- a Chao area: the garden's state, with the area at CHAO_AREA_SLOT. The
  race's state holds its land file and the land it compiles, and so does
  the lobby's, whatever limit area_files[] gives either.

A run checks each state that holds IMAGE: boot for the main executable, the
garden for CHAO, and the image's own for any other, with its demo's for a
stage. The main executable lies in every state, and CHAO in each Chao
area's, so the other images' runs check them as well. A run fails where:

- a source gives no number for an address or size the contract reads, or
  an address outside main RAM;
- CHAO's ELF holds no segment where areas.h places an archive, or none
  besides them;
- areas.h's WORK_AREA_FILE lies outside WORK_AREA;
- area_files[] has a row in a form the contract doesn't read, or gives a
  Chao area no limit;
- IMAGE, unless it is the main executable, has its entry in other RAM
  than its loader reads it to: STAGE_SLOT, or CHAO_AREA_SLOT for a Chao
  area;
- a Chao area ends past its limit;
- an owner of a state ends before it starts;
- an owner of a state holds a byte outside main RAM;
- two owners of a state overlap.

Otherwise it writes ROOT/memory/IMAGE.txt: each state's name, then each of
its owners, from its start to its end, in address order.
"""
import argparse
import ast
from collections import namedtuple
from pathlib import Path
import re

import csource
from config import CHAO_AREAS, MAIN, aligned, at_root, fail, hexa, save
from dc_overlay import ET_EXEC, Elf, physical

AREAS_H = Path('src/system/areas.h')
STRT2_C = Path('sdk/shinobi/strt2.c')
SBINIT_C = Path('src/system/sbinit.c')
GARDEN_C = Path('modules/chao/garden.c')
SOURCES = (AREAS_H, STRT2_C, SBINIT_C, GARDEN_C)
# CHAO's image, the garden
GARDEN = 'chao'
# a stage's image, stg<stage>, which an attract demo's file loads beside
STAGE = re.compile(r'stg\d\d')
# the owner that is a stage's demo file, and the owners it runs over, which a
# demo's state holds it in place of
DEMO_FILE = 'demo file'
DEMO_FILE_AREAS = ('DEMO_AREA', 'MESSAGE_AREA', 'SCRIPT_AREA', 'REQUEST_AREA')
# a mini event's areas and the camera areas, in address order. they lie end
# to end from MESSAGE_AREA up to CAMLIN_AREA_END
EVENT_AND_CAMERA_AREAS = ('MESSAGE_AREA', 'SCRIPT_AREA', 'REQUEST_AREA', 'CAMSET_AREA', 'CAMSET_AREA_0',
                          'CAMSET_AREA_1', 'CAMLIN_AREA_0', 'CAMLIN_AREA_1')
# player 1's camera areas, each mapped to player 0's area, whose size
# loader.c passes as the capacity of player 1's read
PLAYER_1_CAMERA_AREAS = {'CAMSET_AREA_1': 'CAMSET_AREA_0', 'CAMLIN_AREA_1': 'CAMLIN_AREA_0'}
# the macros that give an owner's end, which may lie just past RAM, as no
# byte of the owner lies there
ENDS = ('WORK_AREA_END', 'CAMLIN_AREA_END', 'STACK_TOP', 'MAIN_MEMORY_END')
# the area each Chao area that reads a land file reads it to, by image
LAND_FILES = {'chaostgrace': 'CHAO_RACE_LAND_AREA', 'chaostglobby': 'CHAO_LOBBY_LAND_AREA'}
# the bit the 0x8c view sets, and the boundary world_build_land rounds a
# land's start up to
VIEW_BIT = 0x80000000
LAND_ALIGN = 32
# a cast in a macro's body, as csource.macro_directives() spells its tokens:
# to an integer, or to a pointer. Arithmetic on a pointer to bytes counts
# bytes, and on a pointer to anything else, a size the contract doesn't read
CAST = re.compile(r'\(\s*(?:(?P<counted>Uint32|uintptr_t|(?:const\s+)?(?:Uint8|Sint8|char)\s*\*)'
                  r'|(?P<scaled>(?:const\s+)?(?:struct\s+)?\w+\s*\*+))\s*\)(?=\s*[\w(])')
# an integer constant's suffix, and an octal constant's digits
SUFFIX = re.compile(r'\b(0[xX][0-9a-fA-F]+|[0-9]+)[uUlL]+\b')
OCTAL = re.compile(r'\b0([0-7]+)\b')
# the nodes a macro's value may hold besides its integer constants: +, - and
# |. no bit of their result depends on a higher bit of an operand, so the
# result modulo 2**32 is the one Uint32 arithmetic gives
ARITHMETIC = (ast.Expression, ast.BinOp, ast.Add, ast.Sub, ast.BitOr)
# area_files[] in garden.c, and each of its rows: an area's file, its tag
# and its limit, an areas.h macro cast to uintptr_t
AREA_FILES = re.compile(r'\barea_files\[\]\s*=\s*\{(.*?)\};', re.S)
AREA_FILE = re.compile(r'\{\s*"([^"]+)"\s*,\s*"[^"]*"\s*,\s*\(uintptr_t\)\s*([A-Z_][A-Z0-9_]*)\s*\}')

# an owner of RAM: its name, and its start and end in the 0x0c view
Owner = namedtuple('Owner', 'name start end')
# a Chao area's limit: the macro that names it, and its address
Limit = namedtuple('Limit', 'name address')
# what a run reads of an image's ELF, each address in the 0x0c view: its
# entry, the lowest start of its LOAD segments and the highest end, and the
# start of the land it compiles, or None where no owner holds its land
Extent = namedtuple('Extent', 'entry start end land')
# the owners the sources fix, as Owners by name; STAGE_SLOT; CHAO_AREA_SLOT;
# each Chao area's Limit, by image; CONFIG_WORK; and the end of RAM
Contract = namedtuple('Contract', 'owners stage_slot chao_slot limits config_work ram_end')


def numbers(path, names):
    """The values of the macros names lists, by name. Each is the value its
    #define gives in the C at path, as make dc's GCC compiles the file. A
    value is an integer constant, or +, - and | of such constants and of the
    file's other macros, taken modulo 2**32 as a Uint32 holds it. A macro
    stands for its tokens as the preprocessor substitutes them, with no
    grouping its body doesn't write. Any part may be cast to Uint32, to
    uintptr_t or to a pointer. A cast to a pointer to anything but bytes
    takes a bare constant alone, as arithmetic on it scales.

    A macro of names that has no value, or that the file defines twice,
    fails, naming path and the macro."""
    defined, twice = {}, set()
    for _, name, macro in csource.macro_directives(csource.code(path, csource.GCC)):
        if macro is not None and macro[0] is None:
            if name in defined:
                twice.add(name)
            defined[name] = ' '.join(macro[1])

    def expanded(text, seen):
        return re.sub(r'\b[A-Z_][A-Z0-9_]*\b', lambda word: word[0] if word[0] not in defined or word[0] in seen
                      else f' {expanded(defined[word[0]], seen | {word[0]})} ', text)

    def value(name):
        scaled = []

        def uncast(cast):
            scaled.append(cast['scaled'])
            return ''

        text = OCTAL.sub(r'0o\1', SUFFIX.sub(r'\1', CAST.sub(uncast, expanded(defined[name], {name})))).strip()
        try:
            tree = ast.parse(text, mode='eval')
        except SyntaxError:
            return None
        if (not all(isinstance(node, ARITHMETIC) or isinstance(node, ast.Constant) and type(node.value) is int
                    for node in ast.walk(tree))
                or any(scaled) and not isinstance(tree.body, ast.Constant)):
            return None
        return eval(compile(tree, str(path), 'eval')) % 2**32

    found = {name: value(name) for name in names if name in defined and name not in twice}
    missing = [name for name in names if found.get(name) is None]
    if missing:
        fail(*(f'{path}: gives no number for {name}, which the memory contract reads' for name in missing))
    return found


def limits(at):
    """Each Chao area's Limit, as area_files[] in garden.c gives it, by
    image. at gives each address the contract reads, by macro. A row's file
    names its image, lowercased without its extension, and an image two rows
    name takes the lower limit, the one both its loads fit. A table missing,
    or holding a row in another form, or naming a macro at lacks, fails, as
    does a Chao area with no row."""
    text = csource.code(GARDEN_C, csource.GCC)
    table = AREA_FILES.search(text)
    if table is None:
        fail(f'{GARDEN_C}: holds no area_files[] that make dc compiles, for the memory contract to read')
    rows = AREA_FILE.findall(table[1])
    if AREA_FILE.sub('', table[1]).replace(',', '').strip():
        fail(f"{GARDEN_C}: area_files[] holds a row in a form the memory contract doesn't read")
    found = {}
    for file, macro in rows:
        if macro not in at:
            fail(f"{GARDEN_C}: area_files[] ends {file} at {macro}, which the memory contract doesn't read")
        image = Path(file).stem.lower()
        if image not in found or at[macro] < found[image].address:
            found[image] = Limit(macro, at[macro])
    missing = sorted(CHAO_AREAS - set(found))
    if missing:
        fail(*(f'{GARDEN_C}: area_files[] gives {image} no limit' for image in missing))
    return found


def contract():
    """The Contract the sources give, as the module's docstring reads them.
    It fails where a source gives no number the contract reads, or an
    address outside main RAM; where WORK_AREA_FILE lies outside WORK_AREA;
    and where limits() fails."""
    sizes = ('CHAO_MODEL_BYTES', 'CHAO_MOTION_BYTES', 'DEMO_AREA_BYTES', 'DEMO_FILE_BYTES')
    addresses = ('WORK_AREA', 'WORK_AREA_FILE', 'WORK_AREA_END', 'STAGE_SLOT', 'CHAO_MODEL_SLOT', 'CHAO_MOTION_SLOT',
                 'CONFIG_WORK', *LAND_FILES.values(), 'CHAO_LAND_BUILD_AREA', 'DEMO_AREA', *EVENT_AND_CAMERA_AREAS,
                 'CAMLIN_AREA_END')
    areas = numbers(AREAS_H, addresses + sizes)
    stack = numbers(STRT2_C, ('STACK_BOTTOM', 'STACK_TOP'))
    heap = numbers(SBINIT_C, ('HEAP_SIZE', 'MAIN_MEMORY_END'))
    slot = numbers(GARDEN_C, ('CHAO_AREA_SLOT',))

    def placed(path, given, names):
        # each address of given by name, in the 0x0c view. an end of ENDS
        # may lie just past RAM, so its owner's last byte places it
        found = {}
        for name in names:
            try:
                found[name] = physical(given[name] - 1) + 1 if name in ENDS else physical(given[name])
            except ValueError:
                fail(f'{path}: {name}, {hexa(given[name])}, lies outside main RAM')
        return found

    at = {**placed(AREAS_H, areas, addresses), **placed(STRT2_C, stack, stack),
          **placed(GARDEN_C, slot, slot), **placed(SBINIT_C, heap, ('MAIN_MEMORY_END',))}
    if not at['WORK_AREA'] <= at['WORK_AREA_FILE'] < at['WORK_AREA_END']:
        fail(f"{AREAS_H}: WORK_AREA_FILE, {hexa(areas['WORK_AREA_FILE'])}, lies outside WORK_AREA, "
             f"{hexa(areas['WORK_AREA'])} to {hexa(areas['WORK_AREA_END'])}")
    ram_end, build, demo = at['MAIN_MEMORY_END'], at['CHAO_LAND_BUILD_AREA'], at['DEMO_AREA']
    owners = [Owner('stack', at['STACK_BOTTOM'], at['STACK_TOP']),
              Owner('heap', ram_end - heap['HEAP_SIZE'], ram_end),
              Owner('WORK_AREA', at['WORK_AREA'], at['WORK_AREA_END']),
              Owner('DEMO_AREA', demo, demo + areas['DEMO_AREA_BYTES']),
              Owner(DEMO_FILE, demo, demo + areas['DEMO_FILE_BYTES']),
              Owner('CHAO_MODEL_SLOT', at['CHAO_MODEL_SLOT'], at['CHAO_MODEL_SLOT'] + areas['CHAO_MODEL_BYTES']),
              Owner('CHAO_MOTION_SLOT', at['CHAO_MOTION_SLOT'], at['CHAO_MOTION_SLOT'] + areas['CHAO_MOTION_BYTES']),
              *(Owner(name, at[name], build) for name in LAND_FILES.values()),
              Owner('CHAO_LAND_BUILD_AREA', build, demo)]
    starts = [at[name] for name in (*EVENT_AND_CAMERA_AREAS, 'CAMLIN_AREA_END')]
    ends = dict(zip(EVENT_AND_CAMERA_AREAS, starts[1:]))
    # an area that ends before it starts stays so, for problems() to name
    for area, player_0 in PLAYER_1_CAMERA_AREAS.items():
        if ends[area] >= at[area]:
            ends[area] = max(ends[area], at[area] + ends[player_0] - at[player_0])
    owners += [Owner(name, at[name], ends[name]) for name in EVENT_AND_CAMERA_AREAS]
    return Contract({owner.name: owner for owner in owners}, at['STAGE_SLOT'], at['CHAO_AREA_SLOT'], limits(at),
                    at['CONFIG_WORK'], ram_end)


def extent(name, elf, terms):
    """The Extent of the image name, from its ELF, an Elf. terms is the
    Contract, whose Chao archives are no part of CHAO. A CHAO whose ELF holds
    no segment where an archive lies, or none but the archives', raises
    ValueError."""
    spans = list(elf.loads)
    if name == GARDEN:
        for archive in (terms.owners['CHAO_MODEL_SLOT'], terms.owners['CHAO_MOTION_SLOT']):
            span = (archive.start, archive.end)
            if span not in spans:
                raise ValueError(f'{elf.path}: no segment lies at {archive.name}, {hexa(archive.start)} to '
                                 f'{hexa(archive.end)}')
            spans.remove(span)
        if not spans:
            raise ValueError(f'{elf.path}: holds no LOAD segment but the Chao archives')
    land = elf.defined.get('_end')
    if land is not None and land & VIEW_BIT:
        land = aligned(physical(land), LAND_ALIGN)
    else:
        land = None
    return Extent(physical(elf.entry), min(start for start, _ in spans), max(end for _, end in spans), land)


def state(image, extents, terms):
    """The state that holds image, as (its name, its Owners). extents gives
    the Extent of image and of each other image the state holds: the main
    executable's, and CHAO's for a Chao area. terms is the Contract."""
    def held(name):
        return Owner(name, extents[name].start, extents[name].end)

    event_and_camera = [terms.owners[name] for name in EVENT_AND_CAMERA_AREAS]
    owners = [held(MAIN), terms.owners['stack'], terms.owners['heap']]
    if image == MAIN:
        start = terms.config_work
        end = min([owner.start for owner in owners if owner.start > start] + [terms.ram_end])
        return 'boot', owners + [Owner('CONFIG_WORK', start, end)]
    owners.append(terms.owners['WORK_AREA'])
    if image != GARDEN and image not in CHAO_AREAS:
        demo = terms.owners['DEMO_AREA']
        land = extents[image].land
        owners += [held(image), demo, *event_and_camera]
        if land is not None:
            owners.append(Owner('STAGE_MODEL_BUILD_AREA', land, demo.start))
        return image, owners
    owners += [held(GARDEN), terms.owners['CHAO_MODEL_SLOT'], terms.owners['CHAO_MOTION_SLOT'], *event_and_camera]
    if image == GARDEN:
        return 'the garden', owners
    owners.append(held(image))
    if image in LAND_FILES:
        owners += [terms.owners[LAND_FILES[image]], terms.owners['CHAO_LAND_BUILD_AREA']]
    return image, owners


def demo_state(image, extents, terms):
    """The state in which the stage image plays an attract demo, as state()
    gives one: the stage's own, with the demo file in place of the owners it
    runs over. None for an image that is no stage's."""
    if not STAGE.fullmatch(image):
        return None
    name, owners = state(image, extents, terms)
    return f'{name}, in a demo', [owner for owner in owners if owner.name not in DEMO_FILE_AREAS] + [
        terms.owners[DEMO_FILE]]


def states(image, extents, terms):
    """Each state that holds image, as state() gives one: its own, then its
    demo's for a stage."""
    demo = demo_state(image, extents, terms)
    return [state(image, extents, terms)] + ([demo] if demo else [])


def address_order(owner):
    """owner's key in address order: its start, then its end."""
    return owner.start, owner.end


def problems(image, extents, terms):
    """Each problem the contract finds in the states that hold image, as the
    module's docstring lists them, once each. extents and terms are as
    state() takes them."""
    found = []
    own, chao = extents[image], image in CHAO_AREAS
    if image != MAIN:
        slot = terms.chao_slot if chao else terms.stage_slot
        if own.entry != slot:
            found.append(f"{image}: its entry is {hexa(own.entry)}, and its loader reads it to "
                         f"{'CHAO_AREA_SLOT' if chao else 'STAGE_SLOT'}, {hexa(slot)}")
    if chao:
        limit = terms.limits[image]
        if own.end > limit.address:
            found.append(f'{image}: it ends at {hexa(own.end)}, past its limit in area_files[], {limit.name}, '
                         f'{hexa(limit.address)}')

    def in_ram(start, end):
        try:
            return physical(start) == start and physical(end - 1) == end - 1
        except ValueError:
            return False

    for _, owners in states(image, extents, terms):
        held = [f'{name}: ends at {hexa(end)}, before it starts at {hexa(start)}' for name, start, end in owners
                if end < start]
        held += [f'{name}, {hexa(start)} to {hexa(end)}, holds bytes outside main RAM' for name, start, end in owners
                 if start < end and not in_ram(start, end)]
        ordered = sorted((owner for owner in owners if owner.start < owner.end), key=address_order)
        for index, (name, start, end) in enumerate(ordered):
            for other, low, high in ordered[index + 1:]:
                if low >= end:
                    break
                held.append(f'{name}, {hexa(start)} to {hexa(end)}, overlaps {other}, {hexa(low)} to {hexa(high)}')
        found += [problem for problem in held if problem not in found]
    return found


def report(image, extents, terms):
    """The lines of IMAGE.txt: the name of each state that holds image, then
    each of its owners, from its start to its end, in address order."""
    return [line for name, owners in states(image, extents, terms)
            for line in (f'{name}\n', *(f'  {hexa(owner.start)}  {hexa(owner.end)}  {owner.name}\n'
                                        for owner in sorted(owners, key=address_order)))]


def main(argv=None):
    parser = argparse.ArgumentParser(prog='dc_memory', description=__doc__.split('\n\n')[0])
    parser.add_argument('image', metavar='IMAGE', help='an image of config/, such as stg13')
    parser.add_argument('root', metavar='ROOT', type=Path, help='the GNU build directory that holds images/')
    args = parser.parse_args(argv)
    at_root(*map(str, SOURCES))
    terms = contract()
    held = {MAIN, args.image, *([GARDEN] if args.image in CHAO_AREAS else [])}
    try:
        extents = {name: extent(name, Elf(args.root / 'images' / f'{name}.elf', ET_EXEC), terms) for name in held}
    except (OSError, ValueError) as error:
        fail(error)
    found = problems(args.image, extents, terms)
    if found:
        fail(*found)
    save(args.root / 'memory' / f'{args.image}.txt', ''.join(report(args.image, extents, terms)).encode())


if __name__ == '__main__':
    main()
