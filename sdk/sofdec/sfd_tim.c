/* a handle's time. a time is a count of ticks and its unit, the ticks a
 * second. a time of unit SFD_NO_UNIT counts no ticks. SFD_COND_CLOCK picks
 * the clock a handle's time comes from */
#include <sfd.h>
#include <sofdec_uty.h>
#include <sg_xpt.h>

/* a handle's clock gives NOT_PLAYING while the handle neither plays nor has
 * finished. under SFD_CLOCK_NONE it gives NO_CLOCK, by which every picture is
 * due. TIME_UNSET is a timer's time not set yet */
#define NOT_PLAYING (-1)
#define NO_CLOCK (-2)
#define TIME_UNSET (-5)
/* the skipped picture's time before any picture is skipped */
#define NONE_SKIPPED (-1)
/* vblank_time until sftim_IsTakeOffExecTime starts it */
#define BEFORE_TAKEOFF (-1)
/* last_exec and judged_exec before any picture is judged */
#define NOT_JUDGED (-1.0F)
/* hold_count before the first verdict of sftim_IsGrExecTime. it is above
 * every hold limit, so that verdict goes by the picture's time */
#define HOLD_COUNT_UNSET 100
/* a frame's time in ticks, where the unit is a picture rate of sftim_prate */
#define FRAME_TICKS 1000
/* the unit of the zero time SFTIM_GetTimeOneFrmVideo gives a frame of a movie
 * whose picture rate is unknown */
#define UNKNOWN_RATE_UNIT 29970
/* the library clock's unit on NTSC: SFD_NORMAL_SPEED ticks a field, at 59.94
 * fields a second */
#define NTSC_CLOCK 59940
/* MPEG's picture rate code for 24 frames a second. sftim_IsGrExecTime takes
 * each code up to it for film, 0 for an unknown rate included */
#define FILM_RATE 2
/* the ticks a second in which SFTIM_IsExecTime compares times: tenths of a
 * millisecond, SFD_COND_DRAW_MARGIN's unit */
#define JUDGE_UNIT 10000.0F
/* a movie of SHORT_MOVIE_PICTURES or fewer ends its video once its last
 * picture's time is known. a longer one ends END_FIELDS after that time */
#define SHORT_MOVIE_PICTURES 3
#define END_FIELDS 2
#define INTERVAL_EASE 3
/* drop-frame time code drops DROPPED_FRAMES frame numbers at the start of
 * each minute but every KEPT_MINUTES-th */
#define DROPPED_FRAMES 2
#define KEPT_MINUTES 10
/* SFD_GetPlayFps's rate for a movie whose picture rate is unknown */
#define UNKNOWN_FPS (-1)
#define INVALID_FPS ((Sint32)0xff000118)
#define INVALID_TIME ((Sint32)0xff000121)
#define INVALID_FRAME ((Sint32)0xff000122)
#define INVALID_SOURCE ((Sint32)0xff000123)
#define INVALID_SKIP ((Sint32)0xff000124)
#define INVALID_SYNC ((Sint32)0xff000125)
#define INVALID_DRAW ((Sint32)0xff000126)
#define INVALID_SEEK ((Sint32)0xff000127)
#define INVALID_NOW ((Sint32)0xff000128)
#define INVALID_RATE ((Sint32)0xff000221)

typedef void (*timecode_converter)(Sint32 rate, const struct sfd_timecode *code,
                                   Sint32 *time, Sint32 *unit);
static Sint32 sftim_GetTimeNone(struct sfd_handle *handle, Sint32 *time,
                                Sint32 *unit);
static Sint32 sftim_GetTimeVsync(struct sfd_handle *handle, Sint32 *time,
                                 Sint32 *unit);
static Sint32 sftim_GetTimeUfrm(struct sfd_handle *handle, Sint32 *time,
                                Sint32 *unit);
static void sftim_HnVbIn(struct sfd_handle *handle);
static void sftim_CntupHnVbIn(struct sfd_handle *handle);
static Bool sftim_IsTimeIncre(struct sfd_handle *handle);
static Bool sftim_IsShortSupply(struct sfd_handle *handle);
static Bool sftim_IsVbinStIncre(struct sfd_handle *handle,
                                struct sfd_timer *timer);
