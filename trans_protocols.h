#ifndef TRANS_PROTOCOLS_H
#define TRANS_PROTOCOLS_H

#include <iostream>
#include <stdio.h>
#include <math.h>

#include <errno.h> 
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "basic_types.h"
#include "util.h"

using namespace std;

class TransProt {
public:
	TransProt ();
	~TransProt ();
	virtual int sendMsgStream (MSG_TYPE *stream, int size) = 0;
	virtual int recvMsgStream (MSG_TYPE *stream, int size) = 0;
protected:
	virtual int sendMsg (MSG_TYPE msg, ACK_TYPE *slast) = 0;
	virtual int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) = 0;
	int inputChannelId;
	int outputChannelId;
};

class StopNWait: public TransProt {
public:
	StopNWait () : TransProt() {timeout = 0;};
	~StopNWait () {};
	int sendMsgStream (MSG_TYPE *stream, int size);
	int recvMsgStream (MSG_TYPE *stream, int size);
private:
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
	int timeout;
};

class SlidingWindow : public TransProt {
public:
	SlidingWindow () : TransProt() {};
	~SlidingWindow () {};
	virtual int sendMsgStream (MSG_TYPE *stream, int size) = 0;
	virtual int recvMsgStream (MSG_TYPE *stream, int size) = 0;
protected:
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
};

class GoBackN : public TransProt {
public:
	GoBackN () : TransProt() {};
	~GoBackN () {};
	int sendMsgStream (MSG_TYPE *stream, int size);
	int recvMsgStream (MSG_TYPE *stream, int size);
protected:
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);	// only send the message with the slast and crc, with possible error
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);	// receive the messages, check and send the ack
private:
	ackbuff acknowledge ();		// return the last ack or nack from the buffer

};

#endif
