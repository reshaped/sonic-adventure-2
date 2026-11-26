/* a connection's send and receive windows, rings of win_size bytes from the
 * buffer pool. while external_send is set, the send window holds a ring of
 * sendf_entry records instead, for TCP_Writef's bytes */
#include <access/at_types.h>
#include <sg_xpt.h>
#include <string.h>
#include <access/tcp_private.h>

/* the index of the send window's last slot while external_send is set. the
 * slots in use run from send_head to send_tail, wrapping past the last, and
 * send_count counts them */
#define SENDF_LAST_ENTRY 8

/* a TCP_Writef's bytes, at vptr. the peer has acknowledged its first acked
 * bytes, and len more await it. request gets the write's notify once the peer
 * has them all */
struct sendf_entry {
    AT_Vptr vptr;
    Sint16 len;
    Sint16 acked;
    Sint32 request;
};

int TCP_PutSendWindow(AT_MaxBuffs *buffers, struct tcb *connection,
                      Sint32 request)
{
    /* the write of buffers, whose bytes wait in pending_send until the send
     * window has room for them all. request gets its notify once they are
     * copied there. -1 while a TCP_Writef's bytes are unacknowledged */
    if (connection->external_send == 1 && TCP_SendWindowCc(connection) != 0) {
        return -1;
    }
    connection->write_request = request;
    memcpy(&connection->pending_send, buffers, sizeof(AT_MaxBuffs));
    connection->external_send = 0;
    return 0;
}

int TCP_PutSendfWindow(const Uint32 *address, Sint16 len,
                       struct tcb *connection, Sint32 request)
{
    struct sendf_entry entry;
    AT_Vptr local;
    AT_Vptr window;

    /* the write of len bytes at *address. they stay there while a slot of the
     * send window names them. request gets its notify once the peer has them
     * all. -1 while a TCP_Write's bytes are unacknowledged, or while every slot
     * is in use */
    local.type = AT_PTR_TYPE_LOCAL;
    window.type = AT_PTR_TYPE_LOCAL;
    if (connection->external_send == 0 && TCP_SendWindowCc(connection) != 0) {
        return -1;
    }
    if ((Uint32)connection->send_count > SENDF_LAST_ENTRY) {
        return -1;
    }
    entry.vptr.buff = *address;
    entry.vptr.type = AT_PTR_TYPE_EXTERNAL;
    entry.len = len;
    entry.acked = 0;
    entry.request = request;
    OS_SetVptrLocal(&local, &entry);
    memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
    if (connection->send_tail) {
        OS_IncVptr(&window, connection->send_tail * sizeof(struct sendf_entry));
    }
    OS_Vmemcpy(&window, &local, sizeof(struct sendf_entry));
    connection->pending_send.total_len += len;
    connection->send_count++;
    if (connection->send_tail < SENDF_LAST_ENTRY) {
        connection->send_tail++;
    } else {
        connection->send_tail = 0;
    }
    connection->external_send = 1;
    return 0;
}

#ifdef NONMATCHING
/* output gets pending's bytes from offset to offset + len, the first of them
 * in its last buffer. -1 where pending doesn't hold them all */