static Sint32 sfdtim_GetTimeAfterSeek(struct sfd_handle *handle, Sint32 *time,
                                      Sint32 *unit);
static void sftim_AddHnVbIn(struct sfd_handle *handle, Sint32 time,
                            Sint32 unit);
static void sftim_IsTakeOffExecTime(struct sfd_handle *handle, Sint32 time,
                                    Sint32 unit, Bool *due);
static void sftim_IsGrExecTime(struct sfd_handle *handle, Bool *due,
                               Float presentation, Float now);
static void sftim_Tc2TimeN(Sint32 rate, const struct sfd_timecode *code,
                           Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time23N(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time29N(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time59N(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time23D(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time29D(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);
static void sftim_Tc2Time59D(Sint32 rate, const struct sfd_timecode *code,
                             Sint32 *time, Sint32 *unit);

/* the picture rates, by MPEG's picture rate code, in pictures per 1000
 * seconds */
static const Sint32 sftim_prate[] = {1,     23976, 24000, 25000, 29970,
                                     30000, 50000, 59940, 60000};
static const timecode_converter sftim_tc2time[][2] = {
    {NULL, NULL},
    {sftim_Tc2Time23N, sftim_Tc2Time23D},
    {sftim_Tc2TimeN, sftim_Tc2TimeN},
    {sftim_Tc2TimeN, sftim_Tc2TimeN},
    {sftim_Tc2Time29N, sftim_Tc2Time29D},
    {sftim_Tc2TimeN, sftim_Tc2TimeN},
    {sftim_Tc2TimeN, sftim_Tc2TimeN},
    {sftim_Tc2Time59N, sftim_Tc2Time59D},
    {sftim_Tc2TimeN, sftim_Tc2TimeN}};
void SFTIM_Init(struct sfd_clock *clock, Sint32 unit)
{
    clock->vsyncs = 0;
    clock->reserved = 0;
    clock->unit = unit;
}
void SFTIM_Finish(struct sfd_clock *clock)
{
}
void SFTIM_InitHn(struct sfd_handle *handle, struct sfd_timer *timer)
{
    Sint32 i;
    SFTIM_SetTimeFn(handle, sftim_GetTimeNone, SFD_CLOCK_NONE);
    SFTIM_SetTimeFn(handle, sftim_GetTimeVsync, SFD_CLOCK_VSYNC);
    SFTIM_SetTimeFn(handle, NULL, SFD_CLOCK_AUDIO);
    SFTIM_SetTimeFn(handle, sftim_GetTimeUfrm, SFD_CLOCK_USER_FRAME);
    SFTIM_SetTimeFn(handle, NULL, SFD_CLOCK_USER);
    timer->skip = NULL;
    SFTIM_InitTtu(&timer->current_picture, 0);
    SFTIM_InitTtu(&timer->first_picture, SFD_NO_TIME);
    SFTIM_InitTtu(&timer->latest_picture, 0);
    SFTIM_InitTtu(&timer->skipped_picture, NONE_SKIPPED);
    SFTIM_InitTtu(&timer->start, SFD_NO_TIME);
    timer->start_time = 0;
    timer->start_unit = 0;
    timer->concat_offset = 0;
    timer->supply_time = 0;
    timer->concat_count = 0;
    for (i = 0; i < SFD_FILE_HISTORY; i++) {
        timer->history[i] = 0;
    }
    timer->end_time = TIME_UNSET;
    timer->end_unit = SFD_NO_UNIT;
    timer->now_time = TIME_UNSET;
    timer->now_unit = SFD_NO_UNIT;
    timer->last_input_time = TIME_UNSET;
    timer->interval = SFD_NO_TIME;
    timer->max_delta = 0;
    timer->min_delta = SFD_NO_TIME;
    timer->user_frames = 0;
    timer->vsyncs = 0;
    timer->speed = SFD_NORMAL_SPEED;
    timer->display_count = 0;
    timer->display_time = 0;
    timer->display_unit = SFD_NO_UNIT;
    timer->hold_count = HOLD_COUNT_UNSET;
    timer->last_exec = NOT_JUDGED;
    timer->last_result = FALSE;
    timer->judged_exec = NOT_JUDGED;
    timer->vblank_time = BEFORE_TAKEOFF;
}
void SFTIM_InitTtu(struct sfd_time *stamp, Sint32 time)
{
    stamp->valid = FALSE;
    SFTIM_InitTcode(&stamp->code);
    stamp->time = time;
    stamp->unit = SFD_NO_UNIT;
}
void SFTIM_InitTcode(struct sfd_timecode *code)
{
    code->rate = 0;
    code->drop = 0;
    code->hour = 0;
    code->minute = 0;
    code->second = 0;
    code->frame = 0;
    code->frame_adjust = 0;
}
/* record an I picture's time, and fold its delta from the last into the
 * interval. both count the time's ticks. a smaller delta lowers the interval
 * by the difference >> INTERVAL_EASE, if that is a tick or more. any other
 * delta replaces it */
void SFTIM_UpdateItime(struct sfd_timer *timer, Sint32 time)
{
    Sint32 delta;
    Uint32 step;
    if (timer->last_input_time == TIME_UNSET) {
        timer->last_input_time = time;
        return;
    }
    delta = time - timer->last_input_time;
    if (delta == 0) {
        return;
    }
    timer->last_input_time = time;
    timer->max_delta = timer->max_delta > delta ? timer->max_delta : delta;
    timer->min_delta = timer->min_delta < delta ? timer->min_delta : delta;
    if (timer->interval != SFD_NO_TIME) {
        if (timer->interval > delta) {
            step = (Uint32)(timer->interval - delta) >> INTERVAL_EASE;
            if (step != 0) {
                timer->interval -= step;
                return;
            }
        }
    }
    timer->interval = delta;
}
/* the time the next I picture is due, judged from a picture at time. it is
 * the last I picture's time plus the interval, where time is before that.
 * where it isn't, the next is the last I picture's time plus the longest
 * delta. where time is past both, it is SFD_NO_TIME */
Sint32 SFTIM_GetNextItime(const struct sfd_timer *timer, Sint32 time)
{
    Sint32 next;
    Sint32 expected;
    Sint32 latest;
    expected = timer->last_input_time + timer->interval;
    latest = timer->last_input_time + timer->max_delta;
    if (time < expected) {
        next = expected;
    } else if (time < latest) {
        next = latest;
    } else {
        next = SFD_NO_TIME;
    }
    return next;
}
void SFTIM_VbIn(void)
{
    struct sfd_clock *clock;
    struct sfd_handle *handle;
    struct sfd_handle *handles;
    Sint32 count;
    Sint32 i;
    clock = &SFLIB_libwork.clock;
    clock->vsyncs++;
    count = SFLIB_libwork.handle_count;
    handles = SFLIB_libwork.handles;
    for (i = 0; i < count; i++) {
        handle = &handles[i];
        if (handle->status == SFD_FREE) {
            continue;
        }
        sftim_HnVbIn(handle);
    }
}
static void sftim_HnVbIn(struct sfd_handle *handle)
{
    struct sfd_timer *timer;
    sftim_CntupHnVbIn(handle);
    timer = &handle->timer;
    SFTIM_GetNowTime(handle, &timer->now_time, &timer->now_unit);
}
static void sftim_CntupHnVbIn(struct sfd_handle *handle)
{
    struct sfd_timer *timer;
    Sint32 mask;
    timer = &handle->timer;
    SFLIB_LockCs(&mask);
    if (sftim_IsTimeIncre(handle)) {
        timer->vsyncs += timer->speed;
    }
    if (sftim_IsVbinStIncre(handle, timer)) {
        timer->vblank_time += timer->speed;
    }
    SFLIB_UnlockCs(&mask);
}
static Bool sftim_IsTimeIncre(struct sfd_handle *handle)
{
    if (handle->status != SFD_PLAYING) {
        return FALSE;
    }
    if (handle->pause) {
        return FALSE;
    }
    if (sftim_IsShortSupply(handle)) {
        return FALSE;
    }
    return TRUE;
}
/* whether the handle's time stops at timer->supply_time, which it does until
 * a picture's time is read */
static Bool sftim_IsShortSupply(struct sfd_handle *handle)
{
    struct sfd_timer *timer;
    struct sfd_time *start;
    struct sfd_time *stamp;
    Sint32 time;
    Sint32 unit;
    Sint32 supply_unit;
    Sint32 supplied;
    timer = &handle->timer;
    start = &timer->start;
    stamp = &timer->latest_picture;
    /* @hack spelling: the supply time and its unit read before the call,
     * where the test could read them after it */
    supply_unit = start->unit;
    supplied = timer->supply_time;
    if (!stamp->valid) {
        sftim_GetTimeVsync(handle, &time, &unit);
        if (UTY_CmpTime(supplied, supply_unit, time, unit)) {
            return TRUE;
        }
    }
    return FALSE;
}
static Bool sftim_IsVbinStIncre(struct sfd_handle *handle,
                                struct sfd_timer *timer)
{
    if (timer->vblank_time == BEFORE_TAKEOFF) {
        return FALSE;
    }
    if (handle->request != SFD_PLAYING) {
        return FALSE;
    }
    return TRUE;
}
void SFTIM_SetStartTime(struct sfd_timer *timer, Sint32 time, Sint32 unit)
{
    timer->start_time = time;
    timer->start_unit = unit;
}
Sint32 SFD_GetTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_TIME);
    }
    return SFTIM_GetTimeSub(handle, time, unit);
}
/* the handle's time from the stream's start: SFTIM_GetTime's, plus the
 * start time SFTIM_SetStartTime set where their units agree. otherwise it
 * adds timer->start's time once that is known. timer->start holds the time of
 * the first picture decoded since the handle was created or reset. a time of
 * unit SFD_NO_UNIT is left as it is */
Sint32 SFTIM_GetTimeSub(struct sfd_handle *handle, Sint32 *time, Sint32 *unit)
{
    struct sfd_timer *timer;
    struct sfd_time *start;
    Sint32 result;
    timer = &handle->timer;
    start = &timer->start;
    result = sfdtim_GetTimeAfterSeek(handle, time, unit);
    if (*unit == SFD_NO_UNIT) {
        return result;
    }
    if (*unit == timer->start_unit) {
        *time += timer->start_time;
    } else if (start->valid) {
        *time += UTY_MulAbDivC(start->time, *unit, start->unit);
    }
    return result;
}
Sint32 SFD_GetTimeAfterSeek(struct sfd_handle *handle, Sint32 *time,
                            Sint32 *unit)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SEEK);
    }
    return sfdtim_GetTimeAfterSeek(handle, time, unit);
}
static Sint32 sfdtim_GetTimeAfterSeek(struct sfd_handle *handle, Sint32 *time,
                                      Sint32 *unit)
{
    return SFTIM_GetTime(handle, time, unit);
}
/* the time at the last vertical blank, or the clock's now before the
 * first */
