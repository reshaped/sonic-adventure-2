#ifndef MPV_SECTION_H
#define MPV_SECTION_H

/* MPV_CODE gives GCC a function's section, which SHC takes from the #pragma
 * section above the function */
#ifdef NONMATCHING
/* the attribute keeps the section with each function through GCC's link-time
 * compile */
#define MPV_CODE(section_name) __attribute__((section(#section_name)))
#else
#define MPV_CODE(section_name)
#endif

#endif
