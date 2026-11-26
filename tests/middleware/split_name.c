/* cvfs_split_name on names whose parts fit its callers' buffers and names
 * whose parts don't. a name whose parts fit splits as the image splits it. a
 * part of CVFS_FNAME_MAX bytes or more leaves both parts empty, and the split
 * writes nothing past either buffer */
#include <assert.h>
#include <string.h>

#include "sg_cvfs.c"

/* a caller's two buffers, each with a guard after it that the split must
 * leave as it was */
struct parts {
    char devname[CVFS_FNAME_MAX];
    char devname_guard[16];
    char path[CVFS_FNAME_MAX];
    char path_guard[16];
};

static const char guard[16] = "untouched guard";

static void split(struct parts *parts, const char *fname)
{
    memset(parts, 'x', sizeof(*parts));
    memcpy(parts->devname_guard, guard, sizeof(guard));
    memcpy(parts->path_guard, guard, sizeof(guard));
    cvfs_split_name(parts->devname, parts->path, fname);
    assert(memcmp(parts->devname_guard, guard, sizeof(guard)) == 0);
    assert(memcmp(parts->path_guard, guard, sizeof(guard)) == 0);
}

/* the name prefix, then count bytes of c, then suffix */
static const char *name(const char *prefix, char c, Sint32 count,
                        const char *suffix)
{
    static char text[NAME_SCAN_MAX];
    Sint32 length = (Sint32)strlen(prefix);

    memcpy(text, prefix, length);
    memset(&text[length], c, count);
    strcpy(&text[length + count], suffix);
    return text;
}

static void refused(const char *fname)
{
    struct parts parts;

    split(&parts, fname);
    assert(parts.devname[0] == '\0');
    assert(parts.path[0] == '\0');
}

int main(void)
{
    struct parts parts;

    split(&parts, "SONIC2.ADX");
    assert(strcmp(parts.devname, "") == 0);
    assert(strcmp(parts.path, "SONIC2.ADX") == 0);
    split(&parts, "gd:EVENT_ADX.AFS");
    assert(strcmp(parts.devname, "GD") == 0);
    assert(strcmp(parts.path, "EVENT_ADX.AFS") == 0);
    split(&parts, name("GD:", 'P', CVFS_FNAME_MAX - 1, ""));
    assert(strcmp(parts.devname, "GD") == 0);
    assert(strlen(parts.path) == CVFS_FNAME_MAX - 1);
    refused(name("", 'N', CVFS_FNAME_MAX, ""));
    refused(name("GD:", 'P', CVFS_FNAME_MAX + 8, ""));
    refused(name("", 'D', CVFS_FNAME_MAX, ":FILE.ADX"));
    return 0;
}
