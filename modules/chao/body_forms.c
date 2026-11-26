/* six models for each Chao type from CHAO_CHILD on. a type's six are its
 * base and grown models, then its swim, fly, run and power models. a Chaos
 * type repeats its one model six times. the models are Chao_model.PRS's,
 * which the garden loads at their addresses */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/body.h"

#include "chao/body_model_art.h"

NJS_CNK_OBJECT *chao_form_models[126] = {
#include "chao/body_forms/chao_form_models.inc"
};

/* the body nodes whose vertices weigh their springs by distance from their
 * parent mesh's spring center. other nodes' vertices take their own mesh's.
 * -1 ends the list */
Sint32 chao_hanging_nodes[15] = {
#include "chao/body_forms/chao_hanging_nodes.inc"
};
