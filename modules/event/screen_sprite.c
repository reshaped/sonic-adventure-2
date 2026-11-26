#include <ninja.h>
#include <sg_xpt.h>

#include "event/scene.h"
#include "event/screen_sprite.h"
#include "system/task.h"
#include "system/util.h"

/* the task level of a screen sprite */
#define EFFECT_LEVEL 2

static void screen_sprite_step(struct task *task);
static void screen_sprite_draw(struct task *task);
static void screen_sprite_destroy(struct task *task);

void event_screen_sprite_start(struct event_screen_sprite *sprite)
{
    struct task *task;
    struct event_screen_sprite *work;

    task =
        task_create(TASK_WORK, EFFECT_LEVEL, screen_sprite_step, "spriteExec");
    task->draw = screen_sprite_draw;
    task->destructor = screen_sprite_destroy;
    work = (struct event_screen_sprite *)task->work;
    *work = *sprite;
}

static void screen_sprite_step(struct task *task)
{
    struct event_screen_sprite *work = (struct event_screen_sprite *)task->work;

    if (event_frame - (Float)work->first_frame < EVENT_SPRITE_FADE_FRAMES &&
        work->fade_in) {
        work->color.a = (event_frame - (Float)work->first_frame) *
                        (1.0F / EVENT_SPRITE_FADE_FRAMES);
    } else if (event_frame - (Float)work->first_frame >=
                   (Float)(work->lifetime - EVENT_SPRITE_FADE_FRAMES) &&
               work->fade_out) {
        work->color.a =
            ((Float)(work->lifetime + work->first_frame) - event_frame) *
            (1.0F / EVENT_SPRITE_FADE_FRAMES);
    } else {
        work->color.a = 1.0F;
    }
    work->color.r = 1.0F;
    work->color.g = 1.0F;
    work->color.b = 1.0F;
    if (event_frame - (Float)work->first_frame >= (Float)work->lifetime) {
        task_destroy(task);
    }
}

static void screen_sprite_draw(struct task *task)
{
    struct event_screen_sprite *work = (struct event_screen_sprite *)task->work;
    NJS_TEXANIM anim;
    NJS_SPRITE sprite;

    anim.sx = event_scene_file->texture_sizes[work->texture].width;
    anim.sy = event_scene_file->texture_sizes[work->texture].height;
    anim.cx = 0;
    anim.cy = 0;
    anim.u1 = 0;
    anim.v1 = 0;
    anim.u2 = EVENT_UV_LAST;
    anim.v2 = EVENT_UV_LAST;
    anim.texid = work->texture;
    sprite.tlist = event_scene_file->texlist;
    sprite.tanim = &anim;
    sprite.ang = 0;
    sprite.sx = work->scale.x;
    sprite.sy = work->scale.y;
    sprite.p.x = work->offset.x + EVENT_SCREEN_CENTER_X;
    sprite.p.y = work->offset.y + EVENT_SCREEN_CENTER_Y;
    draw_attr_save();
    njSetConstantMaterial(&work->color);
    njDrawSprite2D(&sprite, 0, EVENT_SPRITE_PRIORITY,
                   NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR);
    draw_attr_restore();
}

static void screen_sprite_destroy(struct task *task)
{
}
