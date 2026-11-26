/* Maple bus driver: a frame's commands, and each port's and device's state
 * from the responses */
#include <kaede.h>
#include <machine.h>
#include <maple_driver.h>
#include <sg_xpt.h>

/* the tags that start the parts of struct mp_context, 'PORT' to 'END ', as
 * little-endian words */
#define TAG_PORT 0x54524f50
#define TAG_DEV 0x20564544
#define TAG_INFO 0x4f464e49
#define TAG_ENBL 0x4c424e45
#define TAG_STAT 0x54415453
#define TAG_RADR 0x52444152
#define TAG_TOUT 0x54554f54
#define TAG_DRQC 0x43515244
#define TAG_FUNC 0x434e5546
#define TAG_END 0x20444e45

/* a command's words ahead of its data: the header, the response address and
 * the command word */
#define COMMAND_HEAD 3
/* the bytes in a word */
#define WORD_SIZE 4

/* the bytes mpdrv_init_globals gives each frame buffer in sendbuf */
#define FRAME_SIZE 0x3000
/* the bits of the first n ports, from port A, as port_bits holds them */
#define FIRST_PORTS(n) ((1 << (n)) - 1)

/* the system bus's revision, in its low byte */
/* @hack type: the register read as a Sint32, where a Uint32 would do */
#define BUS_REVISION (*(volatile Sint32 *)0xa05f689c)
/* the last system bus revision on which the driver enables only ports A to
 * C */
#define LAST_THREE_PORT_REVISION 1
#define MAPLE_REGISTERS ((volatile Sint32 *)0xa05f6c00)
/* what mpdrv_init_globals writes to _mp->enable for a root device and for
 * an expansion device */
#define ENABLE_ROOT 'R'
#define ENABLE_EXPANSION 'E'
/* the settings mpdrv_init_globals starts the driver with */
#define START_NOPCOUNT 5
#define START_DREQCOUNT 10
#define START_ROOTPLAY 5
/* the lowest response code the driver reads as an error */
#define ERROR_RESPONSE 0xe0
/* the two response headers the driver reads as a frame left unanswered */
#define UNANSWERED 0xffffffff
#define UNANSWERED_ZERO 0xffffff00
/* the byte of a response header that holds the source address, with
 * swapmode MPDRV_SWAP_DATA and with MPDRV_SWAP_COMMAND */
#define HEAD_SOURCE 2
#define HEAD_SOURCE_SWAPPED 1

/* a word of the bytes a, b, c and d, from the most significant byte down */
#define PACK_WORD(a, b, c, d)                                                  \
    ((((a) << 24) & 0xff000000) | (((b) << 16) & 0xff0000) |                   \
     (((c) << 8) & 0xff00) | ((d) & 0xff))
/* x with its four bytes in reverse order */
#define SWAP_WORD(x)                                                           \
    ((((x) << 24) & 0xff000000) | (((x) << 8) & 0xff0000) |                    \
     (((x) >> 8) & 0xff00) | (((x) >> 24) & 0xff))
/* the words in record, as mpdrv_memset_l counts them */
#define WORDS(record) (sizeof(record) / WORD_SIZE)

/* a Maple address holds its port number in ADDRESS_PORT, from bit
 * ADDRESS_PORT_SHIFT. an expansion device's address sets its socket's bit of
 * ADDRESS_SOCKETS, and a root device's sets bit 5. a command's origin, the
 * host's address, is its port's bits alone */
#define ADDRESS_PORT 0xc0
#define ADDRESS_PORT_SHIFT 6
#define ADDRESS_SOCKETS 0x1f
/* the two bits of a port number, in an address and in a header */
#define PORT_MASK 3
/* an address's low 28 bits, which hold its physical address in main memory */
#define DMA_ADDRESS_MASK 0x0fffffff

/* a description's connector byte, after its area code, holds the device's
 * connector directions. a root device's holds connector_dir[0] in its low
 * two bits, and connector_dir[1] in the two above. an expansion device's
 * marks its connector_dir[0] with one of its low four bits */
#define ROOT_DIRECTION_MASK 3
#define ROOT_DIRECTION_SHIFT 2
#define EXPANSION_DIRECTION_MASK 0x0f
/* the bytes of the product name and of the license in a device's
 * description. product_name keeps its last NUL at NAME_LAST */
#define NAME_BYTES 30
#define LICENSE_BYTES 60
#define NAME_LAST 31
/* the characters of maple_clear_info's "(none)" */
#define NONE_LENGTH 6

