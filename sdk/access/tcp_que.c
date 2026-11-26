/* tcp_que_tbl, which the packet driver fills and the TCP/IP task empties */
#include <access/packet_queue.h>

struct packet_queue tcp_que_tbl;

void tcp_que_init(struct packet_queue *queue)
{
    struct queued_frame *slot;
    int index;

    queue->head = 0;
    queue->tail = 0;
    for (index = 0; index < TCP_QUEUE_SLOTS; index++) {
        slot = &queue->slots[index];
        slot->ready = 0;
        slot->interface_number = -1;
        slot->length = 0;
    }
}

void tcp_que_regist(struct packet_queue *queue, int length,
                    int interface_number)
{
    unsigned int index;

    index = queue->head % TCP_QUEUE_SLOTS;
    queue->slots[index].ready = 1;
    queue->slots[index].interface_number = interface_number;
    queue->slots[index].length = length;
    /* @bug arithmetic: head and tail take slot 0 twice where they wrap past
     * UINT_MAX, since 2^32 isn't a multiple of TCP_QUEUE_SLOTS. if the queue
     * is empty there, tcp_que_available reports it full, and no frame moves
     * head again. otherwise a frame queued at the wrap is lost if the next
     * comes before the task takes it (0x0cada09c, 0x0cada0da) */
#ifdef NONMATCHING
    /* this side repairs the bug tagged above: head wraps at TCP_QUEUE_SLOTS, so
     * the slots take their turns past any count of frames */
    queue->head = (index + 1) % TCP_QUEUE_SLOTS;
#else
    queue->head++;
#endif
}

void tcp_que_release(struct packet_queue *queue)
{
    unsigned int index;

    index = queue->tail % TCP_QUEUE_SLOTS;
    queue->slots[index].ready = 0;
    queue->slots[index].interface_number = -1;
    queue->slots[index].length = 0;
    /* tail wraps as tcp_que_regist's head does */
#ifdef NONMATCHING
    /* this side repairs the bug tagged in tcp_que_regist, which tail shares
     * with head: tail wraps at TCP_QUEUE_SLOTS */
    queue->tail = (index + 1) % TCP_QUEUE_SLOTS;
#else
    queue->tail++;
#endif
}

int tcp_que_available(struct packet_queue *queue)
{
    if (queue->tail % TCP_QUEUE_SLOTS == (queue->head + 1) % TCP_QUEUE_SLOTS) {
        return 0;
    }
    return 1;
}

int tcp_que_empty(struct packet_queue *queue)
{
    if (queue->tail % TCP_QUEUE_SLOTS == queue->head % TCP_QUEUE_SLOTS) {
        return 1;
    }
    return 0;
}

struct queued_frame *tcp_que_wptr(struct packet_queue *queue)
{
    return &queue->slots[queue->head % TCP_QUEUE_SLOTS];
}

struct queued_frame *tcp_que_rptr(struct packet_queue *queue)
{
    return &queue->slots[queue->tail % TCP_QUEUE_SLOTS];
}
