/* the Chao kindergarten's rooms. chaostgkinder_step gives a task the
 * kindergarten's step, draw and destructor. the kindergarten opens in the
 * house and changes rooms behind a fade. a room's objects are the task's
 * children, and a change of room destroys them */
#ifndef CHAOSTGKINDER_ROOMS_H
#define CHAOSTGKINDER_ROOMS_H
#include <sg_xpt.h>

struct task;

enum kinder_room_id {
    KINDER_HOUSE,
    KINDER_CORRIDOR,
    KINDER_BLACK_MARKET,
    KINDER_CLASSROOM,
    KINDER_HEALTH_CENTER,
    KINDER_LIBRARY,
    KINDER_PRINCIPAL,
    KINDER_PLAYGROUND,
    KINDER_CONNECT,
    KINDER_EDITOR,
    KINDER_ACCOUNT,
    KINDER_ROOMS
};

/* what ntInfIsNetworkInfo returned as the kindergarten opened. the corridor
 * offers a connection only where it is NTD_OK */
extern Sint32 kinder_network_info;
/* the Chao the player carries, or NULL. a room without the player reads it,
 * and the classroom sets it to the Chao the player takes. the next room with
 * the player hands it to the player */
extern struct task *kinder_carried_chao;

void chaostgkinder_step(struct task *task);
void kinder_house_init(struct task *task);
void kinder_corridor_init(struct task *task);
void kinder_classroom_init(struct task *task);
void kinder_health_center_init(struct task *task);
void kinder_book_prompt_step(struct task *task);
void kinder_editor_init(struct task *task);
void kinder_account_init(struct task *task);
void kinder_principal_room_init(struct task *task);
/* fade out, enter room by entrance, and fade back in. entrance indexes the
 * room's own entrances, and must not be negative. a request made before the
 * last one's room loads is dropped. a room with no such entrance loads
 * nothing, and leaves the pads disabled */
void kinder_request_room(Sint32 room, Sint32 entrance);
/* the house's entrances */
enum kinder_house_entrance { HOUSE_FROM_LOBBY, HOUSE_FROM_CORRIDOR };
/* the corridor's entrances, each the slot of the door it enters by */
enum kinder_corridor_entrance {
    CORRIDOR_FROM_HOUSE = 0,
    CORRIDOR_FROM_CLASSROOM = 2,
    CORRIDOR_FROM_HEALTH_CENTER = 3,
    CORRIDOR_FROM_LIBRARY = 4,
    CORRIDOR_FROM_PRINCIPAL = 5
};
/* leave for the lobby on the next step, without a fade */
void kinder_request_lobby(void);
/* the entrance the player entered the current room by */
Sint32 kinder_current_entrance(void);

#endif