/* the member exports these functions, though no header of the SDK declares
 * them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
Sint32 mpdrv_hreset(Sint32 port);
Sint32 mpdrv_sdckbenable(Sint32 port);
Sint32 mpdrv_sdckbdisable(Sint32 port);
Sint32 mpdrv_set_transagain_ns(Sint32 device);
Sint32 mpdrv_set_hreset_ns(Sint32 port);
Sint32 mpdrv_set_end(void);
Sint32 mpdrv_memset_l(void *record, Sint32 value, Sint32 count);
void *mpdrv_p2v(void *addr);
/* NOLINTEND(misc-use-internal-linkage) */

static Sint32 maple_update_root(Sint32 port, KDS_DEVICEINFO *info,
                                struct maple_device *dev, Uint32 *response);
static Sint32 maple_update_expansion(Uint8 socket_bit, KDS_PORT *state,
                                     KDS_DEVICEINFO *info,
                                     struct maple_device *dev,
                                     Uint32 *response);
static Sint32 maple_read_info(KDS_DEVICEINFO *info, struct maple_device *dev,
                              Uint32 *response);
static Sint32 maple_clear_info(KDS_DEVICEINFO *info);
static void maple_point_at_registers(void);

/* the table maps an expansion device's EXPANSION_DIRECTION_MASK bits to its
 * connector_dir[0]. a single bit set maps to that bit's number, and no bit or
 * several to 0 */
/* @hack type: the table without const, where const would do: nothing writes
 * it */
static Uint8 maple_expansion_direction[EXPANSION_DIRECTION_MASK + 1] = {
    0, 0, 1, 0, 2, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0};

void mpdrv_init_globals(void)
{
    Sint32 i;

    _mp->recv_side = 0;
    _mp->send_side = 0;
    for (i = 0; i < KDD_DEV_MAX; i++) {
        _mp->recv_data[0][i] =
            (Uint32 *)(_mp->recvbuf + (i * MPDRV_RESPONSE_SIZE));
        _mp->recv_data[1][i] = (Uint32 *)(_mp->recvbuf + ((i + KDD_DEV_MAX) *
                                                          MPDRV_RESPONSE_SIZE));
    }
    _mp->send_frame[0] = (Uint8 *)_mp->sendbuf;
    _mp->send_frame[1] = (Uint8 *)(_mp->sendbuf + FRAME_SIZE);
    _mp->last_cmd = (Uint32 *)_mp->send_frame[0];
    mpdrv_memset_l(_mp->port, 0, WORDS(_mp->port));
    mpdrv_memset_l(_mp->device, 0, WORDS(_mp->device));
    for (i = 0; i < KDD_DEV_MAX; i++) {
        maple_clear_info(&_mp->info[i]);
    }
    for (i = 0; i < KDD_DEV_MAX; i++) {
        _mp->enable[i] = 0;
    }
    /* @hack spelling: a bare scope for the port loop's locals, not declared
     * at the function's top */
    {
        /* @hack type: port_bits and ports as char, where Uint8 would do */
        char port_bits[MPDRV_PORTS] = {1, 2, 4, 8};
        char ports;
        Sint32 port;
        Sint32 socket;

        if ((BUS_REVISION & 0xff) <= LAST_THREE_PORT_REVISION) {
            ports = FIRST_PORTS(3);
        } else {
            ports = FIRST_PORTS(MPDRV_PORTS);
        }
        for (port = 0; port < MPDRV_PORTS; port++) {
            if (port_bits[port] & ports) {
                _mp->enable[port * MPDRV_PORT_DEVICES] = ENABLE_ROOT;
                for (socket = 0; socket < MPDRV_PORT_DEVICES - 1; socket++) {
                    _mp->enable[(port * MPDRV_PORT_DEVICES) + 1 + socket] =
                        ENABLE_EXPANSION;
                }
            }
        }
    }
    _mp->timeout = (Sint32)MPDRV_TIMEOUT(MPDRV_START_TIMEOUT);
    _mp->send_size = 0;
    _mp->last_cmd_words = 0;
    _mp->next_seq = 1;
    _mp->debugmode = 0;
    _mp->unk_10ec = 0;
    _mp->recv_pending = 0;
    _mp->nopcount = START_NOPCOUNT;
    _mp->speed = 0;
    _mp->dreqcount = START_DREQCOUNT;
    _mp->rootplay = START_ROOTPLAY;
    _mp->optimize = 0;
    _mp->nops = 0;
    _mp->gunmode = 0;
    _mp->active_gunmode = 0;
    _mp->autoflash = 0;
    _mp->tag_port = TAG_PORT;
    _mp->tag_dev = TAG_DEV;
    _mp->tag_info = TAG_INFO;
    _mp->tag_enbl = TAG_ENBL;
    _mp->tag_stat = TAG_STAT;
    _mp->tag_radr = TAG_RADR;
    _mp->tag_tout = TAG_TOUT;
    _mp->tag_drqc = TAG_DRQC;
    _mp->tag_func = TAG_FUNC;
    _mp->tag_end = TAG_END;
    mpdrv_swapmode(MPDRV_SWAP_DATA);
    maple_point_at_registers();
}

