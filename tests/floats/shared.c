/* Each output receives a different addend, so SHC can only share the product.
 * The caller supplies the inputs and can change saved objects between writes.
 */
extern float shared_factor;
extern volatile float shared_volatile;
extern void shared_change(void);
extern void shared_publish(float *address);

struct shared_member {
    float value;
};

void shared_global_call(float *out, float b, float c, float d)
{
    out[0] = shared_factor * b + c;
    shared_change();
    out[1] = shared_factor * b + d;
}

void shared_global_store(float *out, float b, float c, float d)
{
    out[0] = shared_factor * b + c;
    out[1] = shared_factor * b + d;
}

void shared_static_escape(float *out, float a, float b, float c, float d)
{
    static float value;

    value = a;
    shared_publish(&value);
    out[0] = value * b + c;
    shared_change();
    out[1] = value * b + d;
}

void shared_local_escape(float *out, float a, float b, float c, float d)
{
    shared_publish(&a);
    out[0] = a * b + c;
    shared_change();
    out[1] = a * b + d;
}

void shared_volatile_read(float *out, float b, float c, float d)
{
    out[0] = shared_volatile * b + c;
    out[1] = shared_volatile * b + d;
}

void shared_pointer(float *out, float *a, float b, float c, float d)
{
    out[0] = *a * b + c;
    shared_change();
    out[1] = *a * b + d;
}

void shared_member_call(float *out, struct shared_member *a, float b, float c,
                        float d)
{
    out[0] = a->value * b + c;
    shared_change();
    out[1] = a->value * b + d;
}

void shared_parameters(float *out, float a, float b, float c, float d)
{
    out[0] = a * b + c;
    out[1] = a * b + d;
}

void shared_parameters_call(float *out, float a, float b, float c, float d)
{
    out[0] = a * b + c;
    shared_change();
    out[1] = a * b + d;
}

void shared_global_temporaries(float *out, float b, float c, float d)
{
    float first;
    float second;

    first = shared_factor * b + c;
    second = shared_factor * b + d;
    out[0] = first;
    out[1] = second;
}

void shared_volatile_parameter(float *out, volatile float a, float b, float c,
                               float d)
{
    out[0] = a * b + c;
    out[1] = a * b + d;
}

void shared_indirect_call(float *out, float b, float c, float d,
                          void (*change)(void))
{
    out[0] = shared_factor * b + c;
    (*change)();
    out[1] = shared_factor * b + d;
}
