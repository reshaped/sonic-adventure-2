/* the drives' commands. a call queues a command in its drive's entry of
 * buVmsCmd. the drive's fiber runs it, and passes its result to the
 * completed callback */
#include <sg_bup.h>
#include <sg_xpt.h>
#include <string.h>

#include "vms.h"

#define VMS_STACK_SIZE 1024

Sint32 buLastError[VMS_DRIVES];
VMS_COMMAND buVmsCmd[VMS_DRIVES];

/* the member exports these functions and data, though no header of the SDK
 * declares them */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* the stacks of the drives' fibers. vmsfs_c_init fills each with its
 * drive's digit, '0' to '7' */
#ifdef NONMATCHING
/* this side's stacks are arrays of Uint32, aligned to a word as
 * buFbrCreateFiber requires */
typedef Uint32 VMS_DRIVE_STACK[VMS_STACK_SIZE / sizeof(Uint32)];
VMS_DRIVE_STACK vmsfs_stack0;
VMS_DRIVE_STACK vmsfs_stack1;
VMS_DRIVE_STACK vmsfs_stack2;
VMS_DRIVE_STACK vmsfs_stack3;
VMS_DRIVE_STACK vmsfs_stack4;
VMS_DRIVE_STACK vmsfs_stack5;
VMS_DRIVE_STACK vmsfs_stack6;
VMS_DRIVE_STACK vmsfs_stack7;
#else
char vmsfs_stack0[VMS_STACK_SIZE];
char vmsfs_stack1[VMS_STACK_SIZE];
char vmsfs_stack2[VMS_STACK_SIZE];
char vmsfs_stack3[VMS_STACK_SIZE];
char vmsfs_stack4[VMS_STACK_SIZE];
char vmsfs_stack5[VMS_STACK_SIZE];
char vmsfs_stack6[VMS_STACK_SIZE];
char vmsfs_stack7[VMS_STACK_SIZE];
#endif
void vmsfs_c_fiber0(void);
void vmsfs_c_fiber1(void);
void vmsfs_c_fiber2(void);
void vmsfs_c_fiber3(void);
void vmsfs_c_fiber4(void);
void vmsfs_c_fiber5(void);
void vmsfs_c_fiber6(void);
void vmsfs_c_fiber7(void);
void vmsfs_c_dispatch(Sint32 drive);
/* NOLINTEND(misc-use-internal-linkage) */