/* what a set call passes mpdrv_dpf when it finds the semaphore held. SHC
 * places constants in the order they are defined. the image holds this one
 * after the initializer of mpdrv_init_globals's port_bits */
static const char maple_no_semaphore[] =
    "***FATAL ERROR:Cannot get semaphore!\n";

Sint32 mpdrv_insertnop(Sint32 count)
{
    Sint32 i;
    Uint32 *word;

    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    for (i = 0; i < count; i++) {
        *word++ = MPDRV_PATTERN_NOP;
    }
    _mp->last_cmd_words = count;
    _mp->send_size += count * WORD_SIZE;
    return 0;
}

/* queue command for device as mpdrv_setcmd_force does, and make it device's
 * queued command. it returns the command's sequence number, or -1 when device
 * has a command queued already */
Sint32 mpdrv_setcmd(Sint32 device, Uint8 command, const void *data,
                    Sint32 count, const void *extra, Sint32 extra_count)
{
    Sint32 i;
    Sint32 total;
    Sint32 length;
    Sint32 device_port;
    Sint32 last_port;
    Uint32 words;
    Uint8 address;
    Uint8 origin;
    Uint32 *word;
    const Sint32 *from;
    /* @hack type: the extra words' bytes as char, where Uint8 would do */
    const char *bytes;
    struct mp_context *context;

    if (_mp->device[device].queued) {
        return -1;
    }
    _mp->device[device].queued = command;
    total = count + extra_count;
    /* @hack type: COMMAND_HEAD added as a Uint32, where an int would do */
    /* @hack spelling: words, which holds the sum until length takes it,
     * where length = total + (Uint32)COMMAND_HEAD would do */
    words = total + (Uint32)COMMAND_HEAD;
    length = words;
    address = _mp_Per2APTable[device];
    origin = address & ADDRESS_PORT;
    /* @hack redundant: the & PORT_MASK, though the shift leaves only the
     * port's bits */
    device_port = origin >> ADDRESS_PORT_SHIFT & PORT_MASK;
    /* @hack spelling: _mp through context for last_cmd_words, last_cmd and
     * the nopcount passed to mpdrv_insertnop, not through _mp at each */
    context = _mp;
    if (_mp->nopcount > 0 && context->last_cmd_words > 0) {
        /* @hack type: the header's high half cast to Sint16 ahead of the
         * mask, where (*context->last_cmd >> MPDRV_PORT_SHIFT) & PORT_MASK
         * would do */
        /* @hack spelling: last_port through a local, not compared in
         * place */
        last_port =
            (Sint16)(*context->last_cmd >> MPDRV_PORT_SHIFT) & PORT_MASK;
        if (device_port == last_port) {
            mpdrv_insertnop(context->nopcount);
        }
    }
    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    *word++ = MPDRV_HEADER_PORT(device_port) | (Uint8)total;
    *word++ = (Uint32)mpdrv_v2p(_mp->recv_data[_mp->recv_side][device]);
    from = data;
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        *word++ = PACK_WORD(command, address, origin, total);
        for (i = 0; i < count; i++) {
            *word++ = *from++;
        }
    } else {
        *word++ = PACK_WORD(total, origin, address, command);
        for (i = 0; i < count; i++) {
            /* @hack spelling: word stepped in a statement of its own, where
             * *word++ = SWAP_WORD(*from) would do */
            *word = SWAP_WORD(*from);
            word++;
            from++;
        }
    }
    /* @hack redundant: a second test of swapmode, which nothing since the
     * first changes, not merged into the first */
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        bytes = extra;
        for (i = 0; i < extra_count; i++) {
            *word++ = PACK_WORD(bytes[0], bytes[1], bytes[2], bytes[3]);
            bytes += WORD_SIZE;
        }
    } else {
        from = extra;
        for (i = 0; i < extra_count; i++) {
            *word++ = *from++;
        }
    }
    _mp->last_cmd_words = length;
    _mp->send_size += length * WORD_SIZE;
    _mp->device[device].queued_seq = _mp->next_seq++;
    _mp->next_seq &= 0x7fffffff;
    if (_mp->next_seq == 0) {
        _mp->next_seq = 1;
    }
    return _mp->device[device].queued_seq;
}

