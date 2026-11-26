/* what the twining, looping, bungee and revolving vines share. each vine poses
 * its own copy of a chain of chunk objects. a player hangs from the chain's
 * tip */
#ifndef SHARED_VINE_H
#define SHARED_VINE_H
#include <ninja.h>
#include <sg_xpt.h>

/* the head of each vine's record, in its task's extra */
struct vine_chain {
    NJS_POINT3 pos; /* the vine's top, or the center its top turns about */
    /* the vine's copy of its model, from syCalloc. each part is the child of
     * the one before */
    NJS_CNK_OBJECT *objects;
    Sint32 count; /* the number of parts in the copy */
    /* the turns about x and about z that set a hanging player along the
     * tip's part */
    Angle tip_ang_x;
    Angle tip_ang_z;
    /* the center of the model of the copy's last part but one, in the world */
    NJS_POINT3 tip;
    /* -1 while a player hangs from the tip. otherwise it counts steps up to the
     * wait before the vine can catch. letting a player go starts the count
     * over. White Jungle's vines count every step. the others count only the
     * steps on which time moves. they also start over when a player comes in
     * reach before the wait is over */
    Sint32 timer;
};

/* two bytes of a vine's task's unk_3c, which White Jungle's vines don't use. a
 * vine copies time_stopped into VINE_HELD when a player catches it. it clears
 * VINE_HELD on a step that finds time moving. while VINE_HELD isn't 0, the vine
 * moves though time is stopped. VINE_DONE is TRUE once the vine has thrown its
 * player */
/* @hack type: VINE_HELD and VINE_DONE read as bytes through unk_3c's address,
 * where (task)->unk_3c.b[0] and (task)->unk_3c.b[2] would do */
#define VINE_HELD(task) (((Sint8 *)&(task)->unk_3c)[0])
#define VINE_DONE(task) (((Sint8 *)&(task)->unk_3c)[2])

/* the twining and looping vines' texture list, model chain and preview
 * model */
extern NJS_TEXLIST vine_texlist;
extern NJS_CNK_OBJECT vine_object;
extern NJS_CNK_OBJECT vine_preview_object;

#endif
