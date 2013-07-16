#include "trans_protocols.h"

void alarm_dummy (int dummy) {};

int GoBackN::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	ACK_TYPE i, j;

	// the first window is the window 0
	i = j = 0;

	// 
	for (;i<size;) {
		// set the window timeout
		// alarm(TIMEOUT);

		// send a window
		for (;j<WINDOW_SIZE; j++)
			sendMsg(stream[i+j], i+j);

		// wait for the transmition timeout
		// pause();

		// if no ack or nack, poll, to check if the receiver is still there
		/*if () {

		}
		// if nack, rollback to the nack
		else */
		if ((ack = acknowledge()).ack < i+j) {
			i = ack.ack;
		}
		// ack only
		else {
			i = i + j + 1;
		}
		j = i;
	}

	return 0;
}

int GoBackN::recvMsgStream (MSG_TYPE *stream, int size) {

	ACK_TYPE rnext = 0;

	for (int i=0; i<size; i++) {
		if (!recvMsg(&stream[i], &rnext))
			i--;
	}

	return 0;

}