static int pending_send_slice(const AT_MaxBuffs *pending, int offset, int len,
                              AT_MaxBuffs *output)
{
    const AT_VptrArray *source;
    AT_VptrArray *destination;
    int first;
    int i;
    int count;
    int remaining;
    int bytes;

    output->bufnum = 0;
    output->total_len = 0;
    if (pending->bufnum < 0 || pending->bufnum > AT_MAX_BUFFERS || offset < 0 ||
        len < 0 || offset >= pending->total_len ||
        len > pending->total_len - offset) {
        return -1;
    }
    first = 0;
    while (first < pending->bufnum && offset >= pending->buffers[first].len) {
        if (pending->buffers[first].len < 0) {
            return -1;
        }
        offset -= pending->buffers[first].len;
        first++;
    }
    if (first == pending->bufnum) {
        return -1;
    }
    remaining = len;
    for (i = first; i < pending->bufnum; i++) {
        bytes = pending->buffers[i].len - (i == first ? offset : 0);
        if (bytes < 0) {
            return -1;
        }
        if (bytes >= remaining) {
            break;
        }
        remaining -= bytes;
    }
    if (i == pending->bufnum) {
        return -1;
    }
    count = i - first + 1;
    remaining = len;
    for (i = first; i < first + count; i++) {
        source = &pending->buffers[i];
        destination = &output->buffers[count - 1 - (i - first)];
        destination->vptr = source->vptr;
        if (offset != 0) {
            OS_IncVptr(&destination->vptr, offset);
        }
        bytes = source->len - offset;
        destination->len = bytes < remaining ? bytes : remaining;
        remaining -= destination->len;
        offset = 0;
    }
    output->bufnum = count;
    output->total_len = len;
    return 0;
}
#endif

