/* the sound of a current, for side_current.h and rising_current.h. its volume
 * falls evenly with the current camera's distance from the current's surface.
 * it is near_volume at the surface and far_volume at sound_range. in Dry
 * Lagoon, while the camera is above the water, above_water_gain scales the
 * volume's rise over far_volume */
#ifndef SHARED_CURRENT_SOUND_H
#define SHARED_CURRENT_SOUND_H

#ifdef LAGOON_CURRENT
#define CURRENT_IN_RANGE(distance) (!((distance) > sound_range))
#define CURRENT_GAIN(gain, distance)                                           \
    do {                                                                       \
        if (lagoon_water_level() <                                             \
            camera_views[camera_current_index]->pos.y) {                       \
            (gain) *= (sound_range - (distance)) * above_water_gain;           \
        } else {                                                               \
            (gain) *= sound_range - (distance);                                \
        }                                                                      \
    } while (0)
#else
#define CURRENT_IN_RANGE(distance) ((distance) <= sound_range)
#define CURRENT_GAIN(gain, distance) ((gain) *= sound_range - (distance))
#endif

/* @hack spelling: the sound played by a macro, where an inline function would
 * do */
#define PLAY_CURRENT_SOUND(task, distance)                                     \
    do {                                                                       \
        Float gain;                                                            \
        int volume;                                                            \
        if (CURRENT_IN_RANGE(distance)) {                                      \
            gain = (near_volume - far_volume) / sound_range;                   \
            CURRENT_GAIN(gain, distance);                                      \
            volume = far_volume + gain;                                        \
            sound_play_for(STREAM_SOUND, (task), 1, 0, 10);                    \
            sound_set_volume((task), SOUND_ANY_ID, volume);                    \
        }                                                                      \
    } while (0)
#endif
