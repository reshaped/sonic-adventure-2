/* As in shared.c, each output receives a different addend, so SHC can only
 * share the product. Each output waits in a local, so that no store through
 * out falls between the two products.
 */
struct shared_reads {
    float value;
};

void shared_member_address(float *out, struct shared_reads t, float k, float c,
                           float d)
{
    struct shared_reads s;
    float *p = &s.value;
    float x;
    float y;

    s = t;
    x = *p * k + c;
    s = t;
    y = *p * k + d;
    out[0] = x;
    out[1] = y;
}

void shared_separate_local(float *out, struct shared_reads t, float k, float c,
                           float d)
{
    struct shared_reads s;
    float *p = &s.value;
    float x;
    float y;
    float u;

    s = t;
    x = *p * k + c;
    u = d;
    y = *p * k + u;
    out[0] = x;
    out[1] = y;
}

void shared_changed_addend(float *out, float a, float b, float c, float d)
{
    float x;
    float y;

    x = c + a * b;
    c = d;
    y = c + a * b;
    out[0] = x;
    out[1] = y;
}

void shared_compound(float *out, float a, float b, float c, float d)
{
    float x = d;
    float y;

    x += a * b;
    y = a * b + c;
    out[0] = x;
    out[1] = y;
}
