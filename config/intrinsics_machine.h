/* machine.h's intrinsics, for intrinsics.h. the pragma below has clang-tidy
 * count each as machine.h's, the header a source that calls one includes */
#ifndef CONFIG_INTRINSICS_MACHINE_H
#define CONFIG_INTRINSICS_MACHINE_H
/* IWYU pragma: private, include <machine.h> */

void _builtin_set_imask(int mask);
int _builtin_get_imask(void);
void *_builtin_get_vbr(void);
void _builtin_set_fpscr(int cr);
int _builtin_get_fpscr(void);
void _builtin_fsca(long angle, float *sinv, float *cosv);
float _builtin_fsrra(float x);
void _builtin_ld_ext(float (*mx)[4]);
void _builtin_st_ext(float (*mx)[4]);
void _builtin_ftrv(float *vec1, float *vec2);
int _builtin_tas(char *addr);
void _builtin_prefetch(void *p);
void _builtin_trapa(int vector);
char *_builtin_strcpy(char *s1, const char *s2);
int _builtin_strcmp(const char *s1, const char *s2);

#endif
