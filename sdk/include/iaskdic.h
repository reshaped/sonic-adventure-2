/* ASK's dictionary file: one read-only stream over the dictionary ROM.
 * FAskRom_Open starts it at its first byte, whatever filename and mode, and
 * returns 1. FAskRom_Read returns the bytes it read, and FAskRom_Seek the new
 * offset. a write returns -1. so does a seek from the start or the cursor to
 * before the start. so does a seek from the cursor or the end to past the end
 */
#ifndef IASKDIC_H
#define IASKDIC_H
extern const char *ntgDicBuild;
int FAskRom_Open(char *filename, int mode);
int FAskRom_Close(int stream);
int FAskRom_Read(int stream, unsigned char *buffer, int length);
int FAskRom_Write(int stream, unsigned char *buffer, int length);
/* FAskRom_Seek's origins */
enum { ASK_FROM_START, ASK_FROM_CURSOR, ASK_FROM_END };
long FAskRom_Seek(int stream, long offset, int origin);
#endif
