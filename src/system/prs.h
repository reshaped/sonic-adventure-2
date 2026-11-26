/* decoding PRS, the LZ77 that packs the disc's files */
#ifndef SYSTEM_PRS_H
#define SYSTEM_PRS_H
#include <sg_xpt.h>

/* decode the PRS stream at from into to, and return the decoded size in
 * bytes. it checks nothing: the stream must end in its end marker, no copy
 * may reach back past to, and to must hold the output */
Sint32 prs_decompress(const void *from, void *to);

#ifdef NONMATCHING
/* Validate a complete stream before decoding it. A null destination measures
 * its decoded size within capacity. The consumed input ends at the PRS end
 * marker and may be shorter than input_size. When decoding, the consumed
 * input and produced output must be disjoint. Optional, separate size outputs
 * receive zero on failure. */
Bool prs_decompress_checked(const void *from, Uint32 input_size, void *to,
                            Uint32 capacity, Uint32 *consumed, Uint32 *written);

enum prs_in_place_result {
    /* buffer is null, the input passes capacity, or the stream fails
     * validation */
    PRS_INVALID,
    PRS_DECODED,
    /* decoding in place would write over input before reading it */
    PRS_OVERRUN
};

/* decode the stream in buffer[offset..offset + input_size) into
 * buffer[0..capacity), validating it as prs_decompress_checked does.
 * PRS_INVALID and PRS_OVERRUN leave the buffer as it was. on PRS_DECODED and
 * PRS_OVERRUN, *consumed gets the stream's size through its end marker and
 * *written its decoded size. on PRS_INVALID both get 0. consumed and written
 * must not be null */
enum prs_in_place_result
prs_decompress_in_place(void *buffer, Uint32 capacity, Uint32 offset,
                        Uint32 input_size, Uint32 *consumed, Uint32 *written);
#endif

#endif