Sint32 vmsfs_c_init(Sint32 drives)
{
#ifdef NONMATCHING
    /* each drive's fiber and stack, for this side's loop below */
    static const struct {
        void (*entry)(void);
        Uint32 *stack;
    } drive_fibers[VMS_DRIVES] = {
        {vmsfs_c_fiber0, vmsfs_stack0}, {vmsfs_c_fiber1, vmsfs_stack1},
        {vmsfs_c_fiber2, vmsfs_stack2}, {vmsfs_c_fiber3, vmsfs_stack3},
        {vmsfs_c_fiber4, vmsfs_stack4}, {vmsfs_c_fiber5, vmsfs_stack5},
        {vmsfs_c_fiber6, vmsfs_stack6}, {vmsfs_c_fiber7, vmsfs_stack7},
    };
    Sint32 drive;
#endif
    /* @hack spelling: a store of 0 for each drive's last result, where one
     * memset would do */
    buLastError[0] = 0;
    buLastError[1] = 0;
    buLastError[2] = 0;
    buLastError[3] = 0;
    buLastError[4] = 0;
    buLastError[5] = 0;
    buLastError[6] = 0;
    buLastError[7] = 0;
    memset(buVmsCmd, 0, sizeof(buVmsCmd));
    memset(vmsfs_stack0, '0', sizeof(vmsfs_stack0));
    memset(vmsfs_stack1, '1', sizeof(vmsfs_stack1));
    memset(vmsfs_stack2, '2', sizeof(vmsfs_stack2));
    memset(vmsfs_stack3, '3', sizeof(vmsfs_stack3));
    memset(vmsfs_stack4, '4', sizeof(vmsfs_stack4));
    memset(vmsfs_stack5, '5', sizeof(vmsfs_stack5));
    memset(vmsfs_stack6, '6', sizeof(vmsfs_stack6));
    memset(vmsfs_stack7, '7', sizeof(vmsfs_stack7));
    if (vmsfb_init() == NULL) {
        return -1;
    }
#ifdef NONMATCHING
    /* this side creates the fibers in one loop, and fails where one can't be
     * created, once vmsfb_exit has taken the server down. the matching side
     * ignores vmsfb_create's result */
    for (drive = 0; drive < VMS_DRIVES; drive++) {
        if ((drives & (1 << drive)) &&
            vmsfb_create(drive_fibers[drive].entry, drive_fibers[drive].stack,
                         VMS_STACK_SIZE) < 0) {
            vmsfb_exit();
            return -1;
        }
    }
#else
    if (drives & (1 << 0)) {
        vmsfb_create(vmsfs_c_fiber0, vmsfs_stack0, sizeof(vmsfs_stack0));
    }
    if (drives & (1 << 1)) {
        vmsfb_create(vmsfs_c_fiber1, vmsfs_stack1, sizeof(vmsfs_stack1));
    }
    if (drives & (1 << 2)) {
        vmsfb_create(vmsfs_c_fiber2, vmsfs_stack2, sizeof(vmsfs_stack2));
    }
    if (drives & (1 << 3)) {
        vmsfb_create(vmsfs_c_fiber3, vmsfs_stack3, sizeof(vmsfs_stack3));
    }
    if (drives & (1 << 4)) {
        vmsfb_create(vmsfs_c_fiber4, vmsfs_stack4, sizeof(vmsfs_stack4));
    }
    if (drives & (1 << 5)) {
        vmsfb_create(vmsfs_c_fiber5, vmsfs_stack5, sizeof(vmsfs_stack5));
    }
    if (drives & (1 << 6)) {
        vmsfb_create(vmsfs_c_fiber6, vmsfs_stack6, sizeof(vmsfs_stack6));
    }
    if (drives & (1 << 7)) {
        vmsfb_create(vmsfs_c_fiber7, vmsfs_stack7, sizeof(vmsfs_stack7));
    }
#endif
    return 0;
}

Sint32 vmsfs_c_exit(void)
{
    memset(buVmsCmd, 0, sizeof(buVmsCmd));
    return 0;
}

void vmsfs_c_synch(void)
{
    vmsfb_exec();
}

void vmsfs_c_fiber0(void)
{
    vmsfs_c_dispatch(0);
}

void vmsfs_c_fiber1(void)
{
    vmsfs_c_dispatch(1);
}

void vmsfs_c_fiber2(void)
{
    vmsfs_c_dispatch(2);
}

void vmsfs_c_fiber3(void)
{
    vmsfs_c_dispatch(3);
}

void vmsfs_c_fiber4(void)
{
    vmsfs_c_dispatch(4);
}

void vmsfs_c_fiber5(void)
{
    vmsfs_c_dispatch(5);
}

void vmsfs_c_fiber6(void)
{
    vmsfs_c_dispatch(6);
}

void vmsfs_c_fiber7(void)
{
    vmsfs_c_dispatch(7);
}

/* the body of drive's fiber, which never returns. it runs the drive's queued
 * command, or sleeps till the server's next run while none is queued. a
 * command whose result is BUD_ERR_NO_DISK unmounts its drive. the fiber clears
 * the command before it calls the completed callback, which can then queue the
 * next */
