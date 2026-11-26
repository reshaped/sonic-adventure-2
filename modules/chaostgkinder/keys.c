#include <sg_pdkbd.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "chaostgkinder/keyboard.h"

#define QUEUE_SIZE 64
/* the keys a keyboard's report holds */
#define REPORT_KEYS 6
/* the held keys repeat repeat_delay polls after the last new key, then
 * every repeat_interval polls */
static const int repeat_delay = 24;
static const int repeat_interval = 6;

static Uint8 language;
static Uint8 modifiers;
static Uint8 held[REPORT_KEYS];
static int held_count;
static int repeat_countdown;
static Uint16 queue[QUEUE_SIZE];
static int tail;
static int head;

static void enqueue_key(Uint8 key)
{
    int next = tail + 1;
    if (next == QUEUE_SIZE) {
        next = 0;
    }
    if (next != head) {
        queue[tail] = key | (modifiers << 8);
        tail = next;
    }
}

void kinder_keys_start(void)
{
    pdKbdInit();
    held_count = 0;
    repeat_countdown = 0;
    tail = 0;
    head = 0;
}

void kinder_keys_stop(void)
{
    pdKbdExit();
}

void kinder_keys_poll(void)
{
    const PDS_KEYBOARD *keyboard = pdKbdGetData(PDD_PORT_D0);
    Uint8 keys[sizeof(held)];
    int i;
    int j;
    int key;
    if (keyboard) {
        int count = 0;
        language = keyboard->info->lang;
        modifiers = keyboard->ctrl;
        for (i = 0; (unsigned)i < sizeof(keyboard->key); i++) {
            key = keyboard->key[i];
            /* @hack spelling: a case for each usage that reports no key,
             * where key <= KBD_ERROR_UNDEFINED would do */
            switch (key) {
            case KBD_NO_EVENT:
            case KBD_ERROR_ROLLOVER:
            case KBD_POST_FAIL:
            case KBD_ERROR_UNDEFINED:
                continue;
            }
            keys[count++] = key;
            for (j = 0; j < held_count; j++) {
                if (held[j] == key) {
                    /* @hack spelling: the goto past enqueue_key, where testing
                     * j after the loop would do */
                    goto next_key;
                }
            }
            enqueue_key(key);
            repeat_countdown = repeat_delay;
        next_key:;
        }
        for (i = 0; i < count; i++) {
            held[i] = keys[i];
        }
        held_count = count;
        if (--repeat_countdown == 0) {
            repeat_countdown = repeat_interval;
            for (i = 0; i < held_count; i++) {
                enqueue_key(held[i]);
            }
        }
    } else {
        held_count = 0;
        repeat_countdown = 0;
    }
}

void kinder_keys_clear(void)
{
    head = tail = 0;
}

Uint16 kinder_keys_next(void)
{
    Uint16 key = 0;
    if (head != tail) {
        key = queue[head];
        if (++head == QUEUE_SIZE) {
            head = 0;
        }
    }
    return key;
}

int kinder_key_held(Uint8 key)
{
    int i;
    for (i = 0; i < held_count; i++) {
        if (held[i] == key) {
            return TRUE;
        }
    }
    return FALSE;
}

int kinder_keys_active(void)
{
    return modifiers || held_count;
}

Uint8 kinder_key_modifiers(void)
{
    return modifiers;
}
