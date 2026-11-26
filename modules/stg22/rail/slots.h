/* the indexes of the textures in the lists of the rail exit, the intake and
 * the shards. their models' texture chunks name them */
#ifndef STG22_RAIL_SLOTS_H
#define STG22_RAIL_SLOTS_H

enum rail_exit_texture {
    RAIL_EXIT_128_CG005,
    RAIL_EXIT_128_CG010,
    RAIL_EXIT_128_CG016,
    RAIL_EXIT_128_CG017,
    RAIL_EXIT_128_CG021,
    RAIL_EXIT_128_CG022,
    RAIL_EXIT_256_CG004,
    RAIL_EXIT_64_CG008,
    RAIL_EXIT_64_CG013,
    RAIL_EXIT_64_CG014,
    RAIL_EXIT_64_CG015,
    RAIL_EXIT_TEXTURES
};

enum intake_texture {
    INTAKE_128_CG015,
    INTAKE_128_CG021,
    INTAKE_128_CG023,
    INTAKE_64_CG001,
    INTAKE_64_CG006,
    INTAKE_64_CG010,
    INTAKE_64_CG013,
    INTAKE_64_CG014,
    INTAKE_64_CG015,
    INTAKE_64_CG019,
    INTAKE_64_CG021,
    INTAKE_64_EE002,
    INTAKE_TEXTURES
};

enum intake_shard_a_texture {
    INTAKE_SHARD_A_64_EE002,
    INTAKE_SHARD_A_TEXTURES
};

enum intake_shard_b_texture {
    INTAKE_SHARD_B_64_EE002,
    INTAKE_SHARD_B_TEXTURES
};
#endif
