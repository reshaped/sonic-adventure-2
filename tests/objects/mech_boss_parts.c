/* a mech boss's make, charge and beam where the heap can't give a part they
 * ask for. the make kills the boss where it gets no aim. the charge goes on
 * without its laser. the beam kills whichever of its sight and gun it got,
 * and asks for both again at its next step. Hot Shot's track holds its sight
 * still from a shot until the shot's bomb ends, and a bomb that ends after
 * the sight writes nothing of it. Big Foot's hit in the air kills its laser
 * once. a build defines BIGFOOT, FDOG or HOTSHOT, and finds that boss's
 * source with only these functions left */
#include <assert.h>
#include <string.h>

#if defined(BIGFOOT)
#include "boss_bigfoot/bigfoot.c"
#define BOSS(name) bigfoot_##name
#define BOSS_LASER BIGFOOT_LASER
#elif defined(FDOG)
#include "boss_fdog/fdog.c"
#define BOSS(name) fdog_##name
#define BOSS_LASER FDOG_LASER
#define BEAM fdog_beam
#define MODE_SHOOT MODE_BEAM
#else
#include "boss_hotshot/hotshot.c"
#define BOSS(name) hotshot_##name
#define BOSS_LASER HOTSHOT_LASER
#define BEAM hotshot_track
#define MODE_SHOOT MODE_TRACK
#endif

/* where the charge starts, past each boss's arena edge. player 0 stands 1
 * behind, nearer than PASS_X */
#define FAR_X 1000.0F

/* a part the stubs make: a sight, a beam gun, a laser or a capsule. each has
 * room for a sight's lock. a kill sets the lock to KILLED_LOCK, which then
 * changes only where a write lands in the killed part */
struct part {
    struct task task;
    struct task_work work;
    Sint32 lock;
};

#define PARTS 32
#define KILLED_LOCK 0x5a5a5a5a

NJS_CNK_OBJECT BOSS(turret_node);
#ifdef BEAM
NJS_CNK_OBJECT BOSS(gun_node);
#endif
struct task_work *player_work[PLAYER_SLOTS];

static struct part parts_made[PARTS];
static Bool killed[PARTS];
static int made;

static struct task boss_task;
static struct task_work boss_work;
static struct boss_record boss_record;
static struct mover boss_mover;
static struct motion_control boss_control;
static struct collision_work boss_collision;
static struct collision_shape boss_shapes[LOW_SHAPE + 1];
static struct BOSS(parts) boss_parts;
static struct task_work player;
static int boss_kills, parts_freed, aims_live;

static NJS_CNK_OBJECT *refused_aim;
static Bool refuse_laser, refuse_sight, refuse_gun;
/* the busy word of the last shot fired */
static Sint32 *shot_busy;

static struct task *make(void)
{
    struct part *part;

    assert(made < PARTS);
    part = &parts_made[made];
    killed[made++] = FALSE;
    memset(part, 0, sizeof(*part));
    part->task.work = &part->work;
    return &part->task;
}

/* the part a stub made that task is */
static struct part *part_of(struct task *task)
{
    struct part *part = (struct part *)task;

    assert(task != NULL && part >= parts_made && part < parts_made + made);
    return part;
}

static Bool live(struct task *task)
{
    return !killed[part_of(task) - parts_made];
}

static int live_parts(void)
{
    int count = 0;
    int i;

    for (i = 0; i < made; i++) {
        count += !killed[i];
    }
    return count;
}

/* the boss, with its record and parts, and the stubs' counts at 0 */
static struct task *boss(void)
{
    memset(&boss_task, 0, sizeof(boss_task));
    memset(&boss_work, 0, sizeof(boss_work));
    memset(&boss_record, 0, sizeof(boss_record));
    memset(&boss_mover, 0, sizeof(boss_mover));
    memset(&boss_parts, 0, sizeof(boss_parts));
    memset(&player, 0, sizeof(player));
    boss_task.work = &boss_work;
    boss_task.motion.p = &boss_mover;
    boss_task.extra.p = &boss_record;
    boss_record.motion = &boss_mover;
    boss_record.block = &boss_control;
    boss_record.own = &boss_parts;
    boss_work.collision = &boss_collision;
    boss_collision.shapes = boss_shapes;
    player_work[0] = &player;
    made = 0;
    boss_kills = 0;
    parts_freed = 0;
    aims_live = 0;
    refused_aim = NULL;
    refuse_laser = refuse_sight = refuse_gun = FALSE;
    shot_busy = NULL;
    return &boss_task;
}

struct task *task_create(Uint16 flags, int level,
                         void (*step)(struct task *task), const char *name)
{
    return &boss_task;
}

void task_kill(struct task *task)
{
    if (task == &boss_task) {
        boss_kills++;
        return;
    }
    assert(live(task));
    killed[part_of(task) - parts_made] = TRUE;
    part_of(task)->lock = KILLED_LOCK;
}

struct boss_record *boss_record_create(struct task *task)
{
    return &boss_record;
}

