/* the fixed areas of RAM the event module loads its files into and plays
 * them from */
#ifndef EVENT_AREAS_H
#define EVENT_AREAS_H
#include <ninja.h>
#include <sg_xpt.h>

/* the area of an event's tracks, which the loader zeroes before it reads
 * them. it ends where the script's work begins */
#define EVENT_TRACK_AREA ((Uint32 *)0x8c5a0000)
#define EVENT_TRACK_BYTES 0x5fc00
#define EVENT_SCRIPT_AREA ((struct event_script_work *)0x8c5ffc00)

/* 4 MB the loader reads three files into in turn. the voice language's
 * tracks come first, where they differ from the text's, then the textures'
 * archive. the scene file comes last, and stays. a movie uses the same RAM
 * as its work */
#define EVENT_SCENE_AREA ((Uint8 *)0x8c600000)
#define EVENT_SCENE_BYTES 0x400000

/* the area the loader reads e<n>texlist.prs into. the file's first word
 * points at the NJS_TEXLIST it holds */
#ifdef NONMATCHING
/* the area by its address: this side's objects don't define event_texlist_area,
 * which only symbols.txt places */
#define EVENT_TEXLIST_AREA ((NJS_TEXLIST **)0x8cbc0000)
#else
/* the area under a symbol, which symbols.txt places at its address. SHC's
 * code takes the address through a relocation, not as a constant */
extern NJS_TEXLIST *event_texlist_area;
#define EVENT_TEXLIST_AREA (&event_texlist_area)
#endif
#define EVENT_MESSAGE_AREA ((Uint8 *)0x8cbd0000)
#ifdef NONMATCHING
/* The texture list ends at the message bank. The full event messages have
 * the same 48 KiB reservation as the mini events, up to SCRIPT_AREA. */
#define EVENT_TEXLIST_BYTES 0x10000
#define EVENT_MESSAGE_BYTES 0xc000
#endif

#endif
