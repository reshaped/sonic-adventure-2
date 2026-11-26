/* Exercise read_parameter_fv's and read_string's returns for a null, on the
 * non-matching side. */
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "atcom_ex.h"
#include "messages.h"

void send_message(const char *text)
{
    (void)text;
    abort(); /* Each kind this test passes is one a reader takes. */
}

int main(void)
{
    char text[] = "12,AB";
    char *cursor = text;
    char *none = NULL;
    Uint32 value = 7;
    char buffer[3];
    assert(read_parameter_fv(&value, NULL, 'd') == AT_FIELD_MISSING);
    assert(read_parameter_fv(&value, &none, 'd') == AT_FIELD_MISSING);
    assert(read_parameter_fv(NULL, &cursor, 'd') == AT_FIELD_MISSING);
    assert(cursor == text && value == 7);
    assert(read_string(buffer, NULL, 2) == AT_FIELD_MISSING);
    assert(read_string(buffer, &none, 2) == AT_FIELD_MISSING);
    assert(read_string(NULL, &cursor, 2) == AT_FIELD_MISSING);
    assert(cursor == text);
    assert(read_parameter_fv(&value, &cursor, 'd') == AT_FIELD_VALUE);
    assert(value == 12);
    assert(read_string(buffer, &cursor, 2) == AT_FIELD_VALUE);
    assert(!strcmp(buffer, "AB"));
    return 0;
}
