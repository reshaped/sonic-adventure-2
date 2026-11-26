"""A stalled optical utility must leave the last checked publication available."""
from hashlib import sha256
import json
import os
from pathlib import Path
from unittest import mock

import dc_publish
import disc
from dc_manifest import collect
from support import TreeTest


@mock.patch.dict(os.environ, {'HUNG': '0.3s'})
class PublicationTimeouts(TreeTest):
    def executable(self, path, script):
        self.write(path, '#!/bin/sh\n' + script)
        Path(path).chmod(0o755)

    def test_pack_and_readback_timeout_preserve_program_and_receipt(self):
        root = Path('release')
        output = root / 'STAGE.PRS'
        receipt = root / 'receipts/stage.txt'
        self.image('stage', 'file STAGE.PRS\nP 0x8c500000 0x8c500004\n')
        self.write(output, b'previous program')
        self.write(receipt, b'previous receipt')
        self.executable('build/prskit', 'cp "$2" "$3"\nif [ "$1" = "$STALL" ]; then exec sleep 5; fi\n')
        with mock.patch.object(dc_publish, 'Elf'), mock.patch.object(dc_publish, 'payload', return_value=b'new'):
            for operation in ('compress', 'decompress'):
                with self.subTest(operation=operation), mock.patch.dict(os.environ, {'STALL': operation}):
                    error = self.fails(dc_publish.publish, 'stage', root, Path('build/prskit'))
                    self.assertRegex(error, f'prskit {operation} .*timed out')
                    self.assertEqual(output.read_bytes(), b'previous program')
                    self.assertEqual(receipt.read_bytes(), b'previous receipt')
                    self.assertEqual(set(root.iterdir()), {output, receipt.parent})

    def test_manifest_timeout_preserves_checked_manifest(self):
        self.image('stage', 'file STAGE.PRS\nP 0x8c500000 0x8c500004\n')
        self.write('release/images/stage.elf', b'linked')
        self.write('release/STAGE.PRS', b'optical')
        hashes = [sha256(content).hexdigest() for content in (b'linked', b'decoded', b'optical')]
        self.write('release/receipts/stage.txt', '  '.join(['stage', *hashes, 'STAGE.PRS']) + '\n')
        manifest = Path('release/manifest.txt')
        self.write(manifest, b'previous manifest')
        self.executable('build/prskit', 'exec sleep 5\n')
        self.assertRegex(self.fails(collect, manifest), 'prskit decompress .*timed out')
        self.assertEqual(manifest.read_bytes(), b'previous manifest')

    def test_gdi_timeout_discards_staged_track_and_reports_its_diagnostic(self):
        output = Path('release/disc')
        self.write(output / 'track.bin', b'previous track')
        self.write(output / disc.RECEIPT, json.dumps({'format': disc.FORMAT, 'files': ['track.bin']}))
        self.executable('build/gdi', 'mkdir "$3"\nprintf partial > "$3/track.bin"\n'
                                     'echo "copying track" >&2\nexec sleep 5\n')
        with self.assertRaises(SystemExit) as stopped:
            with disc.staged_disc(output) as staged:
                disc.gdi('rebuild', 'retail.gdi', staged)
        self.assertIn('gdi rebuild retail.gdi', str(stopped.exception))
        self.assertIn('timed out', str(stopped.exception))
        self.assertIn('copying track', str(stopped.exception))
        self.assertEqual((output / 'track.bin').read_bytes(), b'previous track')
        self.assertEqual(set(output.parent.iterdir()), {output, output.parent / '.disc.lock'})
