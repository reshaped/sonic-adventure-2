/* Kamui's profile settings, which no file holds */
#include <kamui.h>
#include <kamui_internal.h>
#include <string.h>

KMDWORD kmiGetProfileInt(const char *section, const char *key, KMDWORD fallback)
{
    KMDWORD value;

#ifdef NONMATCHING
    value = fallback;
#endif
    if (strcmp(section, "HARDWARE") == 0) {
        /* NOLINTNEXTLINE(clang-diagnostic-sometimes-uninitialized) */
        if (strcmp(key, "EnableTexture16MB") == 0) {
            value = 0;
        }
    } else {
        value = fallback;
    }
    /* @bug uninitialized: in section "HARDWARE", a key other than
     * "EnableTexture16MB" returns what strcmp left in R4 (the BF at 0x8c16a3fc
     * and the MOV R4,R0 at 0x8c16a40a) */
    return value;
}
