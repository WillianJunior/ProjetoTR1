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
	virtual int sendMsg (MSG_TYPE msg, ACK_TYPE *slast) = 0;
	virtual int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) = 0;
protected:
	int inputChannelId;
	int outputChannelId;
};

class StopNWait: public TransProt {
public:
	StopNWait () : TransProt() {timeout = 0;};
	~StopNWait () {};
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
private:
	int timeout;
};

class SlidingWindow : public TransProt {
public:
	SlidingWindow () : TransProt() {};
	~SlidingWindow () {};
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
};

#endif