Sint32 mpdrv_setcmd_force(Sint32 device, Uint8 command, const void *data,
                          Sint32 count, const void *extra, Sint32 extra_count)
{
    Sint32 i;
    Sint32 total;
    Sint32 length;
    Sint32 device_port;
    Sint32 last_port;
    Uint32 words;
    Uint8 address;
    Uint8 origin;
    Uint32 *word;
    const Sint32 *from;
    /* @hack type: the extra words' bytes as char, where Uint8 would do */
    const char *bytes;

    total = count + extra_count;
    /* @hack type: COMMAND_HEAD added as a Uint32, where an int would do */
    /* @hack spelling: words, which holds the sum until length takes it,
     * where length = total + (Uint32)COMMAND_HEAD would do */
    words = total + (Uint32)COMMAND_HEAD;
    length = words;
    address = _mp_Per2APTable[device];
    origin = address & ADDRESS_PORT;
    /* @hack redundant: the & PORT_MASK, though the shift leaves only the
     * port's bits */
    device_port = origin >> ADDRESS_PORT_SHIFT & PORT_MASK;
    if (_mp->nopcount > 0 && _mp->last_cmd_words > 0) {
        /* @hack type: the header's high half cast to Sint16 ahead of the
         * mask, where (*_mp->last_cmd >> MPDRV_PORT_SHIFT) & PORT_MASK would
         * do */
        /* @hack spelling: last_port through a local, not compared in
         * place */
        last_port = (Sint16)(*_mp->last_cmd >> MPDRV_PORT_SHIFT) & PORT_MASK;
        if (device_port == last_port) {
            mpdrv_insertnop(_mp->nopcount);
        }
    }
    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    *word++ = MPDRV_HEADER_PORT(device_port) | (Uint8)total;
    *word++ = (Uint32)mpdrv_v2p(_mp->recv_data[_mp->recv_side][device]);
    from = data;
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        *word++ = PACK_WORD(command, address, origin, total);
        for (i = 0; i < count; i++) {
            *word++ = *from++;
        }
    } else {
        *word++ = PACK_WORD(total, origin, address, command);
        for (i = 0; i < count; i++) {
            /* @hack spelling: word stepped in a statement of its own, where
             * *word++ = SWAP_WORD(*from) would do */
            *word = SWAP_WORD(*from);
            word++;
            from++;
        }
    }
    /* @hack redundant: a second test of swapmode, which nothing since the
     * first changes, not merged into the first */
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        bytes = extra;
        for (i = 0; i < extra_count; i++) {
            *word++ = PACK_WORD(bytes[0], bytes[1], bytes[2], bytes[3]);
            bytes += WORD_SIZE;
        }
    } else {
        from = extra;
        for (i = 0; i < extra_count; i++) {
            *word++ = *from++;
        }
    }
    _mp->last_cmd_words = length;
    _mp->send_size += length * WORD_SIZE;
    return 0;
}

/* add port's reset pattern to the frame being built, and return 0 */
Sint32 mpdrv_hreset(Sint32 port)
{
    /* @hack spelling: the pattern stored through word, not through
     * _mp->last_cmd */
    Uint32 *word;

    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    *word = MPDRV_HEADER_PORT(port) | MPDRV_PATTERN_RESET;
    _mp->last_cmd_words = 1;
    _mp->send_size += WORD_SIZE;
    return 0;
}

/* add the pattern that puts port in light gun mode, and return 0 */
Sint32 mpdrv_sdckbenable(Sint32 port)
{
    /* @hack spelling: the pattern stored through word, not through
     * _mp->last_cmd */
    Uint32 *word;

    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    *word = MPDRV_HEADER_PORT(port) | MPDRV_PATTERN_GUN_ON;
    _mp->last_cmd_words = 1;
    _mp->send_size += WORD_SIZE;
    return 0;
}

/* add the pattern that takes port out of light gun mode, and return 0 */
Sint32 mpdrv_sdckbdisable(Sint32 port)
{
    /* @hack spelling: the pattern stored through word, not through
     * _mp->last_cmd */
    Uint32 *word;

    _mp->last_cmd += _mp->last_cmd_words;
    word = _mp->last_cmd;
    *word = MPDRV_HEADER_PORT(port) | MPDRV_PATTERN_GUN_OFF;
    _mp->last_cmd_words = 1;
    _mp->send_size += WORD_SIZE;
    return 0;
}

