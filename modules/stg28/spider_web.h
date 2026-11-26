/* the spider web, SPIDERWEB in Pyramid Cave's object list: a web, turned and
 * scaled as its work says */
#ifndef STG28_SPIDER_WEB_H
#define STG28_SPIDER_WEB_H
#include <ninja.h>

struct task;

/* the web's texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *cave_spider_web_texlists[];

/* the object list's step for SPIDERWEB. task needs a work. its pos places the
 * web, and its ang turns it by njRotateY, njRotateX and njRotateZ. scl.x plus
 * 1 scales it. the web has no collision record */
void cave_spider_web_init(struct task *task);

#endif
