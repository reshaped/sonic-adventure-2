/* Calls made through the external interface can change either source of a
 * product. */
float shared_factor;
volatile float shared_volatile;
unsigned int shared_calls;
static float *published;

#ifdef __GNUC__
__attribute__((noipa))
#endif
void shared_publish(float *address)
{
    published = address;
}

#ifdef __GNUC__
__attribute__((noipa))
#endif
void shared_change(void)
{
    shared_factor = -shared_factor;
    if (published != 0) {
        *published = -*published;
        published = 0;
    }
    shared_calls++;
}
