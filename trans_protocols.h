#ifndef TRANS_PROTOCOLS_H
#define TRANS_PROTOCOLS_H

#include <iostream>

#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "basic_types.h"

using namespace std;

class SendProt {
public:
	SendProt ();
	~SendProt ();
	virtual int sendMsg (MSG_TYPE msg, ACK_TYPE *slast) = 0;
	virtual int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) = 0;
protected:
	int inputChannelId;
	int outputChannelId;
};

class StopNWait: public SendProt {
public:
	StopNWait () : SendProt() {};
	~StopNWait () {};
	int sendMsg (MSG_TYPE msg, ACK_TYPE *slast);
	int recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext);
};

#endif