Sint32 mpdrv_check_port(Sint32 port)
{
    KDS_PORT *state = &_mp->port[port];
    /* @hack spelling: the root device's response area held in response,
     * where the header read in place would do */
    Uint32 *response;
    Uint32 head;
    Sint32 result;
    Uint8 lmon;
    Uint8 *bytes;

    response = _mp->recv_data[1 - _mp->recv_side][_mp_RootDeviceNo[port]];
    head = *response;
    switch (head) {
    case UNANSWERED:
    case UNANSWERED_ZERO:
        state->playcount++;
        /* @hack spelling: the return in an else, where an early return while
         * playcount is within rootplay would do */
        if (state->playcount > _mp->rootplay) {
            lmon = 0;
            result = -1;
        } else {
            return 0;
        }
        break;
    default:
        /* @hack spelling: a byte cursor that each branch sets, where ((Uint8
         * *)&head)[HEAD_SOURCE_SWAPPED] and [HEAD_SOURCE] would do */
        if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
            bytes = (Uint8 *)&head + HEAD_SOURCE_SWAPPED;
        } else {
            bytes = (Uint8 *)&head + HEAD_SOURCE;
        }
        lmon = *bytes;
        state->playcount = 0;
        result = 0;
        break;
    }
    state->lmold = state->lmon;
    state->lmon = lmon;
    state->lmcon = (state->lmold ^ state->lmon) & state->lmon;
    state->lmremv = ~state->lmon & (state->lmold ^ state->lmon);
    return result;
}

Sint32 mpdrv_make_devicedata(Sint32 device, Sint32 port)
{
    KDS_PORT *state = &_mp->port[port];
    struct maple_device *dev = &_mp->device[device];
    KDS_DEVICEINFO *info = &_mp->info[device];
    Uint32 *response = _mp->recv_data[1 - _mp->recv_side][device];
    Uint8 socket_bit;
    Sint32 result;

    dev->old = dev->on;
    dev->old_command = dev->command;
    if (dev->sent_seq) {
        dev->done_seq = dev->sent_seq;
        dev->sent_seq = 0;
    }
    socket_bit = _mp_Per2APTable[device] & ADDRESS_SOCKETS;
    if (socket_bit) {
        result = maple_update_expansion(socket_bit, state, info, dev, response);
    } else {
        result = maple_update_root(port, info, dev, response);
    }
    /* @hack spelling: sent compared with 0, where if (dev->sent) would do */
    if (dev->sent != 0) {
        /* @hack spelling: the response area looked up again, where response,
         * which holds it, would do */
        dev->recvdata = _mp->recv_data[1 - _mp->recv_side][device];
    }
    if (dev->recognized) {
        dev->changed = 0;
        dev->recognized = 0;
    }
    dev->changed |= dev->connect;
    dev->changed |= dev->remove;
    return result;
}

/* set dev->on from its response to the frame last received, and return 0.
 * where the frame sent dev MPDRV_CMD_DREQ, on means the response is
 * KDD_RES_DEVICESTATUS. otherwise on means any response code but
 * KDD_RES_NORESPONSE and 0 */
/* @hack redundant: response, which the function never reads */
static Sint32 maple_decide_presence(struct maple_device *dev, Uint32 *response)
{
    if (dev->sent == MPDRV_CMD_DREQ) {
        if (dev->response == KDD_RES_DEVICESTATUS) {
            dev->on = 1;
        } else {
            dev->on = 0;
        }
    } else {
        switch (dev->response) {
        case KDD_RES_NORESPONSE:
        case 0:
            dev->on = 0;
            break;
        default:
            dev->on = 1;
            break;
        }
    }
    return 0;
}

/* copy the header of dev's response into dev, and return 0. dev->errcode
 * becomes -1 for KDD_RES_NORESPONSE, and 0 for a response code below
 * ERROR_RESPONSE. for any other code, it becomes the word after the header */
static Sint32 maple_read_response(struct maple_device *dev, Uint32 *response)
{
    Uint32 head = *response++;
    /* @hack spelling: head's bytes read through a cursor, where ((Uint8
     * *)&head)[0] to [3] would do */
    Uint8 *bytes;

    /* @hack spelling: the cursor set in each branch, not once above the
     * if */
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        bytes = (Uint8 *)&head;
        dev->datasize = *bytes++;
        dev->src_ap = *bytes++;
        dev->dst_ap = *bytes++;
        dev->response = *bytes;
    } else {
        bytes = (Uint8 *)&head;
        dev->response = *bytes++;
        dev->dst_ap = *bytes++;
        dev->src_ap = *bytes++;
        dev->datasize = *bytes;
    }
    if (dev->response == KDD_RES_NORESPONSE) {
        dev->errcode = -1;
    } else if (dev->response < ERROR_RESPONSE) {
        dev->errcode = 0;
    } else {
        dev->errcode = *response;
    }
    return 0;
}

