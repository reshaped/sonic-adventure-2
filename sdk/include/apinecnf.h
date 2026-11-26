/* the network SDK's Japanese input over ASK. comkan_henkan converts a
 * reading, and comkan_nextkouho and comkan_prevkouho step round its
 * candidates. each writes the candidate and the reading's unconverted rest to
 * output, which holds capacity bytes. each returns -1 where it fails */
#ifndef APINECNF_H
#define APINECNF_H
extern char *ntgAskBuild;
int comkan_init(void);
int comkan_henkan(unsigned char *output, int capacity, unsigned char *input,
                  int input_length, int backward);
int comkan_nextkouho(unsigned char *output, int capacity);
int comkan_prevkouho(unsigned char *output, int capacity);
#endif
