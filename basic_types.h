#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <math.h>

#define MSG_TYPE int
#define ACK_TYPE int

#define TIMEOUT 2

#define ACK_PACK_SIZE 5
#define CRC_SIZE 4

#define IN_MSGQ_KEY 0x1927
#define OUT_MSGQ_KEY 0x1928

#define EXTRACT_SLAST(ACK) ACK*pow(10, CRC_SIZE)

enum protocols_options {  
	STOPNWAIT = 1,
	SLIDINGWINDOW = 2,
	EXIT = 9
};

#endif