/* update a port's root device from its response, and return 0. its standing
 * command becomes MPDRV_CMD_GETCOND while it is present, and MPDRV_CMD_DREQ
 * while it is not. info is read when it connects, and cleared when it is
 * removed */
/* @hack redundant: port, which the function never reads */
static Sint32 maple_update_root(Sint32 port, KDS_DEVICEINFO *info,
                                struct maple_device *dev, Uint32 *response)
{
    maple_read_response(dev, response);
    maple_decide_presence(dev, response);
    dev->connect = (dev->old ^ dev->on) & dev->on;
    dev->remove = ~dev->on & (dev->old ^ dev->on);
    /* @hack spelling: the tests nested, where one test of all three
     * conditions would do */
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        if (dev->response != KDD_RES_NORESPONSE && dev->response != 0) {
            mpdrv_swaplong(response, (Uint8)*response + 1);
        }
    }
    if (dev->on) {
        dev->command = MPDRV_CMD_GETCOND;
    } else {
        dev->command = MPDRV_CMD_DREQ;
    }
    /* @hack redundant: an empty test of remove */
    if (dev->remove) {
    }
    if (dev->connect) {
        info->is_root = 1;
        maple_read_info(info, dev, response);
    }
    if (dev->remove) {
        maple_clear_info(info);
    }
    return 0;
}

/* update the device in a port's expansion socket from its response and the
 * port's state, and return 0. socket_bit is the socket's bit in
 * state->lmon */
static Sint32 maple_update_expansion(Uint8 socket_bit, KDS_PORT *state,
                                     KDS_DEVICEINFO *info,
                                     struct maple_device *dev, Uint32 *response)
{
    maple_read_response(dev, response);
    dev->connect = 0;
    dev->remove = 0;
    if (state->lmcon & socket_bit) {
        dev->command = MPDRV_CMD_DREQ;
        dev->on = 0;
        dev->retries++;
    }
    if (state->lmremv & socket_bit) {
        dev->command = MPDRV_CMD_DKILL;
        dev->on = 0;
        dev->remove = 1;
        dev->retries = 0;
    } else {
        /* @hack redundant: an empty else */
    }
    if (state->lmon & socket_bit) {
        if (dev->sent == MPDRV_CMD_NONE) {
            if (dev->on) {
                dev->command = MPDRV_CMD_NONE;
            }
        } else if (dev->sent == MPDRV_CMD_DREQ) {
            if (dev->response == KDD_RES_DEVICESTATUS) {
                dev->connect = 1;
                dev->on = 1;
                dev->command = MPDRV_CMD_NONE;
            } else {
                dev->retries++;
                if (dev->retries > _mp->dreqcount) {
                    dev->on = 0;
                    dev->command = MPDRV_CMD_ABSENT;
                    dev->retries = 0;
                } else {
                    dev->command = MPDRV_CMD_DREQ;
                }
            }
        } else if (dev->sent == MPDRV_CMD_DKILL) {
            dev->remove = 1;
            dev->on = 0;
            dev->command = MPDRV_CMD_ABSENT;
            dev->retries = 0;
        } else if (dev->sent) {
            dev->command = MPDRV_CMD_NONE;
        }
    } else {
        dev->command = MPDRV_CMD_ABSENT;
        dev->on = 0;
    }
    /* @hack spelling: the tests nested three deep, where one test of all
     * five conditions would do */
    if (dev->on && dev->sent) {
        if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
            if (*response != UNANSWERED && *response != UNANSWERED_ZERO) {
                mpdrv_swaplong(response, (Uint8)*response + 1);
            }
        }
    }
    if (dev->connect) {
        info->is_root = 0;
        maple_read_info(info, dev, response);
    }
    if (dev->remove) {
        info->is_root = 0;
        maple_clear_info(info);
    }
    return 0;
}

/* copy a device's description from the words after its response header into
 * info, and return 0. product_name and license end in a NUL */
