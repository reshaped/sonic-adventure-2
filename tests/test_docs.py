"""What the docs cite and quote: each link and anchor resolves, each table row
a doc quotes is a row of some table, and each block of C it quotes is in a
source. And what TOOLCHAIN.md must say: each patched source's entry."""

import collections
import fnmatch
import os
import posixpath
import re
import subprocess
import unittest
from pathlib import Path

import vendor
from support import TreeTest

ROOT = Path(__file__).resolve().parents[1]
FENCE = re.compile(r' {0,3}(`{3,}|~{3,})(\w*)')
HEADING = re.compile(r' {0,3}#{1,6}(?:[ \t]+(.*?))?(?:[ \t]+#+)?[ \t]*')
# what a slug drops of inline markup: a code span's backticks, an image, a
# link's brackets and destination, an HTML tag, an escape's backslash and an
# underscore that opens or closes emphasis
INLINE = re.compile(r'(`+)(.+?)\1|!\[[^\]]*\]\([^)]*\)|\[([^\]]*)\]\([^)]*\)|</?[A-Za-z][^>]*>|\\(.)'
                    r'|(?<!\w)_+(?=\S)|(?<=\S)_+(?!\w)')
CODE = re.compile(r'(`+).+?\1')
# an inline link's or image's destination, or a reference definition's
LINK = re.compile(r'(?:\]\(\s*|^ {0,3}\[[^\]^][^\]]*\]:\s*)<?([^\s)>]+)')
# a URL, whose path names no file of the tree
URL = re.compile(r'[A-Za-z][\w+.-]*://\S*')
# a path to a Markdown file, # and an anchor
NAMED = re.compile(r'([\w./-]+\.md)#([\w-]+)')
# a link into a GitHub repository at a commit or branch, short of the
# punctuation that ends a sentence around it
GITHUB = re.compile(r'(https://github\.com/[\w.-]+/[\w.-]+)/(?:blob|tree)/([\w.-]*\w)'
                    r'(?:[/#?][^\s)>"\'`]*[^\s)>"\'`.,;:!?])?')
# a splits.txt row that places a range or bounds a section, at real addresses
ROW = re.compile(r'(?:\S+\s+)?[A-Z][A-Z0-9]*\s+0x[0-9a-f]{8}\s+0x[0-9a-f]{8}')


def read(path):
    """The lines of the file at path, numbered as editors number them; a
    byte that isn't UTF-8 cites nothing."""
    return Path(path).read_bytes().decode('utf-8', 'replace').split('\n')


def blocks(lines):
    """Each fenced code block in a Markdown file's lines, as (info, first,
    lines): its info string's first word, the number of its first line
    inside the fences, and those lines."""
    found, fence = [], None
    for number, line in enumerate(lines, 1):
        opened = FENCE.match(line)
        if fence:
            if opened and opened[1].startswith(fence[0]) and not line[opened.end(1):].strip():
                found.append(fence[1:])
                fence = None
            else:
                fence[3].append(line)
        # backticks with another backtick after them open a code span, not a block
        elif opened and not (opened[1][0] == '`' and '`' in line[opened.end():]):
            fence = [opened[1], opened[2], number + 1, []]
    return [tuple(block) for block in found]


def fenced(lines):
    """The numbers of the lines that fenced code blocks hold, fences included."""
    return {number for _, first, held in blocks(lines) for number in range(first - 1, first + len(held) + 1)}


def slug(heading):
    """GitHub's slug of a heading's text: its inline markup dropped, then
    lowercase, with every character but a letter, digit, space, hyphen or
    underscore gone, and each space a hyphen."""
    text = INLINE.sub(lambda markup: markup[2] or markup[3] or markup[4] or '', heading)
    return re.sub(r'[^\w\- ]', '', text.lower()).replace(' ', '-')


