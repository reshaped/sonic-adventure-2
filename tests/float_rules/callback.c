/* Keep an observable call in the terminal LTO fixture. Its caller must keep the
 * original destination while reading the value installed by the callback. */
float probe_global;
float probe_next;
float *probe_pointer;

#ifdef __GNUC__
__attribute__((noipa))
#endif
float probe_change(void)
{
    probe_pointer = &probe_next;
    probe_global = 4.0F;
    return 2.0F;
}