void vmsfs_c_dispatch(Sint32 drive)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    Sint32 unused;
    Sint32 result;
    Sint32 operation;
    Uint32 callback_argument;
    VMS_COMMAND *command;

    while (1) {
        command = &buVmsCmd[drive];
        /* @hack redundant: a store of result that each command's case
         * replaces */
        result = 0;
        switch (command->operation) {
        case BUD_OP_CONNECT:
            command->drive = command->argument[0];
            result = vmsfs_connect(command->argument[0]);
            break;
        case BUD_OP_MOUNT:
            command->drive = command->argument[0];
            result = vmsfs_mount(command->argument[0]);
            break;
        case BUD_OP_FORMATDISK:
            command->drive = command->argument[0];
            result = vmsfs_format(command->argument[0], command->label,
                                  command->argument[2], &command->time,
                                  command->argument[4]);
            break;
        case BUD_OP_CLOSEFILE:
            command->drive = ((BU_FILE *)command->argument[0])->drive;
            result = vmsfs_fclose((BU_FILE *)command->argument[0],
                                  command->argument[1]);
            break;
        case BUD_OP_READFILE:
            command->drive = ((BU_FILE *)command->argument[0])->drive;
            result =
                vmsfs_fread((BU_FILE *)command->argument[0],
                            (void *)command->argument[1], command->argument[2]);
            break;
        case BUD_OP_WRITEFILE:
            command->drive = ((BU_FILE *)command->argument[0])->drive;
            result = vmsfs_fwrite((BU_FILE *)command->argument[0],
                                  (const void *)command->argument[1],
                                  command->argument[2]);
            break;
        case BUD_OP_DELETEFILE:
            command->drive = command->argument[0];
            result = vmsfs_fdelete(command->argument[0], command->name);
            break;
        case BUD_OP_RENAMEFILE:
            command->drive = command->argument[0];
            result = vmsfs_frename(command->argument[0], command->name,
                                   command->new_name);
            break;
        case BUD_OP_SETFILEATTR:
            command->drive = command->argument[0];
            result = vmsfs_fsetattr(command->argument[0], command->name,
                                    command->argument[2], command->argument[3]);
            break;
        case BUD_OP_DEFRAGDISK:
            command->drive = command->argument[0];
            result = vmsfs_xdefrag(command->argument[0],
                                   (void *)command->argument[1]);
            break;
        case BUD_OP_READIMAGE:
            command->drive = command->argument[0];
            result = vmsfs_read_disk_image(command->argument[0],
                                           (void *)command->argument[1]);
            break;
        case BUD_OP_WRITEIMAGE:
            command->drive = command->argument[0];
            result = vmsfs_write_disk_image(command->argument[0],
                                            (const void *)command->argument[1]);
            break;
        case BUD_OP_LOADFILE:
            command->drive = command->argument[0];
            result = vmsfs_load_file(command->argument[0], command->name,
                                     (void *)command->argument[2],
                                     command->argument[3]);
            break;
        case BUD_OP_SAVEFILE:
            command->drive = command->argument[0];
            result = vmsfs_save_file(command->argument[0], command->name,
                                     (const void *)command->argument[2],
                                     command->argument[3], &command->time,
                                     command->argument[5]);
            break;
        case BUD_OP_SAVEEXECFILE:
            command->drive = command->argument[0];
            result = vmsfs_save_exec_file(command->argument[0], command->name,
                                          (const void *)command->argument[2],
                                          command->argument[3], &command->time,
                                          command->argument[5]);
            break;
        case BUD_OP_LOADFILEEX:
            command->drive = command->argument[0];
            result =
                vmsfs_load_file_ex(command->argument[0], command->name,
                                   (void *)command->argument[2],
                                   command->argument[3], command->argument[4]);
            break;
        case BUD_OP_DELETEFILEEX:
            command->drive = command->argument[0];
            result = vmsfs_fdelete_ex(command->argument[0],
                                      (const char **)command->argument[1],
                                      command->argument[2]);
            break;
        case BUD_OP_REWRITEEXECFILE:
            command->drive = command->argument[0];
            result = vmsfs_rewrite_exec_file(
                command->argument[0], command->name,
                (const void *)command->argument[2], command->argument[3],
                command->argument[4]);
            break;
        case BUD_OP_REWRITEFILE:
            command->drive = command->argument[0];
            result =
                vmsfs_rewrite_file(command->argument[0], command->name,
                                   (const void *)command->argument[2],
                                   command->argument[3], command->argument[4]);
            break;
        case BUD_OP_CHANGEBANK:
            command->drive = command->argument[0];
            result =
                vmsfs_changebank(command->argument[0], command->argument[1]);
            break;
        case BUD_OP_LOCKBANK:
            command->drive = command->argument[0];
            result = vmsfs_lockbank(command->argument[0], command->argument[1]);
            break;
        case BUD_OP_GETBANKINFO:
            command->drive = command->argument[0];
            result = vmsfs_getbankinfo(
                command->argument[0], (Sint32 *)command->argument[1],
                (Sint32 *)command->argument[2], (Sint32 *)command->argument[3]);
            break;
        case BUD_OP_NOP:
        default:
            vmsfb_sleep();
            continue;
        }
        if (result == BUD_ERR_NO_DISK) {
            vmsfs_unmount(command->drive);
        }
        buLastError[drive] = result;
        operation = command->operation;
        callback_argument = command->callback_argument;
        command->unk_00 = 0;
        command->state = 0;
        command->operation = BUD_OP_NOP;
        command->unk_0c = 0;
        command->drive = 0;
        command->argument[0] = 0;
        command->argument[1] = 0;
        command->argument[2] = 0;
        command->argument[3] = 0;
        command->argument[4] = 0;
        command->argument[5] = 0;
        if (command->completed != NULL) {
            command->completed(drive, operation, result, callback_argument);
        }
    }
}

