/* Original-compiler probes for the play float pass. */
typedef float ProbeFloat;

struct probe_pair {
    float position;
    float speed;
};

extern float probe_global;
extern float *probe_pointer;
extern float probe_change(void);

float add_product(float a, float b, float c)
{
    return a * b + c;
}

float subtract_product(float a, float b, float c)
{
    return c - a * b;
}

float negative_product(float a, float b, float c)
{
    return -(a * b) + c;
}

float subtract_products(float a, float b, float c)
{
    return a * b - c * c;
}

float product_then_constant(float a, float b)
{
    return a * b - 0.5F;
}

float sum_products(float a, float b, float c, float d)
{
    return a * b + c * d;
}

float fold_constants(float a, float b)
{
    return (a * b + 0.5F) + 1.0F;
}

float order_names(float a, float b, float c, float d)
{
    return -a + b + c * d;
}

float order_element(float *values, float a, float b, float c)
{
    return -a + values[0] + b * c;
}

float order_dereference(float *value, float a, float b, float c)
{
    return a - *value + b * c;
}

float ordered_parentheses(float a, float b, float c, float d)
{
    return a - (b - c * d);
}

float order_global(float a, float b, float c)
{
    return -a + probe_global + b * c;
}

float order_member(struct probe_pair *pair, float a, float b)
{
    return -(pair->position * a) + pair->speed * b;
}

float order_call(float a, float b, float c)
{
    return -a + probe_change() + b * c;
}

float fused_typedef(ProbeFloat a, ProbeFloat b)
{
    return 1.0F + a * b;
}

float fused_cast(float a, float b, float c)
{
    return c + (ProbeFloat)(a * b);
}

void shared_local(float *out, float a, float b, float c, float d)
{
    out[0] = c + a * b;
    out[1] = d + a * b;
}

void shared_sum(float *out, float a, float b, float c)
{
    out[0] = c + a * b;
    out[1] = c + a * b;
}

void assign_pointer(float b)
{
    *probe_pointer += probe_change() * b;
}

void assign_name(float b)
{
    probe_global += probe_change() * b;
}

void assign_index(float *values, int *index, float a, float b)
{
    values[(*index)++] += a * b;
}

void subtract_assignment(float *total, float a, float b)
{
    *total -= a * b;
}

void assign_integer(int *total, float a, float b)
{
    *total += a * b;
}

float constant_product(float a)
{
    return a + 0.1F * 3.0F;
}

float divide_parameter(float x, float divisor)
{
    return x / divisor;
}

float reciprocal(float x)
{
    return x / 3.0F;
}

float add_quotient(float x, float y)
{
    return y + x / 3.0F;
}

void assign_quotient(float *x)
{
    *x /= 3.0F;
}

/* SHC may combine these operations across a boundary the source pass preserves.
 */
float across_statement(float a, float b, float c)
{
    float product = a * b;
    return product + c;
}

static float multiply(float a, float b)
{
    return a * b;
}

float across_inline(float a, float b, float c)
{
    return multiply(a, b) + c;
}

float across_branch(int select, float a, float b, float c)
{
    if (select) {
        return c + a * b;
    }
    return c - a * b;
}