int TCP_GetSendWindow(int offset, int len, AT_MaxBuffs *buffs,
                      struct tcb *connection)
{
    /* buffs must be empty. it gets the len bytes from offset on, the first of
     * them in its last buffer. offset counts from the first byte the peer
     * hasn't acknowledged. with external_send set, the result is -1 for len 0
     * or no slot in use. otherwise it is -1 for an offset past the bytes */
    if (connection->external_send == 1) {
        struct sendf_entry entry;
        AT_Vptr window;
        AT_Vptr data;
        AT_Vptr local;
        AT_Vptr temp;
        int skip = offset;
        int rest = len;
        int index;
        int count;
        int left;
        int right;
        Sint16 n;
        AT_VptrArray *segment;

        window.type = AT_PTR_TYPE_LOCAL;
        data.type = AT_PTR_TYPE_LOCAL;
        local.type = AT_PTR_TYPE_LOCAL;
        if (len == 0 || connection->send_count == 0) {
            return -1;
        }
        OS_SetVptrLocal(&local, &entry);
        index = connection->send_head;
        memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
        if (index) {
            OS_IncVptr(&window, index * sizeof(struct sendf_entry));
        }
        OS_Vmemcpy(&local, &window, sizeof(struct sendf_entry));
        if (skip) {
            while (entry.len <= skip) {
                skip -= entry.len;
                if (index < SENDF_LAST_ENTRY) {
                    index++;
                } else {
                    index = 0;
                }
                memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
                if (index) {
                    OS_IncVptr(&window, index * sizeof(struct sendf_entry));
                }
                OS_Vmemcpy(&local, &window, sizeof(struct sendf_entry));
            }
            memcpy(&data, &entry.vptr, sizeof(AT_Vptr));
            /* @hack spelling: the difference assigned to entry.len, where
             * entry.len -= skip would do */
            entry.len = entry.len - skip;
            OS_IncVptr(&data, entry.acked);
            OS_IncVptr(&data, skip);
        } else {
            memcpy(&data, &entry.vptr, sizeof(AT_Vptr));
            OS_IncVptr(&data, entry.acked);
        }
        offset = buffs->bufnum;
        while (rest) {
            segment = &buffs->buffers[buffs->bufnum];
            if (rest >= entry.len) {
                buffs->total_len += (segment->len = entry.len);
                /* @hack spelling: the difference assigned to rest, where
                 * rest -= entry.len would do */
                rest = rest - entry.len;
            } else {
                segment->len = rest;
                buffs->total_len += rest;
                rest = 0;
            }
            memcpy(&segment->vptr, &data, sizeof(AT_Vptr));
            buffs->bufnum++;
            if (rest) {
                if (index < SENDF_LAST_ENTRY) {
                    index++;
                } else {
                    index = 0;
                }
                memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
                if (index) {
                    OS_IncVptr(&window, index * sizeof(struct sendf_entry));
                }
                OS_Vmemcpy(&local, &window, sizeof(struct sendf_entry));
                memcpy(&data, &entry.vptr, sizeof(AT_Vptr));
            }
        }
        count = buffs->bufnum - offset;
        for (left = (count - 2) / 2, right = (count + 1) / 2;
             left >= 0 && right < count; left--, right++) {
            memcpy(&temp, &buffs->buffers[right].vptr, sizeof(AT_Vptr));
            memcpy(&buffs->buffers[right].vptr, &buffs->buffers[left].vptr,
                   sizeof(AT_Vptr));
            /* @hack spelling: the address alone copied to left, where the
             * whole vptr would do: every entry's is AT_PTR_TYPE_EXTERNAL */
            memcpy(&buffs->buffers[left].vptr, &temp, sizeof(Uint32));
            n = buffs->buffers[right].len;
            buffs->buffers[right].len = buffs->buffers[left].len;
            buffs->buffers[left].len = n;
        }
    } else {
        AT_VptrArray *segment;
        int i;
        Uint16 start = offset;
        int first = -1;
        int out = 0;
        Uint16 pos = connection->send_head + start;
        Uint16 skip;
        int j;

        if (offset >=
            connection->send_count + connection->pending_send.total_len) {
            return -1;
        }
        if (offset < connection->send_count) {
            /* @hack spelling: bound, a copy of win_size, where win_size would
             * do */
            int bound = win_size;

            if (offset + len <= connection->send_count) {
                if ((Sint32)connection->send_head + offset < bound) {
                    if ((Sint32)connection->send_head + offset + len <= bound) {
                        buffs->bufnum = 1;
                        memcpy(&buffs->buffers[0].vptr,
                               &connection->send_window, sizeof(AT_Vptr));
                        OS_IncVptr(&buffs->buffers[0].vptr, pos);
                        buffs->buffers[0].len = len;
                    } else {
                        buffs->bufnum = 2;
                        memcpy(&buffs->buffers[0].vptr,
                               &connection->send_window, sizeof(AT_Vptr));
                        memcpy(&buffs->buffers[1].vptr,
                               &connection->send_window, sizeof(AT_Vptr));
                        OS_IncVptr(&buffs->buffers[1].vptr, pos);
                        buffs->buffers[0].len =
                            len - (buffs->buffers[1].len = win_size - pos);
                    }
                } else {
                    buffs->bufnum = 1;
                    memcpy(&buffs->buffers[0].vptr, &connection->send_window,
                           sizeof(AT_Vptr));
                    OS_IncVptr(&buffs->buffers[0].vptr, pos - win_size);
                    buffs->buffers[0].len = len;
                }
                buffs->total_len = len;
                goto done;
            }
            len -= connection->send_count - start;
        } else {
#ifdef NONMATCHING
            /* this side repairs the two bugs tagged below. pending_send_slice
             * takes the bytes without cutting pending_send's own buffer or
             * reading before buffs's buffers[0], and returns -1 where
             * pending_send doesn't hold them all */
            return pending_send_slice(&connection->pending_send,
                                      offset - connection->send_count, len,
                                      buffs);
#else
            offset -= connection->send_count;
            segment = connection->pending_send.buffers;
            for (i = 0; i < connection->pending_send.bufnum; segment++, i++) {
                if (offset < segment->len) {
                    first = i;
                    skip = offset;
                    buffs->bufnum = 1;
                    memcpy(&buffs->buffers[0].vptr, &segment->vptr,
                           sizeof(AT_Vptr));
                    OS_IncVptr(&buffs->buffers[0].vptr, skip);
                    buffs->total_len = len;
                    buffs->buffers[0].len = len;
                    if (offset + len <= segment->len) {
                        /* @hack redundant: the stores of len, which the two
                         * lines above made */
                        buffs->total_len = len;
                        buffs->buffers[0].len = len;
                        goto done;
                    } else {
                        /* @bug logic: drops the last skip bytes of the
                         * pending buffer from the write. the cut shortens
                         * pending_send's own buffer, where the copy in buffs
                         * was meant, and leaves its address as it was.
                         * pending_send.total_len still counts the bytes
                         * (0x0cadd9ea) */
                        segment->len -= skip;
                        buffs->buffers[0].len = buffs->total_len = segment->len;
                    }
                    i++;
                    while (i < connection->pending_send.bufnum) {
                        /* @bug memory: reads before buffs's buffers[0] once
                         * the bytes skipped outnumber the buffers. the shift
                         * that makes room at buffers[0] reads from index
                         * i - skip - 1 down, where i - first - 1 is meant.
                         * skip counts bytes, and first a buffer
                         * (0x0cadda46) */
                        for (j = i - first, offset = i - skip - 1; j > 0;
                             j--, offset--) {
                            memcpy(&buffs->buffers[j].vptr,
                                   &buffs->buffers[offset].vptr,
                                   sizeof(AT_Vptr));
                            buffs->buffers[j].len = buffs->buffers[offset].len;
                        }
                        /* @hack spelling: the difference assigned to len,
                         * where len -= segment->len would do */
                        len = len - segment->len;
                        buffs->bufnum++;
                        memcpy(&buffs->buffers[0].vptr, &(++segment)->vptr,
                               sizeof(AT_Vptr));
                        if (len <= segment->len) {
                            buffs->buffers[0].len = len;
                            buffs->total_len += len;
                            goto done;
                        }
                        buffs->buffers[0].len = segment->len;
                        buffs->total_len += segment->len;
                        i++;
                    }
                    buffs->buffers[0].len = buffs->buffers[1].len =
                        buffs->total_len = 0;
                    goto done;
                }
                offset -= segment->len;
            }
            goto done;
#endif
        }

        for (segment = &connection->pending_send.buffers[i = 0];
             i < connection->pending_send.bufnum; segment++, i++) {
            if (len <= segment->len) {
                buffs->bufnum = 1;
                memcpy(&buffs->buffers[0].vptr, &segment->vptr,
                       sizeof(AT_Vptr));
                buffs->total_len = len;
                buffs->buffers[0].len = len;
                out = 1;
                break;
            }
            len -= segment->len;
        }
        segment--;
        i--;
        while (i > first) {
            /* @hack spelling: source, a copy of segment, where segment would
             * do */
            AT_VptrArray *source;

            buffs->bufnum++;
            memcpy(&buffs->buffers[out].vptr, &(source = segment)->vptr,
                   sizeof(AT_Vptr));
            buffs->total_len += (buffs->buffers[out].len = source->len);
            out++;
            segment--;
            /* @hack spelling: the difference assigned to i, where i-- would
             * do */
            i = i - 1;
        }
        segment = &buffs->buffers[out];
        pos = connection->send_head + start;
        if (pos < win_size) {
            /* @hack redundant: the label window, which no jump takes */
        window: /* NOLINT(clang-diagnostic-unused-label) */
            if ((Uint32)(connection->send_head + connection->send_count) <=
                win_size) {
                buffs->bufnum++;
                memcpy(&segment->vptr, &connection->send_window,
                       sizeof(AT_Vptr));
                OS_IncVptr(&segment->vptr, pos);
                segment->len = connection->send_count - start;
            } else {
                buffs->bufnum += 2;
                memcpy(&segment->vptr, &connection->send_window,
                       sizeof(AT_Vptr));
                memcpy(&segment[1].vptr, &connection->send_window,
                       sizeof(AT_Vptr));
                OS_IncVptr(&segment[1].vptr, pos);
                segment->len = connection->send_count - start -
                               (segment[1].len = win_size - pos);
            }
        } else {
            buffs->bufnum++;
            memcpy(&segment->vptr, &connection->send_window, sizeof(AT_Vptr));
            OS_IncVptr(&segment->vptr, pos - win_size);
            segment->len = connection->send_count - start;
        }
        buffs->total_len += connection->send_count - start;
    }
done:
    return 0;
}

