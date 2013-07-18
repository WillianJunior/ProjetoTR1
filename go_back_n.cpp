#include "trans_protocols.h"

GoBackN::GoBackN (int timeout, int slast_size, int rnext_size, float prob_error, int window): SendRecv(timeout, slast_size, rnext_size, prob_error) {
	this->window = window;
}

int GoBackN::sendMsgStream (MSG_TYPE *stream, int size) {

	ackbuff ack, temp_ack;
	ID_TYPE i;

	// the first window is the window 0
	i = 0;

	while (i<(ID_TYPE)size) {

		// send a window
		for (ID_TYPE j=i;j-i<window && j<(ID_TYPE)size; j++) {
			cout << "Sending package " << j << endl;
			sendMsg(stream[j], &j);
		}

		// wait for the transmition timeout
		sleep(timeout);

		int ack_count = 0;

		// get the first ack and then compare if there is a newer to get
		if (msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0)
			ack_count++;
		while (msgrcv(outputChannelId, &temp_ack, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0) {
			if (crc(temp_ack.ack) == 0)
				ack = temp_ack;
			ack_count++;
		}

		// if no ack or nack, poll, to check if the receiver is still there?
		if (ack_count == 0)
			cout << "No acks nor nacks received" << endl;
		else if (crc(ack.ack) != 0)
			cout << "None of the acknowledges passed on CRC check" << endl; 
		else {
			cout << "Shifting window" << endl;
			cout << "Ack received: " << (EXTRACT_ID_FROM_ACK(ack.ack, slast_size)) << endl;
			if (EXTRACT_ID_FROM_ACK(ack.ack, slast_size) < i+window) {
				cout << "Nack found: " << (EXTRACT_ID_FROM_ACK(ack.ack, slast_size)) << endl;
				i = EXTRACT_ID_FROM_ACK(ack.ack, slast_size);
			}
			// ack only
			else {
				cout << "Setting next window" << endl;
				i += window;
			}
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