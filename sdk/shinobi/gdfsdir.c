/* the GD file system's directory reads and searches */
#include <gdfs_lif.h>
#include <gdfsdir.h>
#include <sg_xpt.h>
#include <shinobi.h>

/* byte offsets in an ISO 9660 directory record. the record holds its
 * extent and size in both byte orders. DR_EXTENT and DR_SIZE are the
 * little-endian halves */
#define DR_LEN 0
#define DR_EXTENT 2
#define DR_SIZE 10
#define DR_FLAGS 25
#define DR_NAME_LEN 32
#define DR_NAME 33
/* the shortest record with a name */
#define DR_MIN_LEN 34
/* the one-byte names of a directory's own record and its parent's */
#define DR_NAME_SELF 0
#define DR_NAME_PARENT 1
/* the bytes of the version that ends a file's name: ;1 */
#define VERSION_LEN 2

/* a record of the type L ISO 9660 path table, whose numbers are
 * little-endian */
struct path_record {
    Uint8 name_len;
    Uint8 ext_len; /* the extended attribute record's length */
    Uint8 extent[4]; /* the directory's first sector */
    Uint8 parent[2]; /* the parent's record number, from 1 */
    /* name_len bytes, with no NUL, and a pad byte after an odd name_len */
    char name[1];
};
/* the byte offset of struct path_record's name. SHC's side of the tree has
 * no stddef.h, and so no offsetof */
#define PT_NAME 8
/* the root directory's record number: the path table's first */
#define ROOT_RECORD 1

static char name_upper(char c)
{
    if (c >= 'a' && c <= 'z') {
        c -= 'a' - 'A';
    }
    return c;
}

void gdFsMemcpy(const void *src, void *dst, Sint32 size)
{
    const char *from = src;
    char *to = dst;

    while (size > 0) {
        *to++ = *from++;
        size--;
    }
}

/* whether name and str match in their first size characters, ignoring
 * case. where name ends sooner, str must end at the same place */
static Bool name_starts(const char *name, const char *str, Sint32 size)
{
    while (size > 0) {
        if (*name != '\0') {
            if (name_upper(*name++) != name_upper(*str++)) {
                return FALSE;
            }
        } else {
            if (*str == '\0') {
                return TRUE;
            }
            return FALSE;
        }
        size--;
    }
    return TRUE;
}

static Bool name_same(const char *name, const char *str)
{
    while (*name != '\0') {
        if (*name++ != *str++) {
            return FALSE;
        }
    }
    if (*str != '\0') {
        return FALSE;
    }
    return TRUE;
}

static Bool name_same_any_case(const char *name, const char *str)
{
    while (*name != '\0') {
        if (name_upper(*name++) != name_upper(*str++)) {
            return FALSE;
        }
    }
    if (*str != '\0') {
        return FALSE;
    }
    return TRUE;
}

/* the length of the name that starts path. the name runs to the first
 * character that is in sep, or to path's end */
static Sint32 name_len(const char *path, const char *sep)
{
    const char *s;
    Sint32 len = 0;

    while (*path != '\0') {
        for (s = sep; *s != '\0'; s++) {
            if (*path == *s) {
                return len;
            }
        }
        path++;
        len++;
    }
    return len;
}

/* a name's search ID: one byte, the same for names that differ only in
 * case. gdFsSearchName compares IDs before it compares names */
static Sint32 name_sid(const char *name)
{
    Uint32 sum = 218;

    while (*name != '\0') {
        sum += (Uint8)name_upper(*name++);
    }
    return (Uint8)(sum >> 2);
}

/* copy the directory record at dr into ent, and return the record's length.
 * it returns 0 for a length byte below DR_MIN_LEN, as the zeros after a
 * sector's last record are. ent is then left as it was */
static Sint32 read_dirrec(Uint8 *dr, GDFS_DIRREC_ENT *ent)
{
    Sint32 len;

    if (dr[DR_LEN] < DR_MIN_LEN) {
        return 0;
    }
    gdFsMemcpy(dr + DR_EXTENT, &ent->fad, sizeof(ent->fad));
    gdFsMemcpy(dr + DR_SIZE, &ent->fsize, sizeof(ent->fsize));
    ent->flag = dr[DR_FLAGS];
    len = dr[DR_NAME_LEN];
    if (!(ent->flag & GDD_FF_DIRECTORY)) {
        len -= VERSION_LEN;
    }
    gdFsMemcpy(dr + DR_NAME, ent->fname, len);
    ent->fname[len] = '\0';
    if (ent->flag & GDD_FF_DIRECTORY) {
        if (ent->fname[0] == DR_NAME_SELF) {
            ent->fname[0] = '.';
        } else if (ent->fname[0] == DR_NAME_PARENT) {
            ent->fname[0] = '.';
            ent->fname[1] = '.';
            ent->fname[2] = '\0';
        }
    }
    ent->sid = name_sid((const char *)ent->fname);
    return dr[DR_LEN];
}

