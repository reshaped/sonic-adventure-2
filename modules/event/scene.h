/* an event's scene file, and the player that plays its scenes */
#ifndef EVENT_SCENE_H
#define EVENT_SCENE_H
#include <ninja.h>
#include <sg_xpt.h>

#define EVENT_UPGRADES 31
#define EVENT_UPGRADE_LISTS 3
#define EVENT_ATTACHMENTS 16
#define EVENT_ATTACHMENT_PARTS 2
/* the event the staff roll plays over */
#define EVENT_STAFF_ROLL 210
/* the first controller, which plays the event, and the second, which drives
 * the debug editors */
#define EVENT_PLAYER_PAD 0
#define EVENT_EDIT_PAD 1

/* parts drawn on nodes of owner's model. each part's model, or null for
 * none, draws where its node lies in owner's pose */
struct event_attachment {
    NJS_CNK_OBJECT *owner;
    struct {
        NJS_CNK_OBJECT *node;
        NJS_CNK_OBJECT *model;
    } parts[EVENT_ATTACHMENT_PARTS];
};

/* the size a screen sprite of the texture draws at before its scale */
struct event_texture_size {
    Sint16 width, height;
};

/* a model of a scene, still where motion is null. it plays motion, and shape
 * with it where shape isn't null. texture_animation, or null, animates its UVs
 * by event_frame. position is its place where motion's root gives none */
struct event_entity {
    NJS_CNK_OBJECT *object;
    NJS_MOTION *motion;
    NJS_MOTION *shape;
    struct texture_anim *texture_animation;
    NJS_POINT3 position;
    Uint32 flags;
};

/* one of Big's motions, with shape beside it where shape isn't null */
struct event_big_motion {
    NJS_MOTION *motion, *shape;
};
/* Big the Cat in a scene, who plays motions[0] to motions[count - 1] in turn,
 * as playback.h says. loop starts them over after the last */
struct event_big {
    NJS_CNK_OBJECT *object;
    struct event_big_motion *motions;
    Sint32 count;
    Bool loop;
};

/* a scene, frames event frames long. its camera plays cameras[0], or another
 * the debug controls pick. the roots of motions place the effects that name
 * them */
struct event_scene {
    struct event_entity *entities;
    Sint32 entity_count;
    NJS_MOTION **cameras;
    Sint32 camera_count;
    NJS_MOTION **motions;
    Sint32 motion_count;
    struct event_big *big; /* in each scene but scene 0 */
    Sint32 frames;
};

/* a texture that cycles with the event. f is event_frame cut to an integer. the
 * words of sets whose uses name texture then name texture + f % frames */
struct event_texture_animation {
    struct event_texture_set *sets;
    Sint32 texture, frames;
};

/* the scene file, e<event>.prs. it holds scene_count + 1 scenes. scene 0's
 * entities draw throughout the event, and its frames is the event's length.
 * scenes 1 to scene_count play in turn. the loader sets texlist to the list
 * that e<event>texlist.prs holds */
struct event_scene_file {
    struct event_scene *scenes;
    NJS_TEXLIST *texlist;
    Sint32 scene_count;
    struct event_texture_size *texture_sizes; /* by texlist's index */
    struct event_mirrors *mirrors;
    /* the objects whose entities leave afterimages, by slot. the list ends at
     * null, or after EVENT_SLOTS */
    NJS_CNK_OBJECT **slot_objects;
    /* EVENT_UPGRADE_LISTS lists of EVENT_UPGRADES nodes, by upgrade. the first
     * two hold the parts an upgrade adds, and the third the part it replaces.
     * an entry is null where the upgrade has none */
    NJS_CNK_OBJECT **upgrades;
    /* tails[0], or null: the node whose child, and that child's sibling, are
     * Tails' two tails */
    NJS_CNK_OBJECT **tails;
    struct event_attachment *attachments; /* EVENT_ATTACHMENTS of them */
    struct event_texture_animation *animation; /* or null */
};

/* how an entity draws at the scene frame, which its flags and its motion
 * decide. a MULTI_ kind draws under the lights its flags switch on */
enum event_draw_kind {
    EVENT_CULLED = 0,
    EVENT_OBJECT = 1,
    EVENT_MOTION = 2,
    EVENT_SHAPE = 3,
    EVENT_UNFOGGED = 4,
    EVENT_MULTI_OBJECT = 5,
    EVENT_MULTI_MOTION = 6,
    EVENT_MULTI_SHAPE = 7,
    EVENT_MODIFIER = 8,
    EVENT_MODIFIER_MOTION = 9
};
/* an entity's flags */
#define EVENT_SIMPLE_LIGHT 1 /* the Simple drawers, not the Easy ones */
#define EVENT_SKIP_FOG 2 /* without a motion: no fog, and no culling */
/* multi light n, from 1 to 4, which holds event_lights[n - 1]. an entity with
 * any of them takes a MULTI_ kind */
#define EVENT_LIGHT(n) (1U << ((n) + 1))
#define EVENT_LIGHT_MASK                                                       \
    (EVENT_LIGHT(1) | EVENT_LIGHT(2) | EVENT_LIGHT(3) | EVENT_LIGHT(4))
#define EVENT_SHADOW_VOLUME 0x40 /* its model as a modifier volume */
#define EVENT_REFLECTED 0x80 /* drawn in the mirrors too */
/* no MULTI_ kind, whatever its lights, and the scene's Simple drawers */
#define EVENT_FORCE_SIMPLE 0x200

/* the scene file's scenes, the event's messages or null, and its number */
extern struct event_scene *event_scenes;
extern struct message_list *event_messages;
extern Sint32 event_number;
/* the event's clock, in event frames. event_frame counts from the event's
 * start, and event_scene_frame from the start of scene event_scene_index.
 * each step adds event_frame_step to event_frame, and event_previous_frame
 * holds what event_frame was before */
extern Float event_frame;
extern Float event_scene_frame;
extern Float event_previous_frame;
extern Float event_frame_step;
extern Sint32 event_scene_index;
extern struct event_scene_file *event_scene_file; /* at EVENT_SCENE_AREA */
/* event_frame_step at the event's own speed */
#define EVENT_FRAME_STEP 0.5F
/* pace frames as frame_pacing_init(interval, interval) does. event_step then
 * steps the event interval times a frame */
void event_set_frame_interval(int interval);
/* the intervals the event plays at. it plays a frame each vertical blank, or
 * every second blank under cue 0's EVENT_HALF_RATE */
#define EVENT_EVERY_BLANK 1
#define EVENT_EVERY_OTHER_BLANK 2

/* switch multi lights 1 to 4 on or off by flags' EVENT_LIGHT bits */
void event_select_lights(Uint32 flags);

/* the event overlay's callbacks */
void event_init(void);
void event_exit(void);
/* run a frame of the event. it returns OVERLAY_RESET once the pads ask for a
 * reset. it returns OVERLAY_DONE once a sequence has played its events, and
 * OVERLAY_RUNNING until then. the port also returns OVERLAY_RESET where an
 * event's file fails to load */
int event_step(void);

#endif