void vmsfs_c_execcallback(Sint32 drive, Sint32 operation, Sint32 result)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    if (command->completed != NULL) {
        command->completed(drive, operation, result,
                           command->callback_argument);
    }
}

void vmsfs_c_progress(Sint32 drive, Sint32 operation, Sint32 done, Sint32 total)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    if (command->progress != NULL) {
        command->progress(drive, operation, done, total);
    }
}

void vmsfs_c_setcompletecallback(BU_COMPLETECALLBACK callback)
{
    Sint32 i;

    for (i = 0; i < VMS_DRIVES; i++) {
        buVmsCmd[i].completed = callback;
    }
}

void vmsfs_c_setprogresscallback(BU_PROGRESSCALLBACK callback)
{
    Sint32 i;

    for (i = 0; i < VMS_DRIVES; i++) {
        buVmsCmd[i].progress = callback;
    }
}

void vmsfs_c_setcallbackparam(Uint32 argument)
{
    Sint32 i;

    for (i = 0; i < VMS_DRIVES; i++) {
        buVmsCmd[i].callback_argument = argument;
    }
}

Sint32 vmsfs_c_getstate(Sint32 drive)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    return command->state;
}

void vmsfs_c_setfunc(Sint32 drive, Sint32 operation)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    command->operation = operation;
}

void vmsfs_c_setparam(Sint32 drive, Sint32 param0, Sint32 param1, Sint32 param2,
                      Sint32 param3, Sint32 param4, Sint32 param5)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    command->argument[0] = param0;
    command->argument[1] = param1;
    command->argument[2] = param2;
    command->argument[3] = param3;
    command->argument[4] = param4;
    command->argument[5] = param5;
    command->state = 1;
}

void vmsfs_c_setvolume(Sint32 drive, const Uint8 *label)
{
    VMS_COMMAND *command = &buVmsCmd[drive];
    Sint32 i;

    for (i = 0; i < VMS_LABEL_SIZE; i++) {
        command->label[i] = label[i];
    }
}

void vmsfs_c_setfname(Sint32 drive, const char *name, const char *new_name)
{
    VMS_COMMAND *command = &buVmsCmd[drive];
    Sint32 i;

    for (i = 0; i < VMS_NAME_SIZE + 1; i++) {
        command->name[i] = 0;
    }
    for (i = 0; i < VMS_NAME_SIZE + 1; i++) {
        command->new_name[i] = 0;
    }
    for (i = 0; i < VMS_NAME_SIZE; i++) {
        if (name[i] == 0) {
            break;
        }
        command->name[i] = name[i];
    }
    for (i = 0; i < VMS_NAME_SIZE; i++) {
        if (new_name[i] == 0) {
            break;
        }
        command->new_name[i] = new_name[i];
    }
}

void vmsfs_c_settime(Sint32 drive, const BUS_TIME *time)
{
    VMS_COMMAND *command = &buVmsCmd[drive];

    memcpy(&command->time, time, sizeof(BUS_TIME));
}
