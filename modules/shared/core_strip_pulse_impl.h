/* the draw of one group of lamp strips, which core_strip_lights.h includes
 * once for each group. the includer defines draw_lamp_at first. it names the
 * draw STRIP_PULSE_DRAW, and gives the group's CORE_STRIPS strips as
 * STRIP_PULSE_LAMPS */
static void STRIP_PULSE_DRAW(struct task *task)
{
    int frame = game_state.steps % ((core_strip_delay * (LAMPS_PER_STRIP - 1)) +
                                    core_strip_rise + core_strip_fall);
    int lamp;
    int strip;
    int age;
    int alpha;
    /* NOLINTNEXTLINE(clang-diagnostic-unused-but-set-variable) */
    int unused_frame;
    Uint32 opacity;
    Uint32 forward;
    Uint32 reverse;

    for (lamp = 0; lamp < LAMPS_PER_STRIP; ++lamp) {
        if ((core_strip_delay * lamp) <= frame &&
            frame <
                (core_strip_delay * lamp) + core_strip_rise + core_strip_fall) {
            if ((age = frame - (core_strip_delay * lamp)) < core_strip_rise) {
                alpha = ((Float)age / (core_strip_rise - 1)) * 255.0F;
            } else {
                alpha = ((Float)(core_strip_rise + core_strip_fall - age - 1) /
                         (core_strip_fall - 1)) *
                        255.0F;
            }
            /* @hack redundant: the selected frame is never consumed. */
            unused_frame = age ? game_state.steps : 0;
            opacity = alpha;
            opacity <<= 24;
            forward = ((Uint32)(int)(alpha * core_strip_red) << 16) |
                      ((Uint32)(int)(alpha * core_strip_green) << 8) |
                      (Uint32)(int)(alpha * core_strip_blue) | opacity;
            reverse = ((Uint32)(int)(alpha * core_strip_reverse_red) << 16) |
                      ((Uint32)(int)(alpha * core_strip_reverse_green) << 8) |
                      (Uint32)(int)(alpha * core_strip_reverse_blue);
            reverse |= opacity;
            for (strip = 0; strip < CORE_STRIPS; ++strip) {
                draw_lamp_at(STRIP_PULSE_LAMPS[strip][lamp], forward);
                draw_lamp_at(
                    STRIP_PULSE_LAMPS[strip][LAMPS_PER_STRIP - 1 - lamp],
                    reverse);
            }
        }
    }
}