Sint32 SFTIM_GetTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit)
{
    struct sfd_timer *timer;
    timer = &handle->timer;
    if (timer->now_time == TIME_UNSET) {
        return SFTIM_GetNowTime(handle, time, unit);
    }
    *time = timer->now_time;
    *unit = timer->now_unit;
    return 0;
}
Sint32 SFD_GetNowTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit)
{
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_NOW);
    }
    return SFTIM_GetNowTime(handle, time, unit);
}
Sint32 SFTIM_GetNowTime(struct sfd_handle *handle, Sint32 *time, Sint32 *unit)
{
    struct sfd_timer *timer;
    sfd_time_fn read_time;
    Sint32 source;
    Sint32 result;
    Sint32 mask;
    SFLIB_LockCs(&mask);
    timer = &handle->timer;
    source = SFSET_GetCond(handle, SFD_COND_CLOCK);
    read_time = timer->sources[source];
    if (read_time == NULL) {
        read_time = sftim_GetTimeNone;
    }
    result = read_time(handle, time, unit);
    SFLIB_UnlockCs(&mask);
    return result;
}
static Sint32 sftim_GetTimeNone(struct sfd_handle *handle, Sint32 *time,
                                Sint32 *unit)
{
    if (!SFTIM_ChkRegularTime(handle, time, unit)) {
        return 0;
    }
    *time = NO_CLOCK;
    *unit = SFD_NO_UNIT;
    return 0;
}
static Sint32 sftim_GetTimeVsync(struct sfd_handle *handle, Sint32 *time,
                                 Sint32 *unit)
{
    struct sfd_timer *timer;
    struct sfd_clock *clock;
    if (SFTIM_ChkRegularTime(handle, time, unit)) {
        timer = &handle->timer;
        *time = timer->vsyncs;
        clock = &SFLIB_libwork.clock;
        *unit = clock->unit;
    }
    return 0;
}
static Sint32 sftim_GetTimeUfrm(struct sfd_handle *handle, Sint32 *time,
                                Sint32 *unit)
{
    /* @bug uninitialized: leaves *time and *unit unset while the handle plays
     * or has finished, and returns 0 (0x8c54b5de, 0x8c54b5f0) */
    if (SFTIM_ChkRegularTime(handle, time, unit)) {
        /* @hack redundant: the timer's address, formed and never read */
        (void)&handle->timer;
    }
    return 0;
}
/* TRUE while the handle plays or has finished, whether or not it has failed
 * since. otherwise it sets the time to NOT_PLAYING and returns FALSE */