void *boss_record_alloc_own(struct boss_record *record, Uint32 size)
{
    assert(size == sizeof(boss_parts));
    record->own = &boss_parts;
    return &boss_parts;
}

void boss_record_free_parts(struct task *task)
{
    parts_freed++;
}

void *syCalloc(Uint32 count, Uint32 size)
{
    assert(count * size == sizeof(boss_parts));
    return &boss_parts;
}

struct node_aim *node_aim_create(NJS_CNK_OBJECT *node, NJS_POINT3 *pivot)
{
    static char aims[2];

    if (node == refused_aim) {
        return NULL;
    }
    assert(aims_live < 2);
    return (struct node_aim *)&aims[aims_live++];
}

void node_aim_free(struct node_aim *aim)
{
    assert(aim != NULL && aims_live > 0);
    aims_live--;
}

void node_aim_set_limits(struct node_aim *aim, struct node_aim_limits *limits)
{
    assert(aim != NULL);
}

struct task *mech_boss_capsule_create(struct task *parent,
                                      struct mech_boss_capsule_spec *spec)
{
    return make();
}

Bool collision_init(struct task *task, const struct collision_shape *shapes,
                    int count, Uint8 kind)
{
    task->work->collision = &boss_collision;
    return TRUE;
}

struct task *mech_boss_laser_create(struct task *parent,
                                    struct mech_boss_laser_spec *spec)
{
    return refuse_laser ? NULL : make();
}

struct task *mech_boss_laser_beam_create(struct task *parent,
                                         struct mech_boss_laser_beam_spec *spec)
{
    assert(live(parent));
    return NULL;
}

void mech_boss_laser_set_mode(struct task *task, Sint8 mode)
{
    assert(live(task));
}

struct angles *mech_boss_laser_angles(struct task *task)
{
    static struct angles angles;

    assert(live(task));
    return &angles;
}

struct task *mech_boss_sight_create(struct task *parent, NJS_POINT3 *target,
                                    Float speed)
{
    return refuse_sight ? NULL : make();
}

Sint32 *mech_boss_sight_lock(struct task *task)
{
    assert(live(task));
    return &part_of(task)->lock;
}

Bool mech_boss_sight_hide_unlocked(struct task *task)
{
    assert(live(task));
    return part_of(task)->lock != 1;
}

struct task *mech_boss_beam_gun_create(struct task *parent, NJS_POINT3 *pos,
                                       struct angles *ang)
{
    return refuse_gun ? NULL : make();
}

void mech_boss_beam_gun_fire(struct task *task, NJS_POINT3 *target,
                             Sint32 *busy)
{
    assert(live(task));
    *busy = 1;
    shot_busy = busy;
}

void mech_boss_beam_gun_charge(struct task *task)
{
    assert(live(task));
}

void node_aim_point(struct task *task, struct node_aim *aim, Float range,
                    NJS_POINT3 *target)
{
    assert(aim != NULL);
}

Bool node_aim_return(struct node_aim *aim)
{
    assert(aim != NULL);
    return TRUE;
}

void node_aim_set(struct node_aim *aim, struct angles *ang)
{
    assert(aim != NULL);
}

void node_aim_reset(struct node_aim *aim)
{
    assert(aim != NULL);
}

/* the calls whose results no check here reads */
void motion_init(struct motion_control *control,
                 const struct motion_table_entry *table)
{
}

void mover_set_gravity(struct task *task, Float gravity)
{
}

void pad_disable_player(int player)
{
}

void motion_link(struct motion_control *control, Sint32 index)
{
}

Angle angle_approach(Angle from, Angle to, Angle step)
{
    return to;
}

Angle mover_turn_to_target_by(struct task *task, Angle step)
{
    return 0;
}

Float njSin(Angle n)
{
    return 0.0F;
}

Float njCos(Angle n)
{
    return 1.0F;
}

Float njScalor(NJS_VECTOR *v)
{
    return 0.0F;
}

Float njUnitVector(NJS_VECTOR *v)
{
    return 0.0F;
}

void vector_diff(const NJS_VECTOR *first, const NJS_VECTOR *second,
                 NJS_VECTOR *out)
{
}

void camera_bosspoint_set_level(int camera, int level)
{
}

void camera_remove_mode(int camera, int index)
{
}

int camera_top(int camera)
{
    return 0;
}

void mech_boss_camera_far(struct task *task, int unused)
{
}

void mech_boss_camera_near(int unused)
{
}

Sint8 sound_play(int id, const void *key, Sint8 priority, Sint16 volume)
{
    return 0;
}

Sint8 sound_play_at_for(int id, const void *key, Sint8 priority, Sint16 volume,
                        Sint16 duration, const NJS_POINT3 *pos)
{
    return 0;
}

void sound_set_volume(const void *key, int id, Sint8 volume)
{
}

void vibration_start(int player, int power, int freq, int time)
{
}

