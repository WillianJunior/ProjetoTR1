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
	TransProt() {};
	TransProt (int timeout, int slast_size, int rnext_size, float prob_error);
	~TransProt ();
	virtual int sendMsgStream (MSG_TYPE *stream, int size) = 0;
	virtual int recvMsgStream (MSG_TYPE *stream, int size) = 0;
protected:
	virtual int sendMsg (MSG_TYPE msg, ACK_TYPE *slast) = 0;
	virtual int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) = 0;
	
	// buffers identifiers
	int inputChannelId;
	int outputChannelId;

	// control variables
	int timeout;
	int slast_size;
	int rnext_size;
	float prob_error;

};


class SendRecv : public TransProt {
protected:
	SendRecv () : TransProt() {};
	SendRecv(int timeout, int slast_size, int rnext_size, float prob_error) : TransProt(timeout, slast_size, rnext_size, prob_error) {};
	~SendRecv () {};
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);

};

class StopNWait: public SendRecv {
public:	
	StopNWait (int timeout, float prob_error);
	~StopNWait () {};
protected:
	int sendMsgStream (MSG_TYPE *stream, int size);
	int recvMsgStream (MSG_TYPE *stream, int size);
private:
	int acknowledge (ackbuff *ack);

	int timeout_count;
};

/*class SlidingWindow : public TransProt {
public:
	SlidingWindow () : TransProt() {};
	~SlidingWindow () {};
	virtual int sendMsgStream (MSG_TYPE *stream, int size) = 0;
	virtual int recvMsgStream (MSG_TYPE *stream, int size) = 0;
protected:
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
};*/

class GoBackN : public SendRecv {
public:
	GoBackN (int timeout, int slast_size, int rnext_size, float prob_error, int window);
	~GoBackN () {};
	int sendMsgStream (MSG_TYPE *stream, int size);
	int recvMsgStream (MSG_TYPE *stream, int size);
private:
	ackbuff acknowledge ();		// return the last ack or nack from the buffer

	int window;

};

class SelectiveRepeat : public SendRecv {
public:
	SelectiveRepeat () : SendRecv() {};
	~SelectiveRepeat () {};
	int sendMsgStream (MSG_TYPE *stream, int size);
	int recvMsgStream (MSG_TYPE *stream, int size);
private:
	ackbuff acknowledge ();		// return the last ack or nack from the buffer

	int window;

};


#endif
