/* the program's start and end records, as sg_ov.h's OVS_MDL_OBJ lays them
 * out. _START_SYS's 32 bytes of code come first, and its literals fill the
 * start record's first 16 bytes. so this source gives only that record's ID,
 * at 0x30. ovMdlEntryTbl and ovMdlDeleteTbl check it, and find the end record
 * by its own ID */
#include <sg_ov.h>
#include <sj.h>

#ifdef NONMATCHING
/* this side places the record in DSGLH, the section the #pragma gives it, and
 * keeps it: nothing refers to it by name */
__attribute__((used, section("DSGLH")))
#else
#pragma section LH
#endif
static UUID module_start_id = {
    0x2062694c, 0x6148, 0x646e, {'l', 'e', ' ', 'S', 't', 'a', 'r', 't'}};

#ifdef NONMATCHING
/* this side places the record in DSGLE, the section the #pragma gives it, and
 * keeps it: nothing refers to it by name */
__attribute__((used, section("DSGLE")))
#else
#pragma section LE
#endif
static OVS_LIB_OBJ module_end = {
    NULL,
    0,
    0,
    0,
    0,
    0,
    {0x2062694c, 0x6148, 0x646e, {'l', 'e', ' ', 'E', 'n', 'd', ' ', ' '}}};