int TCP_FreeSendWindow(int len, struct tcb *connection)
{
    /* the peer has acknowledged len more bytes. each sendf_entry whose bytes it
     * has all of gets its notify. a TCP_Write's bytes in pending_send move into
     * the window once it has room for them all. with external_send clear, a
     * call that leaves none waiting gives write_request its notify */
    if (connection->external_send == 1) {
        struct sendf_entry entry;
        AT_Vptr window;
        /* @hack redundant: spare, whose type is set and never read */
        AT_Vptr spare;
        AT_Vptr local;
        int rest;
        Uint32 index;

        rest = len;
        window.type = AT_PTR_TYPE_LOCAL;
        spare.type = AT_PTR_TYPE_LOCAL;
        local.type = AT_PTR_TYPE_LOCAL;
        if (len != 0 && connection->send_count != 0) {
            OS_SetVptrLocal(&local, &entry);
            memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
            index = connection->send_head;
            if (index) {
                OS_IncVptr(&window, index * sizeof(struct sendf_entry));
            }
            while (OS_Vmemcpy(&local, &window, sizeof(struct sendf_entry)),
                   entry.len <= rest && connection->send_count != 0) {
                connection->send_count--;
                rest -= entry.len;
                connection->pending_send.total_len -= entry.len;
                if (connection->send_head < SENDF_LAST_ENTRY) {
                    connection->send_head++;
                } else {
                    connection->send_head = 0;
                }
                if (connection->send_count == 0) {
                    connection->send_tail = 0;
                    connection->send_head = 0;
                    connection->external_send = 0;
                }
                API_SendfNotify(connection - tcb_table, entry.request, 0,
                                &entry.vptr);
                index = connection->send_head;
                memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
                if (index) {
                    OS_IncVptr(&window, index * sizeof(struct sendf_entry));
                }
            }
            if (rest == 0 || connection->send_count == 0) {
                return 0;
            }
            connection->pending_send.total_len -= rest;
            entry.len -= rest;
            entry.acked += rest;
            OS_Vmemcpy(&window, &local, sizeof(struct sendf_entry));
        }
    } else {
        AT_VptrArray *segment;
        int i;

        if (len != 0) {
            if (len <= connection->send_count) {
                connection->send_count -= len;
                /* @hack spelling: the step of send_head inside the test, where
                 * a statement of its own would do */
                if (connection->send_head += len,
                    connection->send_head >= win_size) {
                    connection->send_head -= win_size;
                }
            } else {
                len -= connection->send_count;
                connection->send_count = 0;
                connection->send_head = connection->send_tail;
                connection->pending_send.total_len -= len;
                segment = connection->pending_send.buffers;
                for (i = 0; i < connection->pending_send.bufnum && len > 0;
                     segment++, i++) {
                    if (len >= segment->len) {
                        len -= segment->len;
                        segment->len = 0;
                    } else {
                        segment->len -= len;
                        OS_IncVptr(&segment->vptr, len);
                        len = 0;
                    }
                }
            }
        }
        if (connection->pending_send.total_len != 0 &&
            connection->pending_send.total_len <=
                win_size - connection->send_count) {
            AT_Vptr window;

            memcpy(&window, &connection->send_window, sizeof(AT_Vptr));
            OS_IncVptr(&window, connection->send_tail);
            segment = connection->pending_send.buffers;
            for (i = 0; i < connection->pending_send.bufnum; segment++, i++) {
                if (segment->len > 0) {
                    Uint16 room;
                    Sint16 n;

                    connection->send_count += segment->len;
                    if (segment->len >
                        (room = win_size - connection->send_tail)) {
                        OS_Vmemcpy(&window, &segment->vptr, room);
                        OS_IncVptr(&segment->vptr, room);
                        segment->len -= room;
                        connection->pending_send.total_len -= room;
                        connection->send_tail = 0;
                        memcpy(&window, &connection->send_window,
                               sizeof(AT_Vptr));
                    }
                    n = segment->len;
                    OS_Vmemcpy(&window, &segment->vptr, n);
                    connection->send_tail += n;
                    OS_IncVptr(&window, n);
                    connection->pending_send.total_len -= n;
                    segment->len = 0;
                }
            }
            if (connection->pending_send.total_len > 0) {
                return 0;
            }
            connection->pending_send.total_len = 0;
            connection->pending_send.bufnum = 0;
        } else if (connection->pending_send.total_len != 0) {
            return 0;
        }
        API_SendNotify(connection - tcb_table, connection->write_request, 0);
        return 0;
    }
    return 0;
}