Bool SFTIM_ChkRegularTime(const struct sfd_handle *handle, Sint32 *time,
                          Sint32 *unit)
{
    Sint32 state;
    state = handle->status;
    if (state != SFD_PLAYING && state != -SFD_PLAYING &&
        state != SFD_FINISHED && state != -SFD_FINISHED) {
        *time = NOT_PLAYING;
        *unit = SFD_NO_UNIT;
        return FALSE;
    }
    return TRUE;
}
Sint32 SFD_OutUsrFrmSync(struct sfd_handle *handle)
{
    struct sfd_timer *timer;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_FRAME);
    }
    timer = &handle->timer;
    timer->user_frames++;
    return 0;
}
Sint32 SFD_SetUsrIsSkipFn(struct sfd_handle *handle, sfd_stage_fn skip)
{
    struct sfd_timer *timer;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SKIP);
    }
    timer = &handle->timer;
    timer->skip = skip;
    return 0;
}
Bool SFD_CmpTime(Sint32 first, Sint32 first_unit, Sint32 second,
                 Sint32 second_unit)
{
    return UTY_CmpTime(first, first_unit, second, second_unit);
}
Bool SFD_IsDispTime(Sint32 first, Sint32 first_unit, Sint32 second,
                    Sint32 second_unit)
{
    return UTY_CmpTime(first, first_unit, second, second_unit);
}
Sint32 SFD_SetUsrTimeFn(struct sfd_handle *handle, sfd_time_fn source)
{
    Sint32 mask;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SOURCE);
    }
    SFLIB_LockCs(&mask);
    SFSET_SetCond(handle, SFD_COND_CLOCK, SFD_CLOCK_USER);
    SFTIM_SetTimeFn(handle, source, SFD_CLOCK_USER);
    SFLIB_UnlockCs(&mask);
    return 0;
}
void SFTIM_SetTimeFn(struct sfd_handle *handle, sfd_time_fn source, Sint32 slot)
{
    struct sfd_timer *timer;
    timer = &handle->timer;
    timer->sources[slot] = source;
}
void SFTIM_Tc2Time(const struct sfd_timecode *code, Sint32 *time, Sint32 *unit)
{
    timecode_converter convert;
    Sint32 index;
    Sint32 rate;
    index = code->rate;
    rate = sftim_prate[index];
    convert = sftim_tc2time[index][code->drop];
    if (convert == NULL) {
        SFLIB_SetErr(NULL, INVALID_RATE);
        *time = 0;
        *unit = SFD_NO_UNIT;
        return;
    }
    convert(rate, code, time, unit);
}

