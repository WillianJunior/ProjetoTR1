#ifndef TRANS_PROTOCOLS_H
#define TRANS_PROTOCOLS_H

#include <stdio.h>
#include <stdlib.h>

using namespace std;

class SendProt {
public:
	SendProt () {};
	virtual int send_msg (MSG_TYPE msg);
};

class StopNWait: public SendProt {
public:
	StopNWait ();
};

class SlidingWindow: public SendProt {
public:
	SlidingWindow();
};

#endif