Sint32 gdFsGetDirEnt(Uint8 *buf, Sint32 fad, GDFS_DIRREC gf_dirrec, Sint32 size)
{
    Uint8 *dr;
    GDFS_DIRREC_ENT *ent = gf_dirrec->dirrec_tbl;
    Uint8 *end;
    Sint32 num;
    Sint32 max_ent;
    Sint32 result;

    if (gf_dirrec->dir_fad == fad) {
        return GDD_ERR_OK;
    }
    gf_dirrec->dir_fad = fad;
    num = 0;
    max_ent = gf_dirrec->max_ent;
    end = buf + GDD_FS_SCTSIZE;
#ifdef NONMATCHING
    /* GCC can't see that dr is set wherever it is read. the test below reads
     * it only where size is GDD_FS_SCTSIZE, and then the loop has run */
    dr = buf;
#endif
    while (size > 0) {
        result = gdFsReadWG(fad, 1, buf);
        if (result != GDD_ERR_OK) {
            gf_dirrec->dir_fad = 0;
            return result;
        }
        dr = buf;
        while (num < max_ent) {
            /* @hack spelling: the assignment inside the test, where a
             * statement of its own would do */
            if ((result = read_dirrec(dr, ent)) == 0) {
                break;
            }
            if (num == 0 && ent->fsize != size) {
                size = ent->fsize;
            }
            dr += result;
            ent++;
            num++;
            if (dr >= end) {
                break;
            }
        }
        if (num >= max_ent) {
            break;
        }
        size -= GDD_FS_SCTSIZE;
        fad++;
    }
    gf_dirrec->dir_num = num;
    /* with the table full, gdFsGetDirEnt returns GDD_ERR_DIROVER where a
     * record may be left unread. size holds the directory's bytes from the
     * sector last read on. a record may be left where size isn't one sector,
     * or where that sector holds one at dr */
    if (num == max_ent && size != 0) {
        if (size != GDD_FS_SCTSIZE) {
            /* @hack spelling: the goto into the test's body, where one test
             * joined by || would do */
            goto over;
        }
        if (dr < end) {
            if (*dr != 0) {
            over:
                gf_dirrec->dir_fad = 0;
                return GDD_ERR_DIROVER;
            }
        }
    }
    return GDD_ERR_OK;
}

Sint32 gdFsSearchName(const char *name, GDFS_DIRREC gf_dirrec, Bool exact)
{
    Sint32 dir_num = gf_dirrec->dir_num;
    GDFS_DIRREC_ENT *ent = gf_dirrec->dirrec_tbl;
    Uint8 sid = name_sid(name);
    Sint32 i;

    for (i = 0; i < dir_num; i++, ent++) {
        if (ent->sid == sid) {
            /* @hack spelling: each name test's result compared with TRUE,
             * where the result alone would do, since each gives only TRUE or
             * FALSE */
            if (exact == TRUE) {
                if (name_same(name, (const char *)ent->fname) == TRUE) {
                    return i + 1;
                }
            } else {
                if (name_same_any_case(name, (const char *)ent->fname) ==
                    TRUE) {
                    return i + 1;
                }
            }
        }
    }
    return 0;
}