#define READ_TIMECODE(source, hours, minutes, seconds, frames)                 \
    do {                                                                       \
        *(hours) = (source)->hour;                                             \
        *(minutes) = (source)->minute;                                         \
        *(seconds) = (source)->second;                                         \
        *(frames) = (source)->frame + (source)->frame_adjust;                  \
    } while (0)

static void sftim_Tc2TimeN(Sint32 rate, const struct sfd_timecode *code,
                           Sint32 *time, Sint32 *unit)
{
    Sint32 hour;
    Sint32 minute;
    Sint32 second;
    Sint32 frame;
    Sint32 total;
    READ_TIMECODE(code, &hour, &minute, &second, &frame);
    total = (hour * rate * 3600) + (minute * rate * 60) + (second * rate) +
            (frame * FRAME_TICKS);
    *time = total;
    *unit = rate;
}

/* define function, which converts a time code at an NTSC rate as at nominal,
 * the whole rate above. it then takes the NTSC rate as the unit */
#define NOMINAL_RATE(function, nominal)                                        \
    static void function(Sint32 rate, const struct sfd_timecode *code,         \
                         Sint32 *time, Sint32 *unit)                           \
    {                                                                          \
        sftim_Tc2TimeN(nominal, code, time, unit);                             \
        *unit = rate;                                                          \
    }
