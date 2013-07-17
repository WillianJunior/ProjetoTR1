#include "trans_protocols.h"

SelectiveRepeat::SelectiveRepeat (int timeout, int slast_size, int rnext_size, float prob_error, int window) : SendRecv(timeout, slast_size, rnext_size, prob_error) {
	this->window = window;
	current_window_snd = (ackbuff*) malloc(window * sizeof(ackbuff));
}

int SelectiveRepeat::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;
	
	ack.mtype = 1;
	msg_temp.mtype = 1;

	// receive message from the transmition mean buffer
	cout << "[SelectiveRepeat] Waiting message..."<< endl;
	
	if (msgrcv(inputChannelId, &msg_temp, sizeof(msgbuff), 0, 0) < 0) {
		cout << "Error waiting package from the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	cout << "Received message: " << msg_temp.msg << endl 
	<< "crc: " << crc(msg_temp.msg) << endl;

	// CRC check
	if (crc(msg_temp.msg) != 0) {
		// discart package
		cout << "Error during transmition...Exiting..." << endl;
	}
	// check the slast
	else {
		// if ok send updated rnext
		*rnext = nextRNext(*rnext);
		ack.ack = *rnext;
		ack.ack <<= CRC_SIZE;
		ack.ack += crc(ack.ack);
		
		cout << "Message received - Sending acknowledge: " << (EXTRACT_RNEXT(ack.ack)) << endl;

		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		#endif
		ack.ack = apply_error(ack.ack, prob_error);

		if (msgsnd(outputChannelId, &ack, sizeof(ackbuff), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
		*msg = EXTRACT_MSG(msg_temp.msg, slast_size);
		*rnext = EXTRACT_RNEXT(msg_temp.msg);

		return true;
	}
	
	return false;

} 

int SelectiveRepeat::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	int ack_count;
	int nack_flag;
	ACK_TYPE i, j, j_temp;

	signal(SIGALRM, alarm_dummy);

	// allocate space for the buffer
	current_window_rcv = (msgbuff*) malloc(size * sizeof(msgbuff));

	// setup the first window
	for (i=0; i<WINDOW_SIZE; i++) {
		current_window_snd[i].type = NACK;
		current_window_snd[i].ack = i;
	}

	// the first package from the first window is package 0
	i = 0;

	while (i < (ACK_TYPE)size) {

		// set the current package to the beginning of the window
		//j = i;

		// reset the ack_count
		ack_count = 0;

		// send all the requested packages from the window
		for (j=0;j<(ACK_TYPE)window;j++) {
			// check if need to be sent
			if (current_window_snd[j].type == NACK && j < (ACK_TYPE)size) {
				// send the asked package
				sendMsg(stream[current_window_snd[j].ack], &(current_window_snd[j].ack));
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
					current_window_snd[(EXTRACT_SLAST(ack.ack, rnext_size))-i-1].type = ACK;
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
			if (current_window_snd[j].type == ACK && !nack_flag) {
				i++;
				// shift all packages and add a new one to the end
				for(j_temp=j;j<(ACK_TYPE)window-2;j_temp++) {
					cout << "Shifted." << endl;
					getchar();
					current_window_snd[j_temp] = current_window_snd[j_temp+1];
				}

				cout << "Window slided " << j_temp-j << " times." << endl;

				current_window_snd[window-1].type = NACK;
				current_window_snd[window-1].ack = current_window_snd[window-2].ack+1;
			}

			// if it is a nack, set the nack_flag
			else
				nack_flag = true;
		}
	}

	return 0;
}

int SelectiveRepeat::recvMsgStream (MSG_TYPE *stream, int size) {

	msgbuff msg_temp;
	ACK_TYPE i;

	current_window_rcv = (msgbuff*) malloc(size * sizeof(msgbuff));

	// setup the first window
	for (i=0; i<(ACK_TYPE)window; i++) {
		current_window_rcv[i].rcv_flag = NACK;
		current_window_rcv[i].msg_num = i;
	}

	// the first package from the first window is package 0

	while (i < (ACK_TYPE)size) {

		// check if there was any kind of error
		if(recvMsg(&(msg_temp.msg), &(msg_temp.msg_num))) {
			// check if the package is already there
			if (current_window_rcv[(EXTRACT_RNEXT(msg_temp.msg_num))].rcv_flag == ACK) {
				// insert the valid data into the message buffer and set it as received
				current_window_rcv[(EXTRACT_RNEXT(msg_temp.msg_num))].msg = msg_temp.msg;
				current_window_rcv[(EXTRACT_RNEXT(msg_temp.msg_num))].rcv_flag = ACK;

				// shift the window, if possible
				while(current_window_rcv[i].rcv_flag == ACK) {
					cout << "Shifted" << endl;
					i++;
				}
			}

		}
	}



	return 0;

}