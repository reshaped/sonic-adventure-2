/* text about the camera zones, which no code reads */
#include "camera/zones.h"

static const char triple_format[] = "%3d %3d %3d";
static const char hit[] = "HIT";
static const char back_first[] = "First : URA";
static const char front_first[] = "First : OMOTE";

/* the ZONE_ shapes' names, by number */
static const char *shape_names[ZONE_BOX + 1] = {"USER", "SPHERE", "PLANE",
                                                "CUBE"};
