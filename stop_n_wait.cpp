#include "trans_protocols.h"

StopNWait::StopNWait (int timeout, int slast_size, int rnext_size, float prob_error): SendRecv(timeout, slast_size, rnext_size, prob_error) {
	timeout_count = 0;
}

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