Sint32 gdFsSearchPath(Uint8 *buf, Sint32 fad, Sint32 size, const char *path)
{
    Sint32 parent;
    Sint32 no;
    Sint32 len;
    const char *name;
    Uint8 *pt;
    Uint8 *end;
    struct path_record *cur;
    struct path_record *rec;
    Uint16 rec_parent;
    Sint32 result;
    Sint32 step;
    Sint32 done;

    /* @hack spelling: the six starting values in this order, where any order
     * would do */
    parent = ROOT_RECORD;
    done = 0;
    no = ROOT_RECORD;
    name = path + 1;
    pt = buf + GDD_FS_SCTSIZE;
    len = 0;
    /* @hack spelling: end taken through cur, where end = pt would do */
    cur = (struct path_record *)pt;
    end = (Uint8 *)cur;
    for (;;) {
        cur = (struct path_record *)pt;
        if (pt >= end ||
            cur->name_len + (cur->name_len & 1) + PT_NAME + pt >= end) {
            step = buf + GDD_FS_SCTSIZE - pt;
            cur = (struct path_record *)(buf - step);
            gdFsMemcpy(pt, cur, step);
            result = gdFsReadWG(fad, 1, buf);
            if (result != GDD_ERR_OK) {
                break;
            }
            pt = (Uint8 *)cur;
            fad++;
        }
        /* @hack spelling: rec, a second pointer to the record, where cur
         * would do */
        rec = cur;
        gdFsMemcpy(rec->parent, &rec_parent, sizeof(rec_parent));
        if (rec_parent == parent &&
            (no == ROOT_RECORD ||
             (rec->name_len == len && name_starts(rec->name, name, len)))) {
            name += len;
            parent = no;
            if (*name == '\0') {
                return gdFsGetLE4(rec->extent);
            }
            if (no != ROOT_RECORD) {
                name++;
            }
            len = name_len(name, "/\\");
        }
        no++;
        /* @hack spelling: the step assigned inside the count's sum, where a
         * statement of its own would do */
        done += step = rec->name_len + (rec->name_len & 1) + PT_NAME;
        pt += step;
        if (done >= size) {
            break;
        }
    }
    return 0;
}

Sint32 gdFsSearchPath_dds(Uint8 *buf, Sint32 fad, Sint32 size, const char *path,
                          const char *first_name)
{
    Sint32 parent;
    Sint32 no;
    Sint32 done;
    Sint32 len;
    Bool first;
    Sint32 first_len;
    Sint32 want;
    const char *match;
    const char *s;
    const char *name;
    Uint8 *pt;
    Uint8 *end;
    struct path_record *cur;
    struct path_record *rec;
    Uint16 rec_parent;
    Sint32 step;
    Sint32 result;

    name = path;
    first = TRUE;
    first_len = 0;
    s = first_name;
    while (*s++ != '\0') {
        first_len++;
    }
    /* @hack spelling: the starting values in this order, where any order
     * would do */
    name++;
    parent = ROOT_RECORD;
    no = ROOT_RECORD;
    done = 0;
    pt = buf + GDD_FS_SCTSIZE;
    /* @hack spelling: end taken through cur, where end = pt would do */
    cur = (struct path_record *)pt;
    end = (Uint8 *)cur;
    len = 0;
    do {
        cur = (struct path_record *)pt;
        if (pt >= end ||
            cur->name_len + (cur->name_len & 1) + PT_NAME + pt >= end) {
            step = buf + GDD_FS_SCTSIZE - pt;
            cur = (struct path_record *)(buf - step);
            gdFsMemcpy(pt, cur, step);
            result = gdFsReadWG(fad, 1, buf);
            if (result != GDD_ERR_OK) {
                break;
            }
            pt = (Uint8 *)cur;
            fad++;
        }
        /* @hack spelling: rec, a second pointer to the record, where cur
         * would do */
        rec = cur;
        gdFsMemcpy(rec->parent, &rec_parent, sizeof(rec_parent));
        if (rec_parent == parent) {
            if (no == ROOT_RECORD) {
                parent = no;
            } else {
                if (first == TRUE) {
                    want = first_len;
                    match = first_name;
                } else {
                    want = len;
                    match = name;
                }
                if (rec->name_len == want &&
                    name_starts((const char *)rec->name, match, want)) {
                    if (!first) {
                        name += len;
                    }
                    if (*name == '\0') {
                        return gdFsGetLE4(rec->extent);
                    }
                    if (first == TRUE) {
                        /* @hack spelling: first cleared in a loop of one
                         * pass, where the statement alone would do */
                        for (;;) {
                            first = FALSE;
                            break;
                        }
                    } else if (no != ROOT_RECORD) {
                        name++;
                    }
                    len = name_len(name, "/\\");
                    parent = no;
                }
            }
        }
        /* @hack spelling: no stepped in a loop of one pass, where the
         * statement alone would do */
        for (;;) {
            no++;
            break;
        }
        /* @hack spelling: the step and the count in one statement, where two
         * would do */
        step = rec->name_len + (rec->name_len & 1) + PT_NAME, done += step;
        pt += step;
    } while (done < size);
    return 0;
}