/* @hack redundant: dev, which the function never reads */
static Sint32 maple_read_info(KDS_DEVICEINFO *info, struct maple_device *dev,
                              Uint32 *response)
{
    Uint8 *from;
    /* @hack spelling: zero, a local for each 0 the function stores, where a 0
     * at each store would do */
    char zero = 0;
    Uint8 *to;

    /* @hack spelling: response stepped past the header, where from = (Uint8
     * *)(response + 1) would do */
    response++;
    from = (Uint8 *)response;
    info->function = PACK_WORD(from[0], from[1], from[2], from[3]);
    from += WORD_SIZE;
    for (to = info->fd[0]; to < &info->fd[0][4]; to++) {
        *to = *from++;
    }
    for (to = info->fd[1]; to < &info->fd[1][4]; to++) {
        *to = *from++;
    }
    for (to = info->fd[2]; to < &info->fd[2][4]; to++) {
        *to = *from++;
    }
    info->area_code = *from++;
    if (info->is_root) {
        info->connector_dir[0] = *from & ROOT_DIRECTION_MASK;
        info->connector_dir[1] =
            *from >> ROOT_DIRECTION_SHIFT & ROOT_DIRECTION_MASK;
    } else {
        info->connector_dir[0] =
            maple_expansion_direction[*from & EXPANSION_DIRECTION_MASK];
        /* @hack type: the zero stored as a char, not to the Uint8 field
         * itself */
        ((char *)info->connector_dir)[1] = zero;
    }
    /* @bug logic: under MPDRV_SWAP_COMMAND, copies the connector byte into
     * product_name[0], with the name after it. the connector's reads leave
     * from on that byte (0x8c15c8d6, 0x8c15c904). this path copies from there
     * (MOV.B @R5+,R3 at 0x8c15c92a), and the other steps past it (ADD #1,R5
     * at 0x8c15c93e). no shipped code sets swapmode to MPDRV_SWAP_COMMAND:
     * only pdSetMode can, through kdSetDriverMode, and no word in any image
     * holds its address, 0x8c10f180 */
    if (_mp->swapmode == MPDRV_SWAP_COMMAND) {
        for (to = (Uint8 *)info->product_name;
             to < (Uint8 *)&info->product_name[NAME_BYTES + 1]; to++) {
            *to = *from++;
        }
    } else {
        from++;
        for (to = (Uint8 *)info->product_name;
             to < (Uint8 *)&info->product_name[NAME_BYTES]; to++) {
            *to = *from++;
        }
        info->product_name[NAME_BYTES] = zero;
    }
    info->product_name[NAME_LAST] = zero;
    for (to = (Uint8 *)info->license;
         to < (Uint8 *)&info->license[LICENSE_BYTES]; to++) {
        *to = *from++;
    }
    info->license[LICENSE_BYTES] = zero;
    info->stdby_pow = from[0] | from[1] << 8;
    info->max_pow = from[2] | from[3] << 8;
    return 0;
}

/* describe no device in info, and return 0. info is zeros, but for a
 * product_name of "(none)" padded with spaces */
static Sint32 maple_clear_info(KDS_DEVICEINFO *info)
{
    Sint32 i;

    mpdrv_memset_l(info, 0, WORDS(*info));
    info->product_name[0] = '(';
    info->product_name[1] = 'n';
    info->product_name[2] = 'o';
    info->product_name[3] = 'n';
    info->product_name[4] = 'e';
    info->product_name[5] = ')';
    for (i = NONE_LENGTH; i < NAME_LAST; i++) {
        info->product_name[i] = ' ';
    }
    info->product_name[NAME_LAST] = 0;
    return 0;
}

Sint32 mpdrv_set_getcond(Sint32 device, Sint32 func)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_getcond_ns(device, func);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_getcond_ns(Sint32 device, Sint32 func)
{
    /* @hack spelling: func through a copy, where &func would do */
    Sint32 function_word = func;

    return mpdrv_setcmd(device, MPDRV_CMD_GETCOND, &function_word, 1, NULL, 0);
}

Sint32 mpdrv_set_dreq(Sint32 device)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_dreq_ns(device);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_dreq_ns(Sint32 device)
{
    return mpdrv_setcmd(device, MPDRV_CMD_DREQ, NULL, 0, NULL, 0);
}

Sint32 mpdrv_set_dkill(Sint32 device)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_dkill_ns(device);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_dkill_ns(Sint32 device)
{
    return mpdrv_setcmd(device, MPDRV_CMD_DKILL, NULL, 0, NULL, 0);
}

Sint32 mpdrv_set_dreset(Sint32 device)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_dreset_ns(device);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_dreset_ns(Sint32 device)
{
    return mpdrv_setcmd(device, MPDRV_CMD_DRESET, NULL, 0, NULL, 0);
}

Sint32 mpdrv_set_allstatreq(Sint32 device)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_allstatreq_ns(device);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_allstatreq_ns(Sint32 device)
{
    return mpdrv_setcmd(device, MPDRV_CMD_ALLSTATREQ, NULL, 0, NULL, 0);
}

