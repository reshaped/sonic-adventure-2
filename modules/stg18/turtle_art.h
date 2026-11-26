/* Both turtles blink by replacing their shared eye node's mesh. */
#ifndef STG18_TURTLE_ART_H
#define STG18_TURTLE_ART_H
#include <ninja.h>

extern const char turtle_texture_filenames[31];
extern NJS_TEXLIST turtle_texlist;
extern NJS_CNK_OBJECT turtle_object;
extern NJS_CNK_OBJECT turtle_eyes[1];
extern NJS_OBJECT turtle_ground[1];
extern NJS_CNK_MODEL turtle_eye_model_0;
extern NJS_CNK_MODEL turtle_eye_model_1;
extern NJS_CNK_MODEL turtle_eye_model_2;
extern NJS_CNK_MODEL turtle_eye_model_3;
extern NJS_CNK_MODEL turtle_eye_model_4;
extern NJS_MOTION turtle_swim_motion;
extern NJS_MOTION turtle_stop_motion;
extern NJS_MOTION turtle_wait_motion;
extern NJS_MOTION turtle_wait_motion_2;
extern NJS_MOTION turtle_arrive_motion;
extern NJS_POINT3 turtle_path_points[141];

#endif
