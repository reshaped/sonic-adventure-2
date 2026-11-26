/* the GCC build's checked PRS decoders, which walk a stream whole before they
 * write. src/system/prs.src describes the stream's items */
#include <sg_xpt.h>
#include <stdint.h>

#include "system/prs.h"

struct prs_reader {
    const Uint8 *input;
    Uint32 size;
    Uint32 position;
    unsigned int control;
    unsigned int bits_left;
};

/* a stream's extent, through its end marker */
struct prs_extent {
    Uint32 input; /* the stream's bytes */
    Uint32 output; /* the bytes it decodes to */
    /* the most the output leads the input at the end of any item: bytes
     * decoded less bytes read. an item reads all its input before it writes.
     * so the stream decodes in place where it starts lead bytes or more past
     * the output */
    Uint32 lead;
};

/* the next control bit, from the low end of the control byte. it returns -1
 * where the input ends before the control byte it needs */
static int next_bit(struct prs_reader *reader)
{
    int bit;

    if (reader->bits_left == 0) {
        if (reader->position == reader->size) {
            return -1;
        }
        reader->control = reader->input[reader->position++];
        reader->bits_left = 8;
    }
    bit = reader->control & 1U;
    reader->control >>= 1;
    reader->bits_left--;
    return bit;
}

/* decode the stream in input[0..size) into output[0..capacity), or with
 * output null measure it. on TRUE *extent describes the stream. FALSE is a
 * stream that ends early, copies from before the output's start, or outgrows
 * capacity */
static Bool decode(const Uint8 *input, Uint32 size, Uint8 *output,
                   Uint32 capacity, struct prs_extent *extent)
{
    struct prs_reader reader;
    Uint32 produced = 0;
    Uint32 lead = 0;
    Uint32 distance, length, i;
    int first, second;
    Uint16 word;

    reader.input = input;
    reader.size = size;
    reader.position = 0;
    reader.control = 0;
    reader.bits_left = 0;
    for (;;) {
        if (produced > reader.position && produced - reader.position > lead) {
            lead = produced - reader.position;
        }
        first = next_bit(&reader);
        if (first < 0) {
            return FALSE;
        }
        if (first) {
            if (reader.position == size || produced == capacity) {
                return FALSE;
            }
            if (output != NULL) {
                output[produced] = input[reader.position];
            }
            reader.position++;
            produced++;
            continue;
        }
        second = next_bit(&reader);
        if (second < 0) {
            return FALSE;
        }
        if (second) {
            if (size - reader.position < 2) {
                return FALSE;
            }
            word = input[reader.position] |
                   ((Uint16)input[reader.position + 1] << 8);
            reader.position += 2;
            if (word == 0) {
                extent->input = reader.position;
                extent->output = produced;
                extent->lead = lead;
                return TRUE;
            }
            distance = 8192U - (word >> 3);
            length = word & 7U;
            if (length != 0) {
                length += 2;
            } else {
                if (reader.position == size) {
                    return FALSE;
                }
                length = input[reader.position++] + 1U;
            }
        } else {
            first = next_bit(&reader);
            second = next_bit(&reader);
            if (first < 0 || second < 0 || reader.position == size) {
                return FALSE;
            }
            length = 2 + 2 * first + second;
            distance = 256U - input[reader.position++];
        }
        if (distance > produced || length > capacity - produced) {
            return FALSE;
        }
        if (output != NULL) {
            for (i = 0; i < length; i++) {
                output[produced + i] = output[produced + i - distance];
            }
        }
        produced += length;
    }
}

Bool prs_decompress_checked(const void *from, Uint32 input_size, void *to,
                            Uint32 capacity, Uint32 *consumed, Uint32 *written)
{
    struct prs_extent extent;
    uintptr_t source = (uintptr_t)from;
    uintptr_t destination = (uintptr_t)to;

    if (consumed != NULL) {
        *consumed = 0;
    }
    if (written != NULL) {
        *written = 0;
    }
    if (from == NULL || !decode(from, input_size, NULL, capacity, &extent)) {
        return FALSE;
    }
    if (to != NULL && extent.output != 0) {
        if (destination >= source ? destination - source < extent.input
                                  : source - destination < extent.output) {
            return FALSE;
        }
        if (!decode(from, extent.input, to, extent.output, &extent)) {
            return FALSE;
        }
    }
    if (consumed != NULL) {
        *consumed = extent.input;
    }
    if (written != NULL) {
        *written = extent.output;
    }
    return TRUE;
}

enum prs_in_place_result
prs_decompress_in_place(void *buffer, Uint32 capacity, Uint32 offset,
                        Uint32 input_size, Uint32 *consumed, Uint32 *written)
{
    struct prs_extent extent;
    const Uint8 *input;

    *consumed = *written = 0;
    if (buffer == NULL || offset > capacity || input_size > capacity - offset) {
        return PRS_INVALID;
    }
    input = (const Uint8 *)buffer + offset;
    if (!decode(input, input_size, NULL, capacity, &extent)) {
        return PRS_INVALID;
    }
    if (extent.lead > offset) {
        *consumed = extent.input;
        *written = extent.output;
        return PRS_OVERRUN;
    }
    if (!decode(input, extent.input, buffer, extent.output, &extent)) {
        return PRS_INVALID;
    }
    *consumed = extent.input;
    *written = extent.output;
    return PRS_DECODED;
}