int TCP_PutRecvWindow(AT_MaxBuffs *bufs, Uint16 offset, struct tcb *connection)
{
    AT_Vptr dst;
    AT_Vptr src;
    Uint32 skip;
    Uint32 n;
    int i;

    /* bufs's bytes from offset on fill the read that waits. they then fill each
     * read the application posts from a read's notify. the rest go into the
     * receive window. a read's notify comes once it is full or the bytes run
     * out. -1 while receiving is disabled */
    if (!connection->receive_disabled) {
        skip = 0;
        for (i = 0; i < bufs->bufnum; i++) {
            if ((Uint16)bufs->buffers[i].len > offset) {
                skip = offset;
                break;
            }
            offset -= bufs->buffers[i].len;
        }
        if (i == bufs->bufnum) {
            return 0;
        }
        if (connection->receive_buffer.type) {
            memcpy(&dst, &connection->receive_buffer, sizeof(AT_Vptr));
            while (i < bufs->bufnum) {
            copy_chunk:
                memcpy(&src, &bufs->buffers[i].vptr, sizeof(AT_Vptr));
                if (skip) {
                    OS_IncVptr(&src, skip);
                }
                if (connection->receive_size - connection->receive_done >=
                    (Uint32)(bufs->buffers[i].len - skip)) {
                    n = bufs->buffers[i].len - skip;
                    OS_Vmemcpy(&dst, &src, n);
                    connection->receive_done += n;
                    OS_IncVptr(&dst, n);
                    skip = 0;
                    i++;
                } else {
                    n = connection->receive_size - connection->receive_done;
                    OS_Vmemcpy(&dst, &src, n);
                    connection->receive_done += n;
                    skip += n;
                }
                if (connection->receive_size ==
                    (Uint32)connection->receive_done) {
                    connection->receive_buffer.type = AT_PTR_TYPE_NONE;
                    API_RecvNotify(connection - tcb_table,
                                   connection->read_request,
                                   connection->receive_done,
                                   connection->rcv_up - connection->rcv_nxt +
                                       connection->receive_done);
                    if (!connection->receive_buffer.type) {
                        goto buffer_window;
                    }
                    memcpy(&dst, &connection->receive_buffer, sizeof(AT_Vptr));
                    /* @hack redundant: the jump to the next line */
                    goto next_chunk;
                next_chunk:
                    if (i < bufs->bufnum) {
                        goto copy_chunk;
                    }
                    switch (connection->state) {
                    case TCPS_CLOSE_WAIT:
                    case TCPS_CLOSING:
                    case TCPS_TIME_WAIT:
                    case TCPS_CLOSED:
                    case TCPS_LAST_ACK:
                        connection->receive_buffer.type = AT_PTR_TYPE_NONE;
                        API_RecvNotify(
                            connection - tcb_table, connection->read_request, 0,
                            connection->rcv_up - connection->rcv_nxt);
                        break;
                    default:
                        break;
                    }
                    return 0;
                }
            }
            connection->receive_buffer.type = AT_PTR_TYPE_NONE;
            API_RecvNotify(connection - tcb_table, connection->read_request,
                           connection->receive_done,
                           connection->rcv_up - connection->rcv_nxt +
                               connection->receive_done);
            /* @hack spelling: a loop whose body always returns, where an if
             * would do */
            while (connection->receive_buffer.type) {
                switch (connection->state) {
                case TCPS_CLOSE_WAIT:
                case TCPS_CLOSING:
                case TCPS_TIME_WAIT:
                case TCPS_CLOSED:
                case TCPS_LAST_ACK:
                    connection->receive_buffer.type = AT_PTR_TYPE_NONE;
                    API_RecvNotify(connection - tcb_table,
                                   connection->read_request, 0,
                                   connection->rcv_up - connection->rcv_nxt);
                    break;
                default:
                    break;
                }
                return 0;
            }
            return 0;
        }
    buffer_window:
        memcpy(&dst, &connection->receive_window, sizeof(AT_Vptr));
        OS_IncVptr(&dst, connection->receive_tail);
        while (i < bufs->bufnum) {
            memcpy(&src, &bufs->buffers[i].vptr, sizeof(AT_Vptr));
            if (skip) {
                OS_IncVptr(&src, skip);
            }
            n = bufs->buffers[i].len - skip;
            if (n > win_size - connection->receive_count) {
                n = win_size - connection->receive_count;
            }
            connection->receive_count += n;
            if (connection->receive_tail + n >= win_size) {
                OS_Vmemcpy(&dst, &src, win_size - connection->receive_tail);
                memcpy(&dst, &connection->receive_window, sizeof(AT_Vptr));
                OS_IncVptr(&src, win_size - connection->receive_tail);
                n -= win_size - connection->receive_tail;
                connection->receive_tail = 0;
            }
            if (n > 0) {
                OS_Vmemcpy(&dst, &src, n);
                connection->receive_tail += n;
                OS_IncVptr(&dst, n);
            }
            skip = 0;
            i++;
        }
        return 0;
    }
    return -1;
}