def anchors(lines):
    """The anchor GitHub gives each # heading in a Markdown file's lines, out
    of code blocks: its slug, or for the nth repeat of a slug, the slug, a
    hyphen and n."""
    code, seen, found = fenced(lines), collections.Counter(), set()
    for number, line in enumerate(lines, 1):
        heading = number not in code and HEADING.fullmatch(line)
        if heading:
            name = slug(heading[1] or '')
            found.add(f'{name}-{seen[name]}' if seen[name] else name)
            seen[name] += 1
    return found


def linked(source, destination):
    """The path from the root and the anchor that a link's destination in
    the file source names, or None for a URL."""
    if re.match(r'[A-Za-z][\w+.-]*:|//', destination):
        return None
    link, _, anchor = destination.partition('#')
    if not link:
        return source, anchor
    # GitHub takes a link that starts with / from the repository's root
    base = '' if link.startswith('/') else posixpath.dirname(source)
    return posixpath.normpath(posixpath.join(base, link.lstrip('/'))), anchor


def unresolved(paths):
    """Each citation that fails to resolve in the files at paths, the tree's
    files, other than a vendored tool's, as FILE:LINE: CITATION: why.

    A citation is a relative link or image in a Markdown file, from the
    file's directory, or from the root where it starts with /; or, in any
    file's text but a Markdown link or a URL, a path from the root to a
    Markdown file, then # and an anchor. It names a file or directory of the
    tree, and its anchor a heading of its Markdown file. A link that starts
    REPOSITORY/blob/COMMIT or REPOSITORY/tree/COMMIT, where REPOSITORY is a
    vendored tool's as tools/SHA256SUMS spells it, names the commit
    tools/SHA256SUMS pins for that tool."""
    pinned = vendor.blocks(Path(vendor.SUMS).read_text()) if Path(vendor.SUMS).exists() else {}
    pins = {repository: (name, commit) for name, (repository, commit, _) in pinned.items()}
    # a vendored tool's citations name its own repository's files
    vendored = tuple(f'tools/{name}/' for name in vendor.TOOLS)
    files = set(paths)
    directories = {str(parent) for path in paths for parent in Path(path).parents}
    headings = {path: anchors(read(path)) for path in paths if path.endswith('.md')}

    def why(path, anchor):
        if path not in files and path not in directories:
            return f'the tree holds no {path}'
        if anchor and path not in headings:
            return f'{path} is not Markdown, so no heading has this anchor'
        if anchor and anchor not in headings[path]:
            return f'no heading of {path} has this anchor'
        return None

    found = []
    for path in paths:
        if path.startswith(vendored):
            continue
        lines = read(path)
        markdown = path.endswith('.md')
        # outside Markdown only NAMED, which needs .md#, and GITHUB, which needs ://, can cite
        if not markdown and not any('.md#' in line or '://' in line for line in lines):
            continue
        code = fenced(lines) if markdown else ()
        for number, line in enumerate(lines, 1):
            # the spans URLs and links hold, where a path is no citation in plain text
            cited, taken = [], [url.span() for url in URL.finditer(line)]
            if markdown and number not in code:
                for link in LINK.finditer(CODE.sub(lambda span: ' ' * len(span[0]), line)):
                    taken.append(link.span(1))
                    target = linked(path, link[1])
                    if target:
                        cited.append((link[1], *target))
            cited += [(name[0], posixpath.normpath(name[1]), name[2]) for name in NAMED.finditer(line)
                      if not any(start <= name.start() < end for start, end in taken)]
            for citation, target, anchor in cited:
                problem = why(target, anchor)
                if problem:
                    found.append(f'{path}:{number}: {citation}: {problem}')
            for link in GITHUB.finditer(line):
                pin = pins.get(link[1])
                if pin and link[2] != pin[1]:
                    found.append(f'{path}:{number}: {link[0]}: tools/SHA256SUMS pins {pin[0]} at {pin[1]}')
    return found


