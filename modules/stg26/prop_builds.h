/* each prop's build, which compiles its models at buffer and returns the
 * first byte past them. egg_quarters.c runs them in turn on one buffer */
#ifndef STG26_PROP_BUILDS_H
#define STG26_PROP_BUILDS_H
#include <sg_xpt.h>

Uint8 *quarters_fire_mouth_build(Uint8 *buffer);
Uint8 *quarters_fire_pot_build(Uint8 *buffer);
Uint8 *quarters_machinery_build(Uint8 *buffer);
Uint8 *quarters_coffin_build(Uint8 *buffer);
Uint8 *quarters_curtain_build(Uint8 *buffer);
Uint8 *quarters_propeller_build(Uint8 *buffer);
Uint8 *quarters_propeller2_build(Uint8 *buffer);
Uint8 *quarters_wall_torch_build(Uint8 *buffer);
Uint8 *quarters_torch_build(Uint8 *buffer);
Uint8 *quarters_treasure_build(Uint8 *buffer);
Uint8 *quarters_snake_pot_build(Uint8 *buffer);

#endif