int TCP_FinNotify(struct tcb *connection)
{
    while (connection->receive_buffer.type) {
        connection->receive_buffer.type = AT_PTR_TYPE_NONE;
        API_RecvNotify(connection - tcb_table, connection->read_request, 0,
                       connection->rcv_up - connection->rcv_nxt);
    }
    return 0;
}

int TCP_SetRecvReq(struct tcb *connection, AT_Vptr *buffer, Uint32 size,
                   Sint32 request)
{
    AT_Vptr src;
    AT_Vptr dst;
    Uint32 n;
    Uint16 room;

    /* the read of size bytes into buffer takes what the receive window holds.
     * so does each read the application posts from a read's notify. -1 while
     * another read waits or receiving is disabled */
    if (!connection->receive_buffer.type && !connection->receive_disabled) {
        connection->read_request = request;
        memcpy(&connection->receive_buffer, buffer, sizeof(AT_Vptr));
        connection->receive_size = size;
        connection->receive_done = 0;
        if (connection->receive_count == 0) {
            switch (connection->state) {
            case TCPS_CLOSE_WAIT:
            case TCPS_CLOSING:
            case TCPS_TIME_WAIT:
            case TCPS_CLOSED:
            case TCPS_LAST_ACK:
                connection->receive_buffer.type = AT_PTR_TYPE_NONE;
                API_RecvNotify(connection - tcb_table, connection->read_request,
                               0, connection->rcv_up - connection->rcv_nxt);
                while (connection->receive_buffer.type) {
                    connection->receive_buffer.type = AT_PTR_TYPE_NONE;
                    API_RecvNotify(connection - tcb_table,
                                   connection->read_request, 0,
                                   connection->rcv_up - connection->rcv_nxt);
                }
                break;
            }
            return 0;
        }
        do {
            n = connection->receive_count > connection->receive_size
                    ? connection->receive_size
                    : connection->receive_count;
            connection->receive_done = n;
            memcpy(&src, &connection->receive_window, sizeof(AT_Vptr));
            OS_IncVptr(&src, connection->receive_head);
            memcpy(&dst, &connection->receive_buffer, sizeof(AT_Vptr));
            if (connection->receive_head + n > win_size) {
                room = win_size - connection->receive_head;
                OS_Vmemcpy(&dst, &src, room);
                OS_IncVptr(&dst, room);
                n -= room;
                connection->receive_count -= room;
                connection->receive_head = 0;
                memcpy(&src, &connection->receive_window, sizeof(AT_Vptr));
            }
            OS_Vmemcpy(&dst, &src, n);
            connection->receive_count -= n;
            connection->receive_head += n;
            if (connection->receive_head >= win_size) {
                connection->receive_head -= win_size;
            }
            connection->receive_buffer.type = AT_PTR_TYPE_NONE;
            API_RecvNotify(connection - tcb_table, connection->read_request,
                           connection->receive_done,
                           connection->rcv_up - connection->rcv_nxt +
                               connection->receive_done);
        } while (connection->receive_buffer.type != AT_PTR_TYPE_NONE &&
                 connection->receive_count > 0);
        /* the peer hears of the room the reads made */
        TCP_Rawsend(connection);
        if (connection->receive_buffer.type) {
            switch (connection->state) {
            case TCPS_CLOSE_WAIT:
            case TCPS_CLOSING:
            case TCPS_TIME_WAIT:
            case TCPS_CLOSED:
            case TCPS_LAST_ACK:
                API_RecvNotify(connection - tcb_table, connection->read_request,
                               0, connection->rcv_up - connection->rcv_nxt);
                break;
            }
        }
        return 0;
    }
    return -1;
}

