/* the functions of BLOCK and BLOCK2 that follow each one's draw. core_block.c
 * and core_lift.c include them, and core_lift.c defines VERTICAL_BLOCK */
#ifndef SHARED_BLOCK_CYCLE_IMPL_H
#define SHARED_BLOCK_CYCLE_IMPL_H

/* a flash begins start steps into each period of core_block_flash_period.
 * its alpha rises toward FLASH_RAMP_TOP over FLASH_FADE of the period, and
 * holds at 1 over FLASH_HOLD of it. it then falls from FLASH_RAMP_TOP over
 * FLASH_FADE. each of the three parts lasts one step at least */
#define FLASH_FADE 0.1F
#define FLASH_HOLD 0.3F
#define FLASH_RAMP_TOP 0.9F
/* the word of the glow's polygon list that holds its texture id and filter
 * mode */
#define GLOW_TEXTURE_WORD 7

static void core_block_contact(struct task *task)
{
    collision_register(task);
}

static void core_block_contact_destroy(struct task *task)
{
}

static int core_block_flash_period(struct task *task)
{
    int period = (Sint32)MOVING_STEPS(task) / njAbs(task->work->scl.z) *
                 njAbs(BLOCK_SCALE(task)) * (2 * BLOCK_HALF_SIZE);
    if (!period) {
        period = 1;
    }
    return period;
}

static void core_block_flash(struct task *task, NJS_CNK_MODEL *model, int start)
{
    int period;
    int frame;
    int fade_start;
    int fade;
    int hold;
    int begin;
    /* @hack spelling: rise, a copy of fade, where fade would do */
    int rise;
    int end;
    Float alpha;
    period = core_block_flash_period(task);
    frame = game_state.steps % period;
    fade = period * FLASH_FADE;
    hold = period * FLASH_HOLD;
    if (!fade) {
        fade = 1;
    }
    rise = fade;
    if (!hold) {
        hold = 1;
    }
    begin = start;
    end = begin + rise;
    if (begin <= frame && frame < end) {
        alpha = (Float)(frame - begin) / (Float)rise;
        alpha *= FLASH_RAMP_TOP;
        model_draw_material_additive(model, alpha, alpha, alpha, alpha);
    } else {
        begin = start + fade;
        end = begin + hold;
        if (begin <= frame && frame < end) {
            alpha = 1.0F;
            model_draw_material_additive(model, alpha, alpha, alpha, alpha);
        } else {
            fade_start = start + fade + hold;
            end = fade_start + fade;
            if (fade_start <= frame && frame < end) {
                alpha = (Float)(end - frame) / (Float)fade;
                alpha *= FLASH_RAMP_TOP;
                model_draw_material_additive(model, alpha, alpha, alpha, alpha);
            }
        }
    }
}

static Float core_block_offset(struct task *task)
{
    Float width;
    Float half;
    Float offset;
    int phase;
    width = task->work->scl.z + (BLOCK_SCALE(task) * BLOCK_TRAVEL_MARGIN);
    half = width * 0.5F;
    offset = half;
    phase = core_block_phase(task);
    if (phase < MOVING_STEPS(task)) {
#ifdef VERTICAL_BLOCK
        offset = (width / (Float)MOVING_STEPS(task) * (Float)phase) - half;
#else
        offset = -(width / (Float)MOVING_STEPS(task) * (Float)phase) + half;
#endif
    }
    if (REVERSED(task)) {
        return -offset;
    }
    return offset;
}

#ifdef VERTICAL_BLOCK
static void core_block_portal_glow(NJS_CNK_MODEL *model)
{
    int flags = model->plist[GLOW_TEXTURE_WORD] & NJD_FFM_MASK;
    flags |= game_state.steps % PORTAL_FRAMES;
    model->plist[GLOW_TEXTURE_WORD] = flags;
    njSetTexture(&core_block_glow_texlist);
    njCnkEasyDrawModel(model);
}
#else
static void core_block_portal_glow(void)
{
    Sint16 *texture;
    int flags;
    texture = &core_block_glow->plist[GLOW_TEXTURE_WORD];
    flags = *texture & NJD_FFM_MASK;
    /* @hack spelling: flags | the frame assigned to flags, where flags |= the
     * frame would do */
    flags = flags | (game_state.steps % PORTAL_FRAMES);
    *texture = flags;
    njSetTexture(&core_block_glow_texlist);
    njCnkEasyDrawModel(core_block_glow);
}
#endif

static int core_block_phase(struct task *task)
{
    int frame;
    frame =
        (Float)game_state.steps +
        ((Float)(MOVING_STEPS(task) + WAIT_STEPS(task)) *
         (Float)((task->work->ang.y & 0xff) << 8) * (1.0F / ANGLE_PER_TURN));
    return frame % (MOVING_STEPS(task) + WAIT_STEPS(task));
}

#endif
