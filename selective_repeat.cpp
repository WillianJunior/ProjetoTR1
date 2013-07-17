#include "trans_protocols.h"

SelectiveRepeat::SelectiveRepeat (int timeout, int slast_size, int rnext_size, float prob_error, int window) : SendRecv(timeout, slast_size, rnext_size, prob_error) {
	this->window = window;
	current_window = (ackbuff*) malloc(window * sizeof(ackbuff));
}

int SelectiveRepeat::acknowledge (ackbuff *ack) {
	
	//ackbuff ack_temp;
	int ack_count = 0;

	while (msgrcv(outputChannelId, ack, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0)
		ack_count++;

	if (ack_count == 0)
		return false;

	if (crc(ack->ack) != 0)
			cout << "Acknowledge failed on CRC check" << endl;
			
	return true;
}

int SelectiveRepeat::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	int ack_count;
	int nack_flag;
	ACK_TYPE i, j;

	// setup the first window
	for (i=0; i<WINDOW_SIZE; i++) {
		current_window[i].type = NACK;
		current_window[i].ack = i;
	}

	// the first package from the first window is package 0
	i = 0;

	while (i < (ACK_TYPE)size) {

		// set the curent package to the beginning of the window
		//j = i;

		// reset the ack_count
		ack_count = 0;

		// send all the requested packages from the window
		for (j=0;j<WINDOW_SIZE;j++) {
			// check if need to be sent
			if (current_window[j].type == NACK && j < (ACK_TYPE)size) {
				// send the asked package
				sendMsg(stream[current_window[j].ack], &(current_window[j].ack));
			}

			// check if it is already the end of the stream
			if (j >= (ACK_TYPE)size) {
				ack_count++;
			}
		}

		// set a alarm to wait for the acks. WARNING: race condition with the alarm and the msgrcv. the alarm can happen in the while body
		alarm(timeout);

		// keep receiving acks
		while (ack_count < window && msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) >= 0) {
			// check the crc
			if (crc(ack.ack) == 0) {
				// check if it is an ack or a nack
				if (EXTRACT_MSG(ack.ack, rnext_size) == ACK) {
					// if it is an ack, update the window status
					current_window[(EXTRACT_SLAST(ack.ack, rnext_size))-i-1].type = ACK;
					ack_count++;
				} else
					cout << "Received nack from package: " << (EXTRACT_SLAST(ack.ack, rnext_size)) << ". It will be resent." << endl;


			} else
				cout << "Ack failed crc. Discarted." << endl;
		}

		// disarm the timeout
		alarm(0);

		// update the window beginning pointer
		nack_flag = false;
		for (j=0;j<(ACK_TYPE)window;j++) {
			// if it is a ack and there wasn't any nack before, foward the window
			if (current_window[j].type == ACK && !nack_flag) {
				i++;
				// shift all packages and add a new one to the end
				for(int j_temp=j;j<(ACK_TYPE)window-1;j_temp++)
					current_window[j_temp] = current_window[j_temp+1];
				current_window[window-1].type = NACK;
				current_window[window-1].ack = current_window[window-2].ack+1;
			}

			// if it is a nack, set the nack_flag
			else
				nack_flag = true;
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