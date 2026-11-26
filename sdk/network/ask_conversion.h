#ifndef ASK_CONVERSION_H
#define ASK_CONVERSION_H
#include "ask_cells.h"
#include <sg_xpt.h>

void henkan(int first, int end, int mode, int length);
void save_fst_bslen(int first);
void first_kouho(int first, int length);
void init_kouho(int start, int reset);
void khmem_raw(int mode);
int kh_count(union ask_candidate *candidate);
int next_gun(int display, int wrap);
int back_gun(int display, int wrap);
int next_kouho(int display, int wrap);
int back_kouho(int display, int wrap);
#endif