NOMINAL_RATE(sftim_Tc2Time23N, 24000)
NOMINAL_RATE(sftim_Tc2Time29N, 30000)
NOMINAL_RATE(sftim_Tc2Time59N, 60000)

/* define function, which converts a drop-frame time code. per_hour,
 * per_minute and per_second are the frames each part holds at the whole rate
 * above the NTSC rate. a minute is DROPPED_FRAMES short of its seconds. the
 * code adds them back for each KEPT_MINUTES-th minute, from minute 0 */
#define DROP_RATE(function, per_hour, per_minute, per_second)                  \
    static void function(Sint32 rate, const struct sfd_timecode *code,         \
                         Sint32 *time, Sint32 *unit)                           \
    {                                                                          \
        Sint32 hour;                                                           \
        Sint32 minute;                                                         \
        Sint32 second;                                                         \
        Sint32 frame;                                                          \
        Sint32 total;                                                          \
        READ_TIMECODE(code, &hour, &minute, &second, &frame);                  \
        total = ((hour * (per_hour)) + (minute * (per_minute)) +               \
                 (minute / KEPT_MINUTES * DROPPED_FRAMES) +                    \
                 (second * (per_second)) + frame) *                            \
                FRAME_TICKS;                                                   \
        *time = total;                                                         \
        *unit = rate;                                                          \
    }
DROP_RATE(sftim_Tc2Time23D, 86292, 1438, 24)
DROP_RATE(sftim_Tc2Time29D, 107892, 1798, 30)
DROP_RATE(sftim_Tc2Time59D, 215892, 3598, 60)

void SFTIM_Pause(struct sfd_handle *handle, Sint32 mode)
{
    Sint32 time;
    Sint32 unit;
    Sint32 mask;
    SFLIB_LockCs(&mask);
    /* @hack redundant: the cases of SFD_RESUME and SFD_PAUSE, which do
     * nothing */
    switch (mode) {
    case SFD_RESUME:
        break;
    case SFD_PAUSE:
        break;
    case SFD_STEP:
        SFTIM_GetTimeOneFrmVideo(handle, &time, &unit);
        sftim_AddHnVbIn(handle, time, unit);
        break;
    }
    SFLIB_UnlockCs(&mask);
}
static void sftim_AddHnVbIn(struct sfd_handle *handle, Sint32 time, Sint32 unit)
{
    struct sfd_timer *timer;
    struct sfd_clock *clock;
    Sint32 ticks;
    timer = &handle->timer;
    clock = &SFLIB_libwork.clock;
    ticks = UTY_MulAbDivC(clock->unit, time, unit);
    timer->vsyncs += ticks;
    timer->vblank_time += ticks;
}
void SFTIM_GetTimeOneFrmVideo(const struct sfd_handle *handle, Sint32 *time,
                              Sint32 *unit)
{
    const struct sfd_movie_info *movie;
    Sint32 rate;
    movie = &handle->movie;
    rate = movie->picture_rate;
    if (rate == 0) {
        *time = 0;
        *unit = UNKNOWN_RATE_UNIT;
    } else {
        *time = FRAME_TICKS;
        *unit = sftim_prate[rate];
    }
}
/* set *fps to the picture rate at the handle's speed, in pictures per 1000
 * seconds, or UNKNOWN_FPS */
