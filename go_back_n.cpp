#include "trans_protocols.h"

GoBackN::GoBackN (int timeout, int slast_size, int rnext_size, float prob_error, int window): SendRecv(timeout, slast_size, rnext_size, prob_error) {
	this->window = window;
}

int GoBackN::acknowledge (ackbuff *ack) {
	
	//ackbuff ack_temp;

	while (msgrcv(outputChannelId, ack, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0);

	if (crc(ack->ack) != 0)
			cout << "Acknowledge failed on CRC check" << endl;
		else
			return true;

	return false;
}

int GoBackN::sendMsgStream (MSG_TYPE *stream, int size) {
	
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
		for (;j-i<WINDOW_SIZE && j<(unsigned int)size; j++) {
			cout << "Sending package " << j << endl;
			sendMsg(stream[j], &j);
		}

		// wait for the transmition timeout
		// pause();

		// if no ack or nack, poll, to check if the receiver is still there
		/*if () {

		}
		// if nack, rollback to the nack
		else */
		sleep(1);
		cout << "Window closed. Checking the acks" << endl;
		acknowledge(&ack);
		cout << "Ack received: " << (EXTRACT_RNEXT(ack.ack)) << endl;
		if (EXTRACT_RNEXT(ack.ack) < j) {
			cout << "Nack found: " << (EXTRACT_RNEXT(ack.ack)) << endl;
			i = EXTRACT_RNEXT(ack.ack);
		}
		// ack only
		else {
			cout << "Setting next window" << endl;
			i = j;
		}
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