def misquoted(paths):
    """Each quotation in the Markdown files of paths that the tree doesn't
    hold, as FILE:LINE: why: a line of a code block that reads as a
    splits.txt row at real addresses and is no row of any config/*/splits.txt,
    and a c block whose lines, each stripped of its indent, occur in a row in
    no C file of paths."""
    rows = {' '.join(line.split('#')[0].split()) for path in paths
            if re.fullmatch(r'config/[^/]+/splits\.txt', path) for line in read(path)}
    sources = ['\n'.join(line.strip() for line in read(path)) for path in paths if path.endswith(('.c', '.h'))]
    found = []
    for path in paths:
        if not path.endswith('.md'):
            continue
        for info, first, held in blocks(read(path)):
            for number, line in enumerate(held, first):
                if ROW.fullmatch(line.strip()) and ' '.join(line.split()) not in rows:
                    found.append(f'{path}:{number}: {line.strip()}: no row of any config/*/splits.txt')
            quoted = '\n'.join(line.strip() for line in held)
            if info == 'c' and not any(f'\n{quoted}\n' in f'\n{text}\n' for text in sources):
                found.append(f'{path}:{first}: its c block occurs in no C file of the tree')
    return found


def ignoring(text):
    """The rules of a .gitignore's text, as (negated, anchored, directory,
    pattern) in order: a pattern that holds a slash but at its end matches a
    path from the root, and any other a name at any depth."""
    found = []
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        negated = line.startswith('!')
        line = line.lstrip('!')
        directory = line.endswith('/')
        line = line.rstrip('/')
        found.append((negated, '/' in line, directory, line.lstrip('/')))
    return found


# an entry under TOOLCHAIN.md's Patched sources: a bullet that opens with its
# source's path in bold
ENTRY = re.compile(r'- \*\*`([^`]+)`\*\*(.*)')
# TOOLCHAIN.md's Patched sources, a section or a part of one, up to the next
SECTION = re.compile(r'^#{2,3} Patched sources\n(.*?)(?=^#{1,3} |\Z)', re.M | re.S)
# what make reads to list each patched source with its patches, a line each
LIST_PATCHED = 'include config/compilers.mk\n$(foreach s,$(PATCHED),$(info $s $($s.patches)))\nall: ;\n'


def unexplained(root='.'):
    """Each source of config/compilers.mk's PATCHED that has no entry under
    TOOLCHAIN.md's Patched sources, and each patch its .patches names that
    its entry doesn't, as a line naming it."""
    listed = subprocess.run(['make', '-s', '-f', '-', 'all'], input=LIST_PATCHED, cwd=root, capture_output=True,
                            text=True, check=True).stdout.split('\n')
    section = SECTION.search((Path(root) / 'TOOLCHAIN.md').read_text())
    # an entry names a patch in its text, not in a link's destination
    entries = {match[1]: re.sub(r'\]\([^)]*\)', ']', match[2])
               for match in map(ENTRY.match, section[1].split('\n') if section else []) if match}
    found = []
    for line in filter(None, listed):
        source, *patches = line.split()
        if source not in entries:
            found.append(f"{source}: patched, but TOOLCHAIN.md's Patched sources has no entry for it")
            continue
        found += [f"{source}: its entry in TOOLCHAIN.md's Patched sources doesn't name {patch}"
                  for patch in patches if not re.search(rf'(?<![\w-]){re.escape(patch)}(?![\w-])', entries[source])]
    return found


def tree(root='.'):
    """The tree's files under root, as paths from root, but for .git and
    what .gitignore ignores: what a clone of the repository would hold."""
    rules = ignoring((Path(root) / '.gitignore').read_text()) if (Path(root) / '.gitignore').exists() else []

    def ignored(path, directory):
        verdict = False
        for negated, anchored, only, pattern in rules:
            if (directory or not only) and fnmatch.fnmatchcase(path if anchored else posixpath.basename(path), pattern):
                verdict = not negated
        return verdict

    found = []
    for at, directories, names in os.walk(root):
        relative = posixpath.relpath(at, root) if at != root else ''
        directories[:] = sorted(name for name in directories if name != '.git'
                                and not ignored(posixpath.join(relative, name), True))
        found += [posixpath.join(relative, name) for name in sorted(names)
                  if not ignored(posixpath.join(relative, name), False)]
    return found