Sint32 SFD_GetPlayFps(struct sfd_handle *handle, Sint32 *fps)
{
    struct sfd_movie_info *movie;
    struct sfd_timer *timer;
    Sint32 rate;
    *fps = UNKNOWN_FPS;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_FPS);
    }
    timer = &handle->timer;
    movie = &handle->movie;
    rate = movie->picture_rate;
    if (rate != 0) {
        *fps = UTY_MulAbDivC(sftim_prate[rate], timer->speed, SFD_NORMAL_SPEED);
    }
    return 0;
}
Sint32 SFD_OutDispSync(struct sfd_handle *handle, Sint32 time, Sint32 unit)
{
    struct sfd_timer *timer;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_SYNC);
    }
    timer = &handle->timer;
    timer->display_count++;
    timer->display_time = time;
    timer->display_unit = unit;
    return 0;
}
Sint32 SFD_IsDrawTime(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                      Bool *due)
{
    Sint32 *conditions;
    *due = FALSE;
    if (SFLIB_CheckHn(handle) != 0) {
        return SFLIB_SetErr(NULL, INVALID_DRAW);
    }
    conditions = handle->conditions;
    /* without SFD_COND_DISPLAY_ALWAYS, SFD_GetFrm hands out a picture only
     * once it is due. so every picture asked about here is due */
    if (!conditions[SFD_COND_DISPLAY_ALWAYS]) {
        *due = TRUE;
        return 0;
    }
    if (handle->request != SFD_PLAYING) {
        *due = FALSE;
        return 0;
    }
    SFTIM_IsExecTime(handle, time, unit, due, conditions[SFD_COND_DRAW_DELAY]);
    return 0;
}
/* set *due to whether a picture at time is due by the handle's time plus
 * delay. delay is in ticks of the library's clock. under SFD_CLOCK_AUDIO,
 * sftim_IsGrExecTime judges it. while the handle's time is NO_CLOCK, every
 * picture is due. while it is another time of unit SFD_NO_UNIT, as before
 * the handle plays, sftim_IsTakeOffExecTime judges it */
void SFTIM_IsExecTime(struct sfd_handle *handle, Sint32 time, Sint32 unit,
                      Bool *due, Sint32 delay)
{
    struct sfd_clock *clock;
    Sint32 now_time;
    Sint32 now_unit;
    Sint32 clock_unit;
    Float presentation;
    Float now;
    clock = &SFLIB_libwork.clock;
    SFTIM_GetTime(handle, &now_time, &now_unit);
    if (now_unit == SFD_NO_UNIT) {
        if (now_time == NO_CLOCK) {
            *due = TRUE;
        } else {
            sftim_IsTakeOffExecTime(handle, time, unit, due);
        }
        return;
    }
    clock_unit = clock->unit;
    now_time += now_unit * delay / clock_unit;
    presentation = (Float)time * JUDGE_UNIT / (Float)unit;
    now = (Float)now_time * JUDGE_UNIT / (Float)now_unit;
    if (handle->conditions[SFD_COND_CLOCK] == SFD_CLOCK_AUDIO) {
        sftim_IsGrExecTime(handle, due, presentation, now);
    } else if (presentation <= now) {
        *due = TRUE;
    } else {
        *due = FALSE;
    }
}
/* judge a picture before the handle plays. the first picture asked about is
 * due, and starts vblank_time at 0. each later one is due once vblank_time
 * reaches its time */
