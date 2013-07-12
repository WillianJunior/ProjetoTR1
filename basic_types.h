#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#define MSG_TYPE unsigned int
#define ACK_TYPE unsigned int

#define TIMEOUT 2

#define FULL_PACK_SIZE 32
#define SLAST_SIZE 1
#define CRC_SIZE 3

#define IN_MSGQ_KEY 0x1927
#define OUT_MSGQ_KEY 0x1928

#define EXTRACT_SLAST(A) (A << (FULL_PACK_SIZE - SLAST_SIZE - CRC_SIZE)) >> (FULL_PACK_SIZE - SLAST_SIZE)
#define EXTRACT_RNEXT(A) A >> CRC_SIZE
#define EXTRACT_MSG(A) A >> (CRC_SIZE + SLAST_SIZE)

#define RAND_SEED time(NULL)

#define PROB_ERROR 0.0000000000001
#define CRC_GEN_POL 0x7

enum protocols_options {  
	STOPNWAIT = 1,
	SLIDINGWINDOW = 2,
	EXIT = 9
};

struct msgbuff {
   long mtype;
   MSG_TYPE msg;
};

struct ackbuff {
   long mtype;
   ACK_TYPE ack;
};

#endif
