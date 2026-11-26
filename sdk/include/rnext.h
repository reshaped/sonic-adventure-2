/* the C runtime's random sequence: the state rand and srand share */
#ifndef RNEXT_H
#define RNEXT_H

/* the last value of rand's sequence, 0 until srand seeds it */
extern unsigned long _rnext;

#endif
