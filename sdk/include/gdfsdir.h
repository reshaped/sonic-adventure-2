/* the file system's directories. a directory is read into its table, and a
 * table or the disc's path table is searched */
#ifndef GDFSDIR_H
#define GDFSDIR_H
#include <sg_xpt.h>
#include <shinobi.h>

/* read the directory at frame address fad into gf_dirrec, a sector at a time
 * through buf. size need only be above 0: the directory's own entry, its
 * first, gives the bytes to read. it reads nothing where gf_dirrec was read
 * from fad already.
 *
 * it returns GDD_ERR_OK. it returns GDD_ERR_DIROVER, with gf_dirrec full,
 * where the directory holds more entries than gf_dirrec has room for. a
 * failed read returns the read's error. each failure leaves gf_dirrec's
 * dir_fad 0 */
Sint32 gdFsGetDirEnt(Uint8 *buf, Sint32 fad, GDFS_DIRREC gf_dirrec,
                     Sint32 size);
/* the position of name's entry in gf_dirrec, from 1, or 0 for none. names
 * compare exactly where exact is TRUE, and without regard to case otherwise */
Sint32 gdFsSearchName(const char *name, GDFS_DIRREC gf_dirrec, Bool exact);
/* search the path table of size bytes at frame address fad for the
 * directory path. it returns the directory's ISO 9660 sector number, or 0 for
 * none and for a failed read. path's first character, a slash or a
 * backslash, is skipped. slashes and backslashes separate its names, which
 * match whatever their case.
 *
 * buf takes the table a sector at a time. where a record spans two
 * sectors, its first part goes to the bytes just below buf. those bytes must
 * be free */
Sint32 gdFsSearchPath(Uint8 *buf, Sint32 fad, Sint32 size, const char *path);
/* search as gdFsSearchPath does, for path within first_name, a directory of
 * the root */
Sint32 gdFsSearchPath_dds(Uint8 *buf, Sint32 fad, Sint32 size, const char *path,
                          const char *first_name);
/* copy size bytes from src to dst, a byte at a time. unlike memcpy's, its
 * source comes first */
void gdFsMemcpy(const void *src, void *dst, Sint32 size);

#endif
