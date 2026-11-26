/* _slow_strcmp1 and _quick_strcmp1 for GCC. make dc compiles this in place of
 * sdk/runtime/__strcmp.src. both read each byte as a signed char, as the
 * shipped pair does. C's strcmp reads an unsigned char. _quick_strcmp1 is the
 * entry for two 4-byte aligned strings. its result is _slow_strcmp1's. the
 * shipped _quick_strcmp1 reads them a long word at a time, through each
 * terminator's long word. the pair here reads no byte past either terminator */
#include <machine.h>

int _slow_strcmp1(const char *s1, const char *s2)
{
    const signed char *left = (const signed char *)s1;
    const signed char *right = (const signed char *)s2;

    while (*left && *left == *right) {
        left++;
        right++;
    }
    return *left - *right;
}

int _quick_strcmp1(const char *s1, const char *s2)
{
    return _slow_strcmp1(s1, s2);
}