Sint16 TCP_SetRecvReqEnt(struct tcb *connection, AT_Vptr *buffer, Uint32 size,
                         Sint32 request)
{
    if (connection->receive_buffer.type) {
        return -1;
    }
    connection->read_request = request;
    memcpy(&connection->receive_buffer, buffer, sizeof(AT_Vptr));
    connection->receive_size = size;
    connection->receive_done = 0;
    return 0;
}

Uint32 TCP_SendWindowSpace(struct tcb *connection)
{
    if (connection->external_send) {
        return win_size;
    } else {
        return win_size - connection->send_count;
    }
}

Uint32 TCP_RecvWindowSpace(struct tcb *connection)
{
    return win_size - connection->receive_count;
}

int TCP_SendWindowCc(struct tcb *connection)
{
    if (connection->external_send) {
        return connection->pending_send.total_len;
    }
    return connection->send_count + connection->pending_send.total_len;
}

int TCP_RecvWindowCc(struct tcb *connection)
{
    return connection->receive_count;
}

Uint32 TCP_RecvWindowSize(struct tcb *connection)
{
    return win_size;
}

int TCP_SendWindowAlloc(struct tcb *connection)
{
    if (connection->send_window.type ||
        MP_Alloc(&connection->send_window, win_size, MP_TCP_WINDOW) != 0) {
        return -1;
    }
    connection->send_count = connection->send_head = connection->send_tail = 0;
    return 0;
}

int TCP_RecvWindowAlloc(struct tcb *connection)
{
    if (connection->receive_window.type ||
        MP_Alloc(&connection->receive_window, win_size, MP_TCP_WINDOW) != 0) {
        return -1;
    }
    connection->receive_count = connection->receive_head =
        connection->receive_tail = 0;
    return 0;
}

int TCP_SendWindowFree(struct tcb *connection)
{
    /* @hack spelling: the return of 0 inside an else, where
     * TCP_RecvWindowFree's shape would do */
    if (!connection->send_window.type) {
        return -1;
    } else {
        MP_Free(&connection->send_window);
        connection->send_window.type = AT_PTR_TYPE_NONE;
        return 0;
    }
}

int TCP_RecvWindowFree(struct tcb *connection)
{
    if (!connection->receive_window.type) {
        return -1;
    }
    MP_Free(&connection->receive_window);
    connection->receive_window.type = AT_PTR_TYPE_NONE;
    return 0;
}
