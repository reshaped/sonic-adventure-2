/* the models and solids of the moving platforms: STEP and STEPB, their
 * Cannon's Core forms, and LASTSTEP */
#ifndef SHARED_STEP_MODELS_H
#define SHARED_STEP_MODELS_H
#include <ninja.h>

/* Eternal Engine and Lost Colony each define step_model. Lost Colony's STEP
 * also draws step_light_model, with fog off */
extern NJS_CNK_MODEL step_model[1];
extern NJS_CNK_MODEL step_light_model;
extern NJS_OBJECT step_collision;
extern NJS_CNK_MODEL step_b_model[1];
extern NJS_OBJECT step_b_collision;

extern NJS_CNK_MODEL core_step_model;
extern NJS_CNK_MODEL core_step_b_model;
extern NJS_OBJECT core_step_collision;
extern NJS_OBJECT core_step_b_collision;

extern NJS_CNK_MODEL last_step_model;
extern NJS_OBJECT last_step_collision;
extern NJS_CNK_OBJECT last_step_left_panel;
extern NJS_CNK_OBJECT last_step_right_panel;
/* @hack type: the node drawn without fog declared as an array of one, where
 * the node alone would do */
extern NJS_CNK_OBJECT last_step_unfogged[1];

#endif