Sint32 mpdrv_set_getminfo(Sint32 device, Sint32 func, Sint32 pt)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_getminfo_ns(device, func, pt);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_getminfo_ns(Sint32 device, Sint32 func, Sint32 pt)
{
    Sint32 words[2];

    words[0] = func;
    words[1] = pt << 24;
    return mpdrv_setcmd(device, MPDRV_CMD_GETMINFO, words, 2, NULL, 0);
}

Sint32 mpdrv_set_bread(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                       Sint32 block)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_bread_ns(device, func, pt, phase, block);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_bread_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                          Sint32 block)
{
    Sint32 words[2];

    words[0] = func;
    words[1] = pt << 24 | phase << 16 | block;
    return mpdrv_setcmd(device, MPDRV_CMD_BREAD, words, 2, NULL, 0);
}

Sint32 mpdrv_set_bwrite(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                        Sint32 block, const void *src, Sint32 size)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_bwrite_ns(device, func, pt, phase, block, src, size);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_bwrite_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                           Sint32 block, const void *src, Sint32 size)
{
    Sint32 words[2];

    words[0] = func;
    words[1] = pt << 24 | phase << 16 | block;
    return mpdrv_setcmd(device, MPDRV_CMD_BWRITE, words, 2, src, size);
}

Sint32 mpdrv_set_getlerr(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                         Sint32 block)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_getlerr_ns(device, func, pt, phase, block);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_getlerr_ns(Sint32 device, Sint32 func, Sint32 pt, Sint32 phase,
                            Sint32 block)
{
    Sint32 words[2];

    words[0] = func;
    words[1] = pt << 24 | phase << 16 | block;
    return mpdrv_setcmd(device, MPDRV_CMD_GETLERR, words, 2, NULL, 0);
}

Sint32 mpdrv_set_transagain(Sint32 device)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_transagain_ns(device);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_transagain_ns(Sint32 device)
{
    return mpdrv_setcmd(device, MPDRV_CMD_TRANSAGAIN, NULL, 0, NULL, 0);
}

Sint32 mpdrv_set_arctrl(Sint32 device, const void *buf)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_arctrl_ns(device, buf);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_arctrl_ns(Sint32 device, const void *buf)
{
    Sint32 func = KDD_FUNC_ARGUN;

    return mpdrv_setcmd(device, MPDRV_CMD_ARCTRL, &func, 1, buf, 1);
}

Sint32 mpdrv_set_ft4ctrl(Sint32 device, const void *buf)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_ft4ctrl_ns(device, buf);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_ft4ctrl_ns(Sint32 device, const void *buf)
{
    Sint32 func = KDD_FUNC_SOUNDINPUT;

    return mpdrv_setcmd(device, MPDRV_CMD_FT4CTRL, &func, 1, buf, 1);
}

Sint32 mpdrv_set_setcond(Sint32 device, Sint32 func, const void *buf,
                         Sint32 size)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_setcond_ns(device, func, buf, size);
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_setcond_ns(Sint32 device, Sint32 func, const void *buf,
                            Sint32 size)
{
    /* @hack spelling: func through a copy, where &func would do */
    Sint32 function_word = func;

    return mpdrv_setcmd(device, MPDRV_CMD_SETCOND, &function_word, 1, buf,
                        size);
}

Sint32 mpdrv_set_hreset(Sint32 port)
{
    return mpdrv_set_hreset_ns(port);
}

Sint32 mpdrv_set_hreset_ns(Sint32 port)
{
    return mpdrv_hreset(port);
}

Sint32 mpdrv_set_end(void)
{
    Sint32 result;

    if (!tas(_mp_SemaphoreAddress)) {
        mpdrv_dpf(maple_no_semaphore);
        return MPDRV_SEMAPHORE_TAKEN;
    }
    result = mpdrv_set_end_ns();
    *_mp_SemaphoreAddress = 0;
    return result;
}

Sint32 mpdrv_set_end_ns(void)
{
    /* @hack spelling: the command's address through word, not through
     * _mp->last_cmd */
    Uint32 *word = _mp->last_cmd;

    *word |= MPDRV_LAST_HEADER;
    return 0;
}

Sint32 mpdrv_memset_l(void *record, Sint32 value, Sint32 count)
{
    Sint32 i;
    Sint32 *p;

    /* @hack spelling: p, a cursor set in the for's first clause, where
     * ((Sint32 *)record)[i] would do */
    for (i = 0, p = record; i < count; i++) {
        *p++ = value;
    }
    return count;
}

void *mpdrv_v2p(void *addr)
{
    return (void *)((Uint32)addr & DMA_ADDRESS_MASK);
}

void *mpdrv_p2v(void *addr)
{
    return addr;
}

static void maple_point_at_registers(void)
{
    _mp->reg = MAPLE_REGISTERS;
}
