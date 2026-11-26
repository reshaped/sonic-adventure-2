"""Validate the SH4ZAM splash's unpacked PVM archive against engine/dc/pvm.c
and verify the decoded 512x256 RGB565 texel payload."""

import ctypes
import hashlib
import struct
import tempfile
import unittest
from pathlib import Path

from support import compile_host

EXPECTED_PVM_BYTES = 64 + 512 * 256 * 2
EXPECTED_TEXELS_SHA256 = '43e8f121bd91745ee43fa1dd81093f0c5b0792a8d948afd8aff67648aab0d4f4'

STUBS = """
#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>

void *syMalloc(Uint32 nbytes) { return malloc(nbytes); }
void *syCalloc(Uint32 n, Uint32 size) { return calloc(n, size); }
void syFree(void *ap) { free(ap); }
void njDrawSprite2D(NJS_SPRITE *sp, Int n, Float pri, Uint32 attr)
{
    (void)sp; (void)n; (void)pri; (void)attr;
}
Sint32 njLoadTexturePvmMemory(Uint8 *addr, NJS_TEXLIST *texlist)
{
    (void)addr; (void)texlist;
    return 1;
}
Sint32 njReleaseTexture(NJS_TEXLIST *texlist)
{
    (void)texlist;
    return 1;
}
Sint32 njReleaseTextureLN(NJS_TEXMEMLIST *addr)
{
    (void)addr;
    return 1;
}
NJS_TEXERRSTAT *njGetTextureErrStat(void)
{
    static NJS_TEXERRSTAT err;
    return &err;
}
void njSetPvmTextureList(NJS_TEXLIST *texlist, NJS_TEXNAME *texname,
                         char *filename, Sint32 size)
{
    (void)texlist; (void)texname; (void)filename; (void)size;
}
void njSetTextureName(NJS_TEXNAME *texname, void *addr, Uint32 globalIndex,
                      Uint32 attr)
{
    (void)texname; (void)addr; (void)globalIndex; (void)attr;
}
"""


class Sh4zamSplash(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        directory = tempfile.TemporaryDirectory()
        cls.addClassCleanup(directory.cleanup)
        stubs = Path(directory.name) / 'stubs.c'
        stubs.write_text(STUBS)
        library = Path(directory.name) / 'splash.so'
        compile_host(library, '-std=c89', '-O2', '-I.', '-Isrc',
                     'engine/dc/sh4zam_splash.c', 'engine/dc/pvm.c',
                     'sdk/ninja/njCheckTextureFormat.c',
                     'sdk/ninja/njTextureGlobal.c', stubs, shared=True)
        cls.lib = ctypes.CDLL(str(library))
        cls.lib.sh4zam_splash_pvm_size.restype = ctypes.c_uint32
        cls.lib.sh4zam_splash_build_pvm.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
        cls.lib.sh4zam_splash_build_pvm.restype = ctypes.c_int
        cls.lib.texture_pvm_count.argtypes = [ctypes.c_void_p, ctypes.c_uint32]
        cls.lib.texture_pvm_count.restype = ctypes.c_uint32

    def test_pvm_passes_checked_loader_and_decodes_expected_texels(self):
        size = self.lib.sh4zam_splash_pvm_size()
        self.assertEqual(size, EXPECTED_PVM_BYTES)

        # Reject NULL and undersized buffers.
        self.assertEqual(self.lib.sh4zam_splash_build_pvm(None, size), 0)
        buf = ctypes.create_string_buffer(size)
        self.assertEqual(self.lib.sh4zam_splash_build_pvm(buf, size - 1), 0)

        # Build the full PVM archive and validate it through engine/dc/pvm.c.
        self.assertEqual(self.lib.sh4zam_splash_build_pvm(buf, size), 1)
        self.assertEqual(self.lib.texture_pvm_count(buf, size), 1)
        self.assertEqual(self.lib.texture_pvm_count(buf, size - 1), 0)

        raw = buf.raw
        # PVMH chunk (48 bytes total, padded for 32-byte-aligned texels at offset 64).
        self.assertEqual(raw[0:4], b'PVMH')
        pvmh_size, status, count, entry_num, global_index = struct.unpack_from('<IHHHI', raw, 4)
        self.assertEqual(pvmh_size, 40)
        self.assertEqual(status, 0x0008)
        self.assertEqual(count, 1)
        self.assertEqual(entry_num, 0)
        self.assertEqual(global_index, 0x5348345a)

        # PVRT chunk (512x256 linear RGB565 rectangle texture).
        self.assertEqual(raw[48:52], b'PVRT')
        pvrt_size, tex_type, width, height = struct.unpack_from('<IIHH', raw, 52)
        self.assertEqual(pvrt_size, 8 + 512 * 256 * 2)
        self.assertEqual(tex_type, 0x0901)
        self.assertEqual((width, height), (512, 256))

        texels = raw[64:]
        self.assertEqual(len(texels), 512 * 256 * 2)
        # Right padding columns (384..511) must be pure black on every row.
        pad_row = bytes((512 - 384) * 2)
        for y in range(256):
            start = (y * 512 + 384) * 2
            self.assertEqual(texels[start:start + len(pad_row)], pad_row)

        self.assertEqual(hashlib.sha256(texels).hexdigest(), EXPECTED_TEXELS_SHA256)
