#include "trans_protocols.h"

ackbuff SelectiveRepeat::acknowledge () {
	
	ackbuff ack_temp;

	while (msgrcv(outputChannelId, &ack_temp, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0);

	return ack_temp;
}

int SelectiveRepeat::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	ACK_TYPE i, j;

	// the first window is the window 0
	i = 0;

	// 
	while (i<(unsigned int)size) {
		// set the window timeout
		// alarm(TIMEOUT);

		j = i;

		// send a window
		for (;j-i<WINDOW_SIZE; j++)
			sendMsg(stream[j], &j);

		// wait for the transmition timeout
		// pause();

		// if no ack or nack, poll, to check if the receiver is still there
		/*if () {

		}
		// if nack, rollback to the nack
		else */
		if ((ack = acknowledge()).ack <= j) {
			i = ack.ack;
		}
		// ack only
		else {
			i = j + 1;
		}
	}	

	return 0;
}

int SelectiveRepeat::recvMsgStream (MSG_TYPE *stream, int size) {

	ACK_TYPE rnext = 0;

	for (int i=0; i<size; i++) {
		if (!recvMsg(&stream[i], &rnext))
			i--;
	}

	return 0;

}