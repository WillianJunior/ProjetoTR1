#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#define MSG_TYPE unsigned int
#define ACK_TYPE unsigned int
#define ID_TYPE unsigned int

#define TIMEOUT 6

#define FULL_PACK_SIZE 32
#define PACK_ID_SIZE 4
#define ACK_TYPE_SIZE 1
//#define WINDOW_SIZE pow(2,PACK_ID_SIZE)
#define WINDOW_SIZE 5

#define PROB_ERROR 0.03
#define CRC_GEN_POL 15
#define CRC_SIZE 3

#define RAND_SEED time(NULL)
#define MAX_DELAY 4

#define IN_MSGQ_KEY 0x1927
#define OUT_MSGQ_KEY 0x1928

#define MANUAL_ERROR
//#define AUTO_DELAY

enum protocols_options {
	STOPNWAIT = 1,
	GOBACKN = 2,
	SELECTIVEREPEAT = 3,
	EXIT = 9
};

enum ack_types {
	ACK,
	NACK
};

// msgbuff package format:
// payload | identifier | crc
struct msgbuff {
   long mtype;
   MSG_TYPE msg;
};

// ackbuff package format:
// ack type | identifier | crc
struct ackbuff {
   long mtype;
   ACK_TYPE ack;
};

struct msg_window {
   ID_TYPE identifier;
   ack_types ack_type;
   MSG_TYPE payload; // deprecated
};

struct receiver_msgbuff {
   ID_TYPE msg_num;
   ack_types rcv_flag;
   MSG_TYPE payload;
};

#endif