# a guide whose anchors are guide, build, tags and tags-1, a pin for shckit,
# and a table
PIN = '1' * 40
SHCKIT = 'https://github.com/reshaped/shckit'
GUIDE = 'GUIDE.md'
FILES = {
    'tools/SHA256SUMS': f'# {SHCKIT} {PIN}\n',
    GUIDE: '# Guide\n\n## Build\n\n## Tags\n\n```sh\n# a comment\n```\n\n### Tags\n',
    'config/a/splits.txt': 'file A.BIN\nP           0x8c010000  0x8c010100\nsrc/a.c     P  0x8c010000  0x8c010040\n',
    'src/a.c': 'void a(void)\n{\n    /* a comment */\n    int x;\n}\n',
}


class Docs(TreeTest):
    def files(self, files):
        files = dict(FILES, **files)
        for path, text in files.items():
            self.write(path, text)
        return list(files)


class Anchors(Docs):
    def test_gives_each_heading_the_anchor_github_gives_it(self):
        lines = ['# The `make check` gate', '## C for SHC: R13 and [R32](#releases) ##', '### `<image>` tables',
                 '#### snake_case, _emphasis_ and \\_escapes\\_', '##### <a id="x"></a>Hashes', '## % C and % assembly']
        self.assertEqual(anchors(lines), {'the-make-check-gate', 'c-for-shc-r13-and-r32', 'image-tables',
                                          'snake_case-emphasis-and-_escapes_', 'hashes', '-c-and--assembly'})

    def test_numbers_each_repeat_of_a_slug_in_document_order(self):
        self.assertEqual(anchors(['# Tags', '## Tags', '### Tags!', '#### Tag']), {'tags', 'tags-1', 'tags-2', 'tag'})

    def test_reads_no_heading_in_a_code_block(self):
        lines = ['# Toolchain', '````sh', '```', '# in the block', '````', '~~~', '# in another', '~~~',
                 '```a``` opens no block', '# Hashes']
        self.assertEqual(anchors(lines), {'toolchain', 'hashes'})


class Citations(Docs):
    def test_accepts_citations_that_resolve(self):
        paths = self.files({
            'README.md': (f'[Tags]({GUIDE}#tags-1), [Build](./{GUIDE}#build), [tables](config/) and [up](#read-me), '
                          f'[rules]({SHCKIT}/blob/{PIN}/docs/inspect.md)\n\n# Read me\n\n[guide]: {GUIDE}#guide\n'),
            'docs/NOTES.md': f'# Notes\n\n[back](../{GUIDE}#build), [table](/config/a/splits.txt) and [here](#notes)\n'
                             '`[not a link](GONE.md)`\n',
            'Makefile': f'# see {GUIDE}#tags, as {SHCKIT}/blob/{PIN}/docs/{GUIDE}#nowhere says\n',
            'tools/shckit/LICENSE': f"{GUIDE}#nowhere, in shckit's own repository\n",
        })
        self.assertEqual(unresolved(paths), [])

    def test_reports_every_citation_that_fails_at_once(self):
        paths = self.files({
            'README.md': (f'[gone](GONE.md), [build]({GUIDE}#biuld) and [top](#nowhere)\n'
                          f'[Tags]({GUIDE}#tags-2), [rows](config/a/splits.txt#rows) and `{GUIDE}#guid`\n'
                          f'```text\n[not a link](GONE.md), see {GUIDE}#nowhere\n```\n\n[old]: OLD.md\n'),
            'Makefile': f'# see {GUIDE}#Build and gone/{GUIDE}#build\n',
            'tools/a.py': f'# {SHCKIT}/tree/main/docs, at its head\n',
        })
        self.assertEqual(unresolved(paths), [
            'README.md:1: GONE.md: the tree holds no GONE.md',
            f'README.md:1: {GUIDE}#biuld: no heading of {GUIDE} has this anchor',
            'README.md:1: #nowhere: no heading of README.md has this anchor',
            f'README.md:2: {GUIDE}#tags-2: no heading of {GUIDE} has this anchor',
            'README.md:2: config/a/splits.txt#rows: config/a/splits.txt is not Markdown, so no heading has this anchor',
            f'README.md:2: {GUIDE}#guid: no heading of {GUIDE} has this anchor',
            f'README.md:4: {GUIDE}#nowhere: no heading of {GUIDE} has this anchor',
            'README.md:7: OLD.md: the tree holds no OLD.md',
            f'Makefile:1: {GUIDE}#Build: no heading of {GUIDE} has this anchor',
            f'Makefile:1: gone/{GUIDE}#build: the tree holds no gone/{GUIDE}',
            f'tools/a.py:1: {SHCKIT}/tree/main/docs: tools/SHA256SUMS pins shckit at {PIN}',
        ])


