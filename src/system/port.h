/* what the port's GCC needs of a declaration that SHC reads as it stands */
#ifndef SYSTEM_PORT_H
#define SYSTEM_PORT_H

/* what a datum's declaration opens with, where code reads the datum at an
 * index the code computes. for SHC it is empty: SHC keeps every datum it
 * compiles in storage */
#ifdef NONMATCHING
/* GCC may find a datum that no code writes read-only, fold it into the code
 * that reads it and drop it. so its build marks the datum used, and the
 * datum stays in storage, where the original's code reads it */
#define KEPT __attribute__((used))
#else
#define KEPT
#endif

#endif
