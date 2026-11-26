/* the body of ahxbsr_fill_word and ahxbsr_fill_word_2, which the image holds
 * twice. AHXBSR_FILL names the function each inclusion defines. each takes a
 * chunk of up to WORD_BYTES from the joint's data. it shifts into bits the
 * whole bytes that fit, frees them, and gives the rest back */
void AHXBSR_FILL(AHXBSR reader)
{
    SJCK chunk;
    SJCK tail;
    Sint32 room;
    Sint32 bytes;
    Sint32 i;
    /* @hack redundant: bits and next declared register, where plain locals
     * would do */
    register Uint32 bits;
    register Uint8 *next;

    SJ_GetChunk(reader->joint, SJ_LIN_DATA, WORD_BYTES, &chunk);
    if (chunk.len <= 0) {
        SJ_UngetChunk(reader->joint, SJ_LIN_DATA, &chunk);
        return;
    }
    room = WORD_BITS - reader->available;
    bytes = chunk.len < (room >> 3) ? chunk.len : (room >> 3);
    bits = reader->bits;
    next = (Uint8 *)chunk.data;
    for (i = 0; i < bytes; i++) {
        bits = (bits << BYTE_BITS) | *next++;
    }
    reader->available += bytes * BYTE_BITS;
    reader->bits = bits;
    SJ_SplitChunk(&chunk, bytes, &chunk, &tail);
    SJ_PutChunk(reader->joint, SJ_LIN_FREE, &chunk);
    SJ_UngetChunk(reader->joint, SJ_LIN_DATA, &tail);
}
