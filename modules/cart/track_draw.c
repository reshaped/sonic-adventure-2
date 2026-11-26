/* the course's draw: a task that draws each section near the camera, with
 * its signs */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "cart/course.h"
#include "cart/objects.h"
#include "cart/track.h"
#include "system/task.h"
#include "system/vector.h"

/* a section farther than DRAW_RANGE from the camera isn't drawn */
#define DRAW_RANGE 3000.0F

static Bool stopping = 1;
static void draw_step(struct task *task);
static void draw_sections(struct task *task);
static void draw_destroy(struct task *task);

void cart_start_course_draw(void)
{
    struct task *task;
    task = task_create(TASK_WORK, 2, draw_step, "courseDisplayExec");
    task->draw = draw_sections;
    task->destructor = draw_destroy;
    stopping = 0;
}
void cart_stop_course_draw(void)
{
    stopping = 1;
}
static void draw_step(struct task *task)
{
    if (stopping) {
        task_destroy(task);
    }
}
static void draw_sections(struct task *task)
{
    int i;
    int j;
    NJS_VECTOR offset;
    njSetTexture(&cart_object_textures);
    njFogDisable();
    for (i = 0; i < cart_section_count; i++) {
        if (cart_sections[i].model) {
            vector_diff(&cart_sections[i].pos,
                        &camera_views[camera_current_index]->pos, &offset);
            if (njScalor2(&offset) < DRAW_RANGE * DRAW_RANGE) {
                njPushMatrixEx();
                njTranslateEx(&cart_sections[i].pos);
                njRotateY(NULL, cart_sections[i].yaw);
                njDirectDrawObject(cart_sections[i].compiled);
                for (j = 0; j < CART_SIGNS; j++) {
                    if (cart_sections[i].signs[j]) {
                        cart_draw_sign(
                            &cart_pieces[cart_sections[i].piece].signs[j],
                            cart_sections[i].signs[j] - 1);
                    }
                }
                njPopMatrixEx();
            }
        }
    }
    njFogEnable();
}
static void draw_destroy(struct task *task)
{
}
