#include "trans_protocols.h"

int StopNWait::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ACK_TYPE slast = 0;

	for (int i=0; i<size; i++) {
		if (!sendMsg(stream[i], &slast))
			i--;
	}

	return 0;
}

int StopNWait::recvMsgStream (MSG_TYPE *stream, int size) {

	ACK_TYPE rnext = 0;

	for (int i=0; i<size; i++) {
		if (!recvMsg(&stream[i], &rnext))
			i--;
	}

	return 0;

}