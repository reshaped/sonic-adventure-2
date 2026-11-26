/* MPEG video start codes: what each one is, and the searches for them */
#include <sg_xpt.h>
#include <sofdec.h>

Sint32 MPV_CheckDelim(const Uint8 *buffer)
{
    /* @hack spelling: cursor, a copy of buffer, where buffer would do */
    const Uint8 *cursor = buffer;
    Sint32 code;
    Sint32 kind;

    code = *cursor++;
    code <<= 8;
    code |= *cursor++;
    code <<= 8;
    code |= *cursor++;
    /* the last shift can pass Sint32's range, so it shifts code unsigned */
    code = (Uint32)code << 8;
    code |= *cursor;
    if (code == MPV_PICTURE_START_CODE) {
        kind = MPV_DELIM_PICTURE;
    } else if (code == MPV_FIRST_SLICE_CODE) {
        kind = MPV_DELIM_SLICE | MPV_DELIM_FIRST_SLICE;
    } else if (code > MPV_FIRST_SLICE_CODE && code <= MPV_LAST_SLICE_CODE) {
        kind = MPV_DELIM_SLICE;
    } else if (code == MPV_USER_DATA_START_CODE) {
        kind = MPV_DELIM_USER_DATA;
    } else if (code == MPV_SEQUENCE_HEADER_CODE) {
        kind = MPV_DELIM_SEQUENCE;
    } else if (code == MPV_EXTENSION_START_CODE) {
        kind = MPV_DELIM_EXTENSION;
    } else if (code == MPV_SEQUENCE_END_CODE) {
        kind = MPV_DELIM_END;
    } else if (code == MPV_GROUP_START_CODE) {
        kind = MPV_DELIM_GROUP;
    } else {
        kind = 0;
    }
    return kind;
}

/* matched counts the start code bytes read so far, backward from the code's
 * last. it is 1 once that byte, which can be any, is read. it is 2 once the
 * 01 before it is read, and 3 once a 00 is. a second 00 completes the code */
Uint8 *MPV_BsearchDelim(Uint8 *cursor, Sint32 length, Sint32 mask)
{
    Uint8 *begin;
    /* @hack redundant: matched declared register, where a plain local would
     * do */
    register int matched;
    Sint8 byte;

    matched = 0;
    begin = cursor - length;
    while (begin < cursor) {
        cursor--;
        byte = *cursor;
        switch (matched) {
        case 0:
            matched = 1;
            break;
        case 1:
            if (byte == 1) {
                matched = 2;
            }
            break;
        case 2:
            if (byte == 0) {
                matched = 3;
            } else if (byte != 1) {
                matched = 1;
            }
            break;
        case 3:
            if (byte == 0) {
                if (MPV_CheckDelim(cursor) & mask) {
                    return cursor;
                }
                matched = 0;
            } else if (byte != 1) {
                matched = 1;
            } else {
                matched = 2;
            }
            break;
        }
    }
    return NULL;
}

/* matched counts the bytes of 00 00 01 read so far, here and in
 * MPV_CopyStm. the byte read while matched is 3 is the code's last */
Uint8 *MPV_SearchDelim(Uint8 *cursor, Sint32 length, Sint32 mask)
{
    Uint8 *end;
    Uint8 *start;
    /* @hack redundant: matched and byte declared register, where plain locals
     * would do */
    register int matched;
    register Sint8 byte;

    matched = 0;
    end = cursor + length;
    while (cursor < end) {
        byte = *cursor++;
        switch (matched) {
        case 0:
            if (byte == 0) {
                matched = 1;
            }
            break;
        case 1:
            if (byte == 0) {
                matched = 2;
            } else {
                matched = 0;
            }
            break;
        case 2:
            if (byte == 1) {
                matched = 3;
            } else if (byte != 0) {
                matched = 0;
            }
            break;
        case 3:
            start = cursor - MPV_START_CODE_BYTES;
            if (MPV_CheckDelim(start) & mask) {
                return start;
            }
            matched = 0;
            break;
        }
    }
    return NULL;
}

Uint8 *MPV_CopyStm(Uint8 *destination, Uint8 *source, Sint32 length,
                   Sint32 mask)
{
    Uint8 *end;
    /* @hack redundant: matched and byte declared register, where plain locals
     * would do */
    register int matched;
    register Sint8 byte;

    matched = 0;
    end = source + length;
    while (source < end) {
        byte = *source++;
        *destination++ = byte;
        switch (matched) {
        case 0:
            if (byte == 0) {
                matched = 1;
            }
            break;
        case 1:
            if (byte == 0) {
                matched = 2;
            } else {
                matched = 0;
            }
            break;
        case 2:
            if (byte == 1) {
                matched = 3;
            } else if (byte != 0) {
                matched = 0;
            }
            break;
        case 3:
            if (MPV_CheckDelim(source - MPV_START_CODE_BYTES) & mask) {
                return source;
            }
            matched = 0;
            break;
        }
    }
    return source;
}