/* make the boss where the heap refuses each aim in turn, then with each */
static void check_make(void)
{
    NJS_CNK_OBJECT *aims[] = {
        &BOSS(turret_node),
#ifdef BEAM
        &BOSS(gun_node),
#endif
    };
    int count = sizeof(aims) / sizeof(aims[0]);
    NJS_POINT3 pos = {0.0F, 0.0F, 0.0F};
    int i;

    for (i = 0; i < count; i++) {
        boss();
        refused_aim = aims[i];
        BOSS(create)(&pos, 0);
        assert(boss_kills == 1 && parts_freed == 1 && aims_live == 0);
    }
    boss();
    BOSS(create)(&pos, 0);
    assert(boss_kills == 0 && aims_live == count);
}

/* run a charge from its start to past the arena's edge, where the heap gives
 * its laser or refuses it */
static void charge(Bool refused)
{
    struct task *task = boss();

    refuse_laser = refused;
    boss_parts.turret_aim = node_aim_create(&BOSS(turret_node), NULL);
    boss_work.mode = MODE_CHARGE;
    boss_work.flags = BOSS_LASER;
    boss_work.pos.x = FAR_X;
    boss_mover.spd.x = 1.0F;
    player.pos.x = FAR_X - 1.0F;
    BOSS(charge)(task);
    assert(boss_work.submode == 2);
    assert(((boss_work.flags & BOSS_LASER) == 0) == refused);
    BOSS(charge)(task);
    assert(boss_work.mode == MODE_COME_ROUND && boss_parts.laser == NULL);
    assert(made == !refused && live_parts() == 0);
}

/* a charge where the heap gives the laser, and one where it refuses it */
static void check_charge(void)
{
    charge(FALSE);
    charge(TRUE);
}

#ifdef BEAM
/* start a beam where the heap refuses the sight, then the gun, then gives
 * both */
static void check_beam(void)
{
    struct task *task = boss();

    boss_work.mode = MODE_SHOOT;
    boss_parts.gun_aim = node_aim_create(&BOSS(gun_node), NULL);
    refuse_sight = TRUE;
    BEAM(task);
    assert(boss_work.submode == 0 && live_parts() == 0);
    assert(boss_parts.sight == NULL && boss_parts.beam_gun == NULL);
    refuse_sight = FALSE;
    refuse_gun = TRUE;
    BEAM(task);
    assert(boss_work.submode == 0 && live_parts() == 0);
    assert(boss_parts.sight == NULL && boss_parts.beam_gun == NULL);
    refuse_gun = FALSE;
    BEAM(task);
    assert(boss_work.submode == 1);
    assert(live(boss_parts.sight) && live(boss_parts.beam_gun));
}
#endif

#ifdef HOTSHOT
/* step a track of one shot from its start to its end, and return its sight.
 * the shot's bomb ends bomb_steps steps after the shot, or outlives the track
 * where bomb_steps is 0. at each step the sight's lock is 1 from the shot
 * until the bomb ends */
static struct task *track(int bomb_steps)
{
    struct task *task = &boss_task;
    struct task *sight = NULL;
    int since_shot = 0;
    int i;

    boss_work.mode = MODE_TRACK;
    boss_work.submode = 0;
    boss_record.power = 2.0F;
    shot_busy = NULL;
    for (i = 0; boss_work.mode == MODE_TRACK; i++) {
        assert(i < BEAM_AIM_STEPS + TRACK_END_STEPS + 4);
        if (shot_busy != NULL && ++since_shot == bomb_steps) {
            *shot_busy = 0;
        }
        hotshot_track(task);
        if (boss_parts.sight != NULL) {
            sight = boss_parts.sight;
            assert(part_of(sight)->lock ==
                   (shot_busy != NULL &&
                    (bomb_steps == 0 || since_shot < bomb_steps)));
        }
    }
    assert(shot_busy != NULL && !live(sight));
    return sight;
}

/* a bomb that ends within its track, one that outlives it, and a track that
 * starts after a shot whose bomb never ends */
static void check_beam_busy(void)
{
    struct task *sight;

    boss();
    boss_parts.gun_aim = node_aim_create(&BOSS(gun_node), NULL);
    track(10);
    sight = track(0);
    *shot_busy = 0;
    assert(part_of(sight)->lock == KILLED_LOCK);
    track(0);
    hotshot_track(&boss_task);
    hotshot_track(&boss_task);
    assert(boss_work.submode == 1 && part_of(boss_parts.sight)->lock == 0);
}
#endif

#ifdef BIGFOOT
/* two hits in the air, the first while the charge's laser lives. the first
 * kills the laser, and the second finds none to kill */
static void check_hit_in_air(void)
{
    struct task *task = boss();

    boss_parts.turret_aim = node_aim_create(&BOSS(turret_node), NULL);
    boss_parts.laser = make();
    boss_work.mode = MODE_HIT_IN_AIR;
    bigfoot_hit_in_air(task);
    assert(boss_parts.laser == NULL && live_parts() == 0);
    boss_work.submode = 0;
    bigfoot_hit_in_air(task);
}
#endif

int main(void)
{
    check_make();
    check_charge();
#ifdef BEAM
    check_beam();
#endif
#ifdef HOTSHOT
    check_beam_busy();
#endif
#ifdef BIGFOOT
    check_hit_in_air();
#endif
    return 0;
}
