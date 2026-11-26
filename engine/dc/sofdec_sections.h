#ifndef DC_SOFDEC_SECTIONS_H
#define DC_SOFDEC_SECTIONS_H

#include <stdint.h>

/* PSGSFD00 to PSGSFD15 are the sections of Sofdec's movie decoder, each
 * PSGSFD_ALIGN-byte aligned. PSGSFD00 and PSGSFD02 hold its code, and PSGSFD01
 * holds mpvasec.src's PSGSFD01_BYTES zeros. PSGSFD03 to PSGSFD15 are empty */
#define PSGSFD_SECTIONS 16
#define PSGSFD_ALIGN 4
#define PSGSFD01_BYTES 1582

/* the sections that hold anything, by index, and their count */
enum { PSGSFD00, PSGSFD01, PSGSFD02, PSGSFD_FILLED_SECTIONS };

/* the sections' starts, ends and sizes in bytes, one array each. strt1.src
 * writes each array's words in section order. each array takes the name of its
 * first word, PSGSFD00's. an end is the address past a section's last byte */
extern const uintptr_t _START_PSGSFD00[PSGSFD_SECTIONS];
extern const uintptr_t _END_PSGSFD00[PSGSFD_SECTIONS];
extern const uint32_t _SIZE_PSGSFD00[PSGSFD_SECTIONS];

/* the check GCC's mpv_lib.c makes in place of MPVASEC_CheckSectionSiz. SHC's
 * code for PSGSFD00 and PSGSFD02 is 6610 and 24576 bytes.
 * MPVASEC_CheckSectionSiz wants each section's size within 0 to 3 or 8 to 11
 * bytes of SHC's. GCC's code has sizes of its own, so this check wants no size
 * of either */
static int mpv_check_section_sizes(void)
{
    unsigned int section;

    for (section = PSGSFD00; section < PSGSFD_FILLED_SECTIONS; ++section) {
        uintptr_t start = _START_PSGSFD00[section];
        uintptr_t end = _END_PSGSFD00[section];

        if (start % PSGSFD_ALIGN != 0 || end <= start ||
            end - start != _SIZE_PSGSFD00[section]) {
            return -1;
        }
    }
    return _SIZE_PSGSFD00[PSGSFD01] == PSGSFD01_BYTES ? 0 : -1;
}

#endif
