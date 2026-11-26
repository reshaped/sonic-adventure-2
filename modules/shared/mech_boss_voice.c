/* the mech bosses' voice script: the lines a boss's pilot speaks as the
 * boss's power falls */
#include <cri_adxt.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/mech_boss_voice.h"
#include "sound/adx.h"
#include "system/game_state.h"
#include "system/task.h"

#define TASK_LEVEL 3

#define LINES 3
#define AT_ONCE 10
/* the first line starts on step FIRST_DELAY + 1. a silence lasts SILENCE + 1
 * steps. the task ends LAST_DELAY + 1 steps after the last line's silence */
#define FIRST_DELAY 5
#define SILENCE 15
#define LAST_DELAY 90
/* voice_play's priority for a line, and for a line's closing voice */
#define LINE_PRIORITY 2
#define CLOSING_PRIORITY 3

/* a script's modes, in its work's mode, in the order it takes them */
enum voice_mode { MODE_DELAY, MODE_CUE, MODE_SPEAK, MODE_SILENCE, MODE_END };

struct voice_line {
    int voice; /* voice_play's id */
    /* the id of a voice played after the line's silence, where above 0 */
    int closing_voice;
    /* the work's timer as the line starts, which no mode reads */
    Sint16 timer;
    Sint16 unk_0a;
    /* the line starts on a step where the boss's power, cut to a Sint8,
     * equals it. AT_ONCE starts it as soon as it is the line in turn */
    Sint8 power;
};

/* a script's work, laid over the struct task_work that task_create
 * allocates */
struct voice_work {
    Sint8 mode;
    Sint8 line; /* the line in turn */
    Uint8 unk_02;
    Uint8 script; /* the boss, an enum mech_boss */
    Uint8 unk_04[2];
    Uint16 timer;
    Float *power;
    Angle unk_0c;
    int stream; /* voice_play's stream for the line, or -1 */
};

#define WORK(task) ((struct voice_work *)(task)->work)

/* the scripts, indexed by enum mech_boss */
static struct voice_line voice_scripts[3][LINES] = {
#include "shared/mech_boss_voice/voice_scripts.inc"
};
#ifdef NONMATCHING
/* the line this side reads after a script's last, for the tag in
 * boss_voice_speak. a boss's power starts at 4 or 5 and only falls, so it
 * never reaches AT_ONCE */
static const struct voice_line voice_after_last = {0, 0, 0, 0, AT_ONCE};

/* the line after the one in turn, or voice_after_last after the script's
 * last */
static const struct voice_line *voice_next_line(const struct voice_work *work)
{
    if (work->line + 1 < LINES) {
        return &voice_scripts[work->script][work->line + 1];
    }
    return &voice_after_last;
}
#endif

static void boss_voice_delay(struct task *task)
{
    struct voice_work *work = WORK(task);

    if (--work->timer == 0) {
        work->mode = MODE_CUE;
        /* @hack redundant: the timer cleared, though it is 0 */
        work->timer = 0;
    }
}

static void boss_voice_cue(struct task *task)
{
    struct voice_work *work = WORK(task);
    /* @hack spelling: start, a flag the two tests set, where one test of
     * either would do */
    Bool start = FALSE;
    struct voice_line *line = &voice_scripts[work->script][work->line];

    if (line->power == AT_ONCE) {
        start = TRUE;
    } else if (line->power == (Sint8)*work->power) {
        start = TRUE;
    }
    if (start) {
        work->stream = voice_play(line->voice, LINE_PRIORITY);
        work->timer = line->timer;
        work->mode = MODE_SPEAK;
    }
}

static void boss_voice_speak(struct task *task)
{
    struct voice_work *work = WORK(task);
    /* @bug memory: reads the line after the script's last while the last
     * plays, here and in boss_voice_silence. for the last script, that line
     * lies past the table (MOV.B @(12,R4),R0 at 0xa2 and 0x10e of the
     * object's code) */
#ifdef NONMATCHING
    /* this side keeps the tag's result by defined means: after the script's
     * last line, next is voice_after_last, so no power cuts the last line.
     * the image reads the next script's first line there, whose power is
     * AT_ONCE. for the Flying Dog's script, the last, it reads 0x54 from
     * dust.c's data, which follows the table in the Flying Dog's image. a
     * boss's power reaches neither */
    const struct voice_line *next = voice_next_line(work);
#else
    struct voice_line *next = &voice_scripts[work->script][work->line + 1];
#endif
    int status;

    if (next->power == (Sint8)*work->power) {
        work->line++;
        work->timer = 0;
        work->mode = MODE_CUE;
        return;
    }
    status = voice_status(work->stream);
    if (status == ADXT_STAT_PLAYEND || status == ADXT_STAT_STOP) {
        work->timer = SILENCE;
        work->mode = MODE_SILENCE;
    }
}

static void boss_voice_silence(struct task *task)
{
    struct voice_work *work = WORK(task);
#ifdef NONMATCHING
    /* this side keeps the result of the tag in boss_voice_speak, as there */
    const struct voice_line *next = voice_next_line(work);
#else
    struct voice_line *next = &voice_scripts[work->script][work->line + 1];
#endif
    struct voice_line *line;

    if (next->power == (Sint8)*work->power) {
        work->line++;
        work->timer = 0;
        work->mode = MODE_CUE;
    } else if (work->timer-- == 0) {
        line = &voice_scripts[work->script][work->line];
        if (line->closing_voice > 0) {
            voice_play(line->closing_voice, CLOSING_PRIORITY);
        }
        work->line++;
        work->timer = 0;
        if (work->line == LINES) {
            work->timer = LAST_DELAY;
            work->mode = MODE_END;
        } else {
            work->mode = MODE_CUE;
        }
    }
}

static void boss_voice_step(struct task *task)
{
    struct voice_work *work = WORK(task);

    if (game_state.time_stopped || game_state.mini_event_running) {
        return;
    }
    switch (work->mode) {
    case MODE_DELAY:
        boss_voice_delay(task);
        break;
    case MODE_CUE:
        boss_voice_cue(task);
        break;
    case MODE_SPEAK:
        boss_voice_speak(task);
        break;
    case MODE_SILENCE:
        boss_voice_silence(task);
        break;
    case MODE_END:
        if (work->timer-- == 0) {
            task_kill(task);
        }
        break;
    }
}

static void boss_voice_destroy(struct task *task)
{
}

struct task *mech_boss_voice_create(struct task *boss_task, Sint8 boss,
                                    Float *power)
{
    struct task *task;
    struct voice_work *work;

    task = task_create(TASK_WORK, TASK_LEVEL, boss_voice_step, "GroboAdxExec");
    if (task == NULL) {
        return NULL;
    }
    work = WORK(task);
    work->script = boss;
    work->line = 0;
    work->stream = -1;
    work->power = power;
    work->timer = FIRST_DELAY;
    task->destructor = boss_voice_destroy;
    return task;
}
