/* ACCESS's receive queue, a ring of TCP_QUEUE_SLOTS frames. a slot holds a
 * frame, its length and the interface it came in on. the ring keeps one slot
 * empty, so it holds TCP_QUEUE_SLOTS - 1 frames at most */
#ifndef ACCESS_PACKET_QUEUE_H
#define ACCESS_PACKET_QUEUE_H
#include <sg_xpt.h>

#define TCP_QUEUE_SLOTS 5
#define TCP_FRAME_BYTES 1536
#define ETHERNET_FRAME_BYTES 1514
#define ETHERNET_MTU 1500

struct queued_frame {
    int ready;
    int interface_number;
    int length;
    Uint8 frame[TCP_FRAME_BYTES];
};

struct packet_queue {
    int reserved;
    unsigned int head;
    unsigned int tail;
    struct queued_frame slots[TCP_QUEUE_SLOTS];
};
extern struct packet_queue tcp_que_tbl;
void tcp_que_init(struct packet_queue *queue);
void tcp_que_regist(struct packet_queue *queue, int length,
                    int interface_number);
void tcp_que_release(struct packet_queue *queue);
int tcp_que_available(struct packet_queue *queue);
int tcp_que_empty(struct packet_queue *queue);
struct queued_frame *tcp_que_wptr(struct packet_queue *queue);
struct queued_frame *tcp_que_rptr(struct packet_queue *queue);
#endif