static void sftim_IsTakeOffExecTime(struct sfd_handle *handle, Sint32 time,
                                    Sint32 unit, Bool *due)
{
    struct sfd_clock *clock;
    struct sfd_timer *timer;
    Sint32 elapsed;
    Sint32 elapsed_unit;
    clock = &SFLIB_libwork.clock;
    timer = &handle->timer;
    if (timer->vblank_time < 0) {
        timer->vblank_time = 0;
        *due = TRUE;
    } else {
        /* @hack spelling: elapsed and elapsed_unit, copies of vblank_time and
         * the clock's unit, where those would do */
        elapsed = timer->vblank_time;
        elapsed_unit = clock->unit;
        /* @hack redundant: a return after the verdict, where the if's end
         * would do */
        if (UTY_CmpTime(time, unit, elapsed, elapsed_unit)) {
            *due = TRUE;
            return;
        } else {
            *due = FALSE;
        }
    }
}
/* judge a picture by the audio clock, its time and now in JUDGE_UNIT ticks.
 * a picture more than SFD_COND_DRAW_MARGIN after now isn't due, and one that
 * far before now is. a picture within the margin gets a verdict, which
 * last_result keeps. while hold_count is at most the hold limit, the verdict
 * is the last one again. otherwise it is whether now has reached the picture.
 * hold_count counts the pictures found due more than the margin before now
 * since the last verdict, each once. the hold limit is 1 for film at normal
 * speed on NTSC, and 0 otherwise */
static void sftim_IsGrExecTime(struct sfd_handle *handle, Bool *due,
                               Float presentation, Float now)
{
    Sint32 *conditions;
    struct sfd_timer *timer;
    Float margin;
    conditions = handle->conditions;
    timer = &handle->timer;
    margin = (Float)conditions[SFD_COND_DRAW_MARGIN];
    if (presentation > now + margin) {
        *due = FALSE;
        return;
    } else if (presentation <= now - margin) {
        *due = TRUE;
        /* @hack spelling: a return in each arm, where one after the if would
         * do */
        if (timer->judged_exec != presentation &&
            timer->last_exec != presentation) {
            timer->last_exec = presentation;
            timer->hold_count = timer->hold_count + 1;
            return;
        } else {
            return;
        }
    }
    /* @hack redundant: the hold limit as a conditional of 1 and 0, where the
     * test's own value would do */
    else if (timer->hold_count <= ((SFLIB_libwork.clock.unit == NTSC_CLOCK &&
                                    handle->movie.picture_rate <= FILM_RATE &&
                                    timer->speed == SFD_NORMAL_SPEED)
                                       ? 1
                                       : 0)) {
        *due = timer->last_result;
    } else if (presentation > now) {
        *due = FALSE;
    } else {
        *due = TRUE;
    }
    timer->hold_count = 0;
    timer->last_result = *due;
    timer->judged_exec = presentation;
}
Bool SFTIM_IsVideoTerm(struct sfd_handle *handle)
{
    struct sfd_timer *timer;
    struct sfd_play_info *play;
    Sint32 end_time;
    Sint32 end_unit;
    Sint32 time;
    Sint32 unit;
    timer = &handle->timer;
    end_time = timer->end_time;
    end_unit = timer->end_unit;
    play = &handle->play;
    if (play->decoded == 0) {
        return TRUE;
    }
    if (end_time == TIME_UNSET) {
        return FALSE;
    }
    if (play->decoded <= SHORT_MOVIE_PICTURES) {
        return TRUE;
    }
    SFTIM_GetTime(handle, &time, &unit);
    end_time += end_unit * (END_FIELDS * SFD_NORMAL_SPEED) / NTSC_CLOCK;
    /* @hack spelling: a test of UTY_CmpTime that returns TRUE or FALSE, where
     * returning its result would do */
    if (UTY_CmpTime(end_time, end_unit, time, unit)) {
        return TRUE;
    }
    return FALSE;
}
void SFTIM_SetSpeed(struct sfd_handle *handle, Sint32 speed)
{
    struct sfd_timer *timer;
    timer = &handle->timer;
    timer->speed = speed;
}
Sint32 SFTIM_GetSpeed(const struct sfd_handle *handle)
{
    const struct sfd_timer *timer;
    timer = &handle->timer;
    return timer->speed;
}