class Quotations(Docs):
    def test_accepts_rows_and_c_the_tree_holds(self):
        paths = self.files({'README.md': ('```text\nfile PATH\nSECTION START END\nP 0x8c010000 0x8c010100\n'
                                          'src/a.c P 0x8c010000 0x8c010040   \n```\n\n'
                                          '```c\n/* a comment */\nint x;\n```\n')})
        self.assertEqual(misquoted(paths), [])

    def test_names_a_row_or_c_the_tree_lacks(self):
        paths = self.files({'README.md': ('```text\ndisc P 0x8c010040 0x8c010100\nsrc/a.c P 0x8c010000 0x8c010044\n'
                                          '```\n\n```c\nint x;\n/* a comment */\n```\n')})
        self.assertEqual(misquoted(paths), [
            'README.md:2: disc P 0x8c010040 0x8c010100: no row of any config/*/splits.txt',
            'README.md:3: src/a.c P 0x8c010000 0x8c010044: no row of any config/*/splits.txt',
            'README.md:7: its c block occurs in no C file of the tree'])


class Walk(Docs):
    def test_walks_what_a_clone_would_hold(self):
        self.files({'.gitignore': '/build/\n/orig/*\n!/orig/.gitkeep\n/tools/wibo-*\n__pycache__/\n*.[Bb][Ii][Nn]\n',
                    'build/a.obj': '', 'orig/1ST_READ.BIN': '', 'orig/.gitkeep': '', 'tools/wibo-i686': '',
                    'tools/__pycache__/a.pyc': '', 'src/a.bin': '', '.git/HEAD': ''})
        self.assertEqual(tree(), ['.gitignore', GUIDE, 'config/a/splits.txt', 'orig/.gitkeep', 'src/a.c',
                                  'tools/SHA256SUMS'])


class Patched(TreeTest):
    def setUp(self):
        super().setUp()
        self.write('config/compilers.mk', 'PATCHED := a.c b.c\na.c.patches := align-pool\n'
                                          'b.c.patches := drop-inline-align align-pool\n')

    def test_accepts_an_entry_that_names_each_patch(self):
        self.write('TOOLCHAIN.md', '## Patched sources\n\n- **`a.c`** gets align-pool.\n'
                                   '- **`b.c`** gets [drop-inline-align](x), then align-pool.\n\n## Images\n')
        self.assertEqual(unexplained(), [])

    def test_names_a_source_without_an_entry_and_a_patch_its_entry_lacks(self):
        self.write('TOOLCHAIN.md', '## Patched sources\n\n- **`b.c`** gets align-pool.\n\n## Images\n'
                                   '- **`a.c`** gets align-pool.\n')
        self.assertEqual(unexplained(), [
            "a.c: patched, but TOOLCHAIN.md's Patched sources has no entry for it",
            "b.c: its entry in TOOLCHAIN.md's Patched sources doesn't name drop-inline-align"])


class Tree(unittest.TestCase):
    def test_every_patched_source_has_its_entry(self):
        self.assertEqual(unexplained(ROOT), [])

    def test_every_citation_and_quotation_in_the_tree_holds(self):
        home = os.getcwd()
        os.chdir(ROOT)
        try:
            paths = tree()
            found = unresolved(paths) + misquoted(paths)
        finally:
            os.chdir(home)
        if found:
            self.fail('\n'.join(found))


if __name__ == '__main__':
    unittest.main()
