"""Write make's initializer inputs for all selected translation units in one scan."""
import argparse
from pathlib import Path

from body_data import declares_automatic, include_root, source_inputs
from config import SOURCE, fail, rows, save


def write_rules(output, sources):
    # A body shared by multiple programs takes its values from the first placing
    # image in name order. Each member still links and checks at its own address.
    sources = dict.fromkeys(sources)
    wanted = {str(source) for source in sources}
    owners = {}
    for table in sorted(Path('config').glob('*/splits.txt')):
        for _, fields in rows(table):
            if len(fields) == 4 and fields[0] in wanted:
                owners.setdefault(fields[0], table.parent.name)

    rules = []
    for source in sources:
        if not SOURCE.fullmatch(str(source)) or source.suffix != '.c':
            fail(f'{source}: expected a project C translation unit')
        if str(source) not in owners:
            fail(f'{source}: no placement in config/*/splits.txt')
        texts = {}
        headers, generated = source_inputs(source, generated_headers=True, texts=texts)
        if declares_automatic(source, texts):
            rules.append(f'body_listing.{source} := source,object')
        if not generated:
            fail(f'{source}: no initializer found in its includes')
        rules.extend((f'body_image.{source} := {owners[str(source)]}',
                      f'body_directory.{source} := {include_root(source, generated)}',
                      f'body_headers.{source} := ' + ' '.join(map(str, headers)),
                      f'body_files.{source} := ' + ' '.join(map(str, generated))))
    data = ('\n'.join(rules) + '\n').encode()
    if not output.is_file() or output.read_bytes() != data:
        save(output, data)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='body_inputs', description=__doc__)
    parser.add_argument('output', type=Path)
    parser.add_argument('sources', type=Path, nargs='+')
    args = parser.parse_args()
    write_rules(args.output, args.sources)
