"""The network's protocols and the ASK keyboard, as the host runs their C: the harnesses of tests/network/ and
tests/access/."""

import hashlib
import importlib.util
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

from support import ROOT, compile_host, original_image


class AskCandidateFrees(unittest.TestCase):
    """Check that ASK frees each cell of the candidates a selection drops, with the portable IME."""

    def test_dropped_candidates_free_every_cell(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'candidate-frees'
            compile_host(program, '-std=c89', '-O2', '-Isdk/network', 'tests/network/candidate_frees.c',
                         'sdk/network/ask_select.c', 'sdk/network/ask_kouho.c')
            subprocess.run([program], check=True)


SPEC = importlib.util.spec_from_file_location('ask_lexicon', ROOT / 'tools/ask_lexicon.py')
lexicon = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(lexicon)


SAMPLE = ['あ\t24:3:0:亜\t40:1:-:阿', '', 'ねこ\t24:0:-:猫']
IMAGE = Path('orig/SONIC2/CHAOSTGKINDER.PRS')


class AskLexicon(unittest.TestCase):
    """Check the ASK lexicon through its native record format and page index."""

    def test_lookup_layout(self):
        rom = lexicon.pack(SAMPLE)
        first = b'\x0d\x00\x01\xa2\x18\x03\x00' + '亜'.encode('cp932')
        first += b'\x28\x01' + '阿'.encode('cp932')
        second = b'\x09\x00\x02\xcd\xb3\x18\x00' + '猫'.encode('cp932')
        self.assertEqual(rom[13 * 1024:14 * 1024], first.ljust(1024, b'\0'))
        self.assertEqual(rom[14 * 1024:], second.ljust(1024, b'\0'))
        self.assertEqual(rom[1024:1032], b'\x01\0\0\0\xcd\xb3\0\0')
        self.assertEqual(rom[78 + 11], 1)
        self.assertEqual(rom[78 + (0xcd - 0xa1) * 11 + 2], 8)

    def test_rejects_broken_entries(self):
        for rows in (['あ'], ['あ\t24:0:-:'], ['あ\t24:0:12:亜'],
                     ['あ\t24:0:-:亜'] * 2, ['あ\t24:0:-:' + '亜' * 511]):
            with self.subTest(rows=rows), self.assertRaises(ValueError):
                lexicon.pack(rows)

    def test_rejects_a_page_the_index_hides(self):
        rows = ['あ\t24:0:-:亜', '', 'ああああい\t24:0:-:亜', '', 'ああああう\t24:0:-:亜']
        with self.assertRaisesRegex(ValueError, 'line 5: '):
            lexicon.pack(rows)
        lexicon.pack(rows[:3] + rows[4:])

    def test_stops_at_the_page_limit(self):
        kana = [chr(code) for code in range(ord('ぁ'), ord('ゔ'))]
        rows = [line for first in kana for second in kana for line in (first + second + '\t24:0:-:亜', '')]
        lexicon.pack(rows[:2048])

        def lines():
            yield from rows[:2050]
            raise AssertionError('pack read past the 1025th page')
        with self.assertRaisesRegex(ValueError, 'line 2049: .* 1024 pages'):
            lexicon.pack(lines())

    def run_lookup(self, work, original=None):
        network = ROOT / 'sdk/network'
        generated = work / 'gen'
        dictionary = generated / 'sdk/network/ask_rom_data/ask_dictionary_rom.inc'
        for table in sorted((ROOT / 'offsets/sdk/network').glob('ask_*.txt')):
            source = Path('sdk/network') / table.with_suffix('.c').name
            if original is None and source.stem == 'ask_rom_data':
                # A short C initializer supplies the sample. The remaining ROM is zero-filled.
                dictionary.parent.mkdir(parents=True)
                rom = lexicon.pack(SAMPLE)
                dictionary.write_text(',\n'.join(','.join(str(b) for b in rom[i:i + 16])
                                                 for i in range(0, len(rom), 16)))
            else:
                args = ['--original', original] if original is not None else []
                subprocess.run([sys.executable, ROOT / 'tools/body_data.py', *args,
                                'chaostgkinder', source, generated], cwd=ROOT, check=True)
        if original is not None:
            rom = bytes(int(value) for value in dictionary.read_text().strip().rstrip(',').split(','))
            self.assertEqual(len(rom), 332 * 1024)
            self.assertEqual(hashlib.sha256(rom).hexdigest(),
                             'cb553ff5039ecd0947d4077395d41605076f7a8ca1b326160554e3275e90a596')
        runner = work / 'ask-words'
        compile_host(runner, '-std=c89', '-O1', '-ffreestanding', '-Wno-unknown-pragmas', '-I', network,
                     '-I', generated / 'sdk', *sorted(network.glob('ask_*.c')), 'tests/network/ask_words.c')
        subprocess.run([runner, 'sample'] if original is None else [runner], check=True)

    def test_lookup_two_sample_pages(self):
        with tempfile.TemporaryDirectory() as directory:
            self.run_lookup(Path(directory))

    @unittest.skipUnless((ROOT / IMAGE).is_file(), 'needs the original Kindergarten overlay')
    def test_lookup_across_all_retail_pages(self):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            self.run_lookup(work, original_image(IMAGE, work))


class IcmpMaskReply(unittest.TestCase):
    """Check the portable stack's ICMP mask reply: a short one is dropped, and a whole one is recorded."""

    def test_short_reply_is_dropped(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'mask-reply'
            compile_host(program, '-std=c89', '-O2', '-Isdk/access', 'tests/access/mask_reply.c')
            subprocess.run([program], check=True)


class IpReceive(unittest.TestCase):
    """Exercise IP_Recv's frees and the reassembly list with the portable stack."""

    def test_frees_and_reassembly_links(self):
        with tempfile.TemporaryDirectory() as directory:
            program = Path(directory) / 'ip-receive'
            compile_host(program, '-std=c89', '-O2', '-Isdk/access', 'tests/access/ip_receive.c')
            subprocess.run([program], check=True)


class IpcpRequest(unittest.TestCase):
    """Check outbound IPCP compression options and workspace handling."""

    def test_compression_and_frame_capacity(self):
        with tempfile.TemporaryDirectory() as directory:
            scratch = Path(directory)
            test = scratch / 'ipcp-request'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'chaostgkinder',
                            'sdk/network/ppp_ipcp.c', directory], cwd=ROOT, check=True)
            # Keep the SDK packet cursor while the fixture controls allocation.
            packet = scratch / 'packet.c'
            packet.write_text('#define PP_alloc_mbuf_work unused_allocator\n#include "ppp_buffer.c"\n')
            compile_host(test, '-std=c89', '-O2', '-Isdk/network', '-I', scratch / 'sdk',
                         'tests/network/ipcp_request.c', 'sdk/network/ppp_ipcp.c', 'sdk/network/ppp_bytes.c', packet)
            subprocess.run([test], check=True)


class LcpNegotiation(unittest.TestCase):
    """Exercise the portable LCP retry count across a failed control-frame write."""

    def test_retry_credit_survives_failed_send(self):
        with tempfile.TemporaryDirectory() as directory:
            test = Path(directory) / 'lcp-retry'
            subprocess.run([sys.executable, ROOT / 'tools/body_data.py', 'chaostgkinder',
                            'sdk/network/ppp_lcp.c', directory], cwd=ROOT, check=True)
            compile_host(test, '-std=c89', '-O2', '-Isdk/network', '-I', Path(directory) / 'sdk',
                         'tests/network/lcp_retry.c', 'sdk/network/ppp_bytes.c')
            subprocess.run([test], check=True)


if __name__ == '__main__':
    unittest.main()
