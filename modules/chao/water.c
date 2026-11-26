/* a garden's animated water. each surface steps its texture by speed,
 * wrapping through the texture_count textures from first_texture. a
 * scrolling surface draws a copy of its object, whose UVs garden_uv_step
 * offsets from the object's own */
#include <ninja.h>

#include "chao/garden_scene.h"
#include "chao/model.h"
#include "system/task.h"

static void water_step(struct task *task)
{
    struct garden_water *water = task->motion.p;

    if (water->object != NULL) {
        water->frame += water->speed;
        if (water->frame >= water->first_texture + water->texture_count) {
            water->frame -= water->texture_count;
        } else if (water->frame < water->first_texture) {
            water->frame += water->texture_count;
        }
        if (task->extra.p == NULL) {
            GARDEN_ANIMATE_TEXTURE(water->object->model, (int)water->frame);
        } else {
            GARDEN_ANIMATE_TEXTURE(
                ((struct garden_uv_scroll *)task->extra.p)->copy->model,
                (int)water->frame);
        }
    }
    if (task->extra.p != NULL) {
        garden_uv_step(task->extra.p);
    }
}

static void water_draw(struct task *task)
{
    struct garden_water *water = task->motion.p;
    njSetTexture(water->texlist);
    if (task->extra.p == NULL) {
        njCnkSimpleDrawObject(water->object);
    } else {
        njCnkSimpleDrawObject(((struct garden_uv_scroll *)task->extra.p)->copy);
    }
}

static void water_destroy(struct task *task)
{
    /* motion points into the caller's surfaces, which task_destroy would
     * free */
    /* @hack redundant: the test of motion before it is cleared */
    if (task->motion.p != NULL) {
        task->motion.p = NULL;
    }
    if (task->extra.p != NULL) {
        garden_uv_destroy(task->extra.p);
        task->extra.p = NULL;
    }
}

void garden_water_create(struct garden_water *surfaces)
{
    struct task *task;
    int index = 0;
    for (; surfaces->object != NULL; surfaces++) {
        task = task_create(TASK_WORK, 3, water_step, "WaterManagerExecutor");
        task->draw = water_draw;
        task->destructor = water_destroy;
        task->motion.p = surfaces;
        task->work->tick = index++;
        if (surfaces->speed >= 0.0F) {
            surfaces->frame = surfaces->first_texture + surfaces->start_frame;
        } else {
            surfaces->frame =
                (surfaces->first_texture + surfaces->texture_count) -
                surfaces->start_frame;
        }
        if (surfaces->scroll) {
            task->extra.p =
                garden_uv_create(surfaces->object, surfaces->du, surfaces->dv);
        }
    }
}
