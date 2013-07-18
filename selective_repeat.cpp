#include "trans_protocols.h"

SelectiveRepeat::SelectiveRepeat (int timeout, ID_TYPE identifiers_size, float prob_error, int window) : SendRecv(timeout, identifiers_size, identifiers_size, prob_error) {
	this->window = window;
	this->identifiers_size = identifiers_size;
	current_window = (msg_window*) malloc(window * sizeof(msg_window));
}

int SelectiveRepeat::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;
	#ifdef AUTO_DELAY
	int delay;
	#endif
	
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
		ack.ack = (ACK_TYPE)ACK;
		ack.ack <<= ACK_TYPE_SIZE;
		*rnext = nextRNext(EXTRACT_ID_FROM_MSG(msg_temp.msg, identifiers_size));
		ack.ack += *rnext;
		ack.ack <<= CRC_SIZE;
		ack.ack += crc(ack.ack);
		
		cout << "Message received: ";
		message_pretty_print(msg_temp.msg, slast_size);
		cout <<"Sending acknowledge: " << (EXTRACT_ID_FROM_ACK(ack.ack, identifiers_size)) << endl;

		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		#endif
		ack.ack = apply_error(ack.ack, prob_error);

		#ifdef AUTO_DELAY
		// add a delay
		delay = rand()%MAX_DELAY;
		cout << "Transmission delay: " << delay << endl;
		sleep(delay);
		#endif

		#ifdef MANUAL_ERROR
		cout << "send this ack? (0,N/1,Y)";
		cin >> prob_error;
		if(prob_error == 1) {
		#endif

		if (msgsnd(outputChannelId, &ack, sizeof(ackbuff), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
		*msg = msg_temp.msg;
		*rnext = EXTRACT_ID_FROM_MSG(msg_temp.msg, identifiers_size);

		#ifdef MANUAL_ERROR
		}
		#endif		

		return true;
	}
	
	return false;

} 

int SelectiveRepeat::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	ID_TYPE ack_count;
	int nack_flag;
	ACK_TYPE i, j;
	int waiter = 0;

	signal(SIGALRM, alarm_dummy);

	// setup the first window
	for (i=0; i<window; i++) {
		// put the package number
		current_window[i].identifier = i;
		current_window[i].ack_type = NACK;
	}

	// the first package from the first window is package 0
	i = 0;

	while (i < (ACK_TYPE)size) {

		// set the current package to the beginning of the window
		//j = i;

		// reset the ack_count
		ack_count = 0;

		// send all the requested packages from the window
		for (j=0;j<window;j++) {
			// check if need to be sent
			if (current_window[j].ack_type == NACK && j < (ACK_TYPE)size) {
				// send the asked package
				sendMsg(stream[current_window[j].identifier], &(current_window[j].identifier));
			}

			// check if it is already the end of the stream to prevent accessing beyond the size
			if (j >= (ACK_TYPE)size) {
				ack_count++;
			}
		}

		#ifdef MANUAL_ERROR
		cout << "Shaw we go? (Y,1/N,0)" << endl;
		while (waiter == 0)
			cin >> waiter;
		#endif

		// set a alarm to wait for the acks. WARNING: race condition with the alarm and the msgrcv. the alarm can happen in the while body
		alarm(timeout);

		// keep receiving acks
		while (ack_count < window && msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) >= 0) {
			// check the crc
			if (crc(ack.ack) == 0) {
				// check if it is an ack or a nack
				if (EXTRACT_ACK_TYPE_FROM_ACK(ack.ack, identifiers_size) == ACK) {
					// if it is an ack, update the window status
					current_window[EXTRACT_ID_FROM_ACK(ack.ack, identifiers_size)-i-1].ack_type = ACK;
					ack_count++;
				} else
					cout << "Received nack from package: " << EXTRACT_ID_FROM_ACK(ack.ack, identifiers_size) << ". It will be resent." << endl;


			} else
				cout << "Ack failed crc. Discarted." << endl;
		}

		// disarm the timeout
		alarm(0);

		// update the window beginning pointer
		nack_flag = false;
		for (ID_TYPE k=0;k<window;k++) {
			// if it is a ack and there wasn't any nack before, foward the window
			if (current_window[0].ack_type == ACK && !nack_flag) {
				i++;
				// shift all packages and add a new one to the end
				for(ID_TYPE j_temp=0;j_temp<window-1;j_temp++)
					current_window[j_temp] = current_window[j_temp+1];

				current_window[window-1].ack_type = NACK;
				current_window[window-1].identifier = current_window[window-2].identifier + 1;
			}
			// if it is a nack, set the nack_flag
			else
				nack_flag = true;
		}
	}

	return 0;
}

int SelectiveRepeat::recvMsgStream (MSG_TYPE *stream, int size) {

	MSG_TYPE msg_temp;
	ID_TYPE msg_num;
	ID_TYPE msg_count = 0;
	receiver_msgbuff *message_buffer = (receiver_msgbuff*) malloc(size * sizeof(receiver_msgbuff));

	// setup the first window
	for (int i=0; i<size; i++) {
		message_buffer[i].rcv_flag = NACK;
		message_buffer[i].msg_num = i;
	}

	while (msg_count < (ID_TYPE)size) {

		// check if there was any kind of error
		if(recvMsg(&msg_temp, &msg_num)) {
			
			// check if the package is already there
			if (message_buffer[EXTRACT_ID_FROM_MSG(msg_temp, identifiers_size)].rcv_flag == NACK) {
				// insert the valid data into the message buffer and set it as received
				message_buffer[EXTRACT_ID_FROM_MSG(msg_temp, identifiers_size)].payload = EXTRACT_MSG_FROM_MSG(msg_temp, identifiers_size);
				message_buffer[EXTRACT_ID_FROM_MSG(msg_temp, identifiers_size)].msg_num = msg_num;
				message_buffer[EXTRACT_ID_FROM_MSG(msg_temp, identifiers_size)].rcv_flag = ACK;

				// shift the window, if possible
				while(message_buffer[msg_count].rcv_flag == ACK && msg_count < (ID_TYPE)size) {
					cout << "Shifted" << endl;
					msg_count++;
				}
			}

		}
	}

	for (int i=0;i<size;i++)
		stream[i] = message_buffer[i].payload;

	return 0;

}