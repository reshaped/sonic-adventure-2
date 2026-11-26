/* the rooms' title banners, and the services the rooms open. they are the
 * classroom's lessons, the health center's checkup, the principal's advice
 * and the account menu */
#ifndef CHAOSTGKINDER_SERVICES_H
#define CHAOSTGKINDER_SERVICES_H
#include <sg_xpt.h>

struct task;

enum kinder_banner_frame {
    KINDER_BANNER_PRINCIPAL = 0,
    KINDER_BANNER_HEALTH_CENTER = 1,
    KINDER_BANNER_CLASSROOM = 4
};

void kinder_banner_load(void);
void kinder_banner_release(void);
void kinder_banner_select(Sint32 selection, Uint32 color);
void kinder_banner_draw(void);

/* kinder_lesson_state's states. KINDER_LESSON_ABSENT is the state without a
 * lesson. the classroom returns to the corridor once the state is
 * KINDER_LESSON_ABSENT or later */
enum lesson_state {
    KINDER_LESSON_RUNNING = 0,
    KINDER_LESSON_ABSENT = 5,
    KINDER_LESSON_DONE = 6
};
struct task *kinder_lesson_create(struct task *parent);
Sint8 kinder_lesson_state(void);
void kinder_lesson_release(void);
void kinder_lesson_set_state(Sint8 state);
/* kinder_checkup_state's states. KINDER_CHECKUP_MEDICINE holds while the
 * medicine's panel shows. KINDER_CHECKUP_ABSENT is the state without a
 * checkup. the health center returns to the corridor once the state is
 * KINDER_CHECKUP_ABSENT or later */
enum checkup_state {
    KINDER_CHECKUP_OPEN = 0,
    KINDER_CHECKUP_MEDICINE = 1,
    KINDER_CHECKUP_ABSENT = 3,
    KINDER_CHECKUP_DONE = 4
};
struct task *kinder_checkup_create(struct task *parent);
Sint8 kinder_checkup_state(void);
void kinder_checkup_release(void);
void kinder_checkup_set_state(Sint8 state);
/* kinder_advice_state's states. KINDER_ADVICE_ABSENT is the state without a
 * menu. the office returns to the corridor once the state is
 * KINDER_ADVICE_ABSENT or later. the menu sets KINDER_ADVICE_UNREACHED where
 * it finds KINDER_ADVICE_ABSENT or later while its topic list is open. no
 * path gets there: the list keeps the state at KINDER_ADVICE_LIST_OPEN
 * until it sets KINDER_ADVICE_LIST_CLOSED */
enum advice_state {
    KINDER_ADVICE_OPEN,
    KINDER_ADVICE_LIST_OPEN,
    KINDER_ADVICE_LIST_CLOSED,
    KINDER_ADVICE_UNREACHED,
    KINDER_ADVICE_ABSENT,
    KINDER_ADVICE_LEAVING
};
struct task *kinder_advice_create(struct task *parent);
Sint8 kinder_advice_state(void);

/* kinder_account_menu_result's results: the menu runs, or it closed after
 * the server saved a profile, or without */
enum account_menu_result { MENU_ACTIVE, MENU_SAVED, MENU_CLOSED };
int kinder_account_menu_create(struct task *parent);
Sint32 kinder_account_menu_result(void);
void kinder_account_menu_ids(Sint32 *ids);

#endif
