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

int StopNWait::sendMsg (MSG_TYPE msg, ACK_TYPE *slast) {

	ackbuff ack;
	msgbuff msg_temp;
	ACK_TYPE rnext = !(*slast);
	struct msqid_ds msg_info;
	
	#ifdef MANUAL_ERROR
	float prob_error;
	#endif

	signal(SIGALRM, alarm_dummy);

	ack.mtype = 1;
	msg_temp.mtype = 1;

	// append the slast
	msg_temp.msg = msg << SLAST_SIZE;
	msg_temp.msg += *slast;

	// apply theoretical error and CRC
	msg_temp.msg = msg_temp.msg << CRC_SIZE;
	msg_temp.msg += crc(msg_temp.msg);

	#ifdef AUTO_ERROR
	msg_temp.msg = apply_error(msg_temp.msg, PROB_ERROR);
	#endif
	#ifdef MANUAL_ERROR
	cout << "Error chance (0-1): ";
	cin >> prob_error;
	msg_temp.msg = apply_error(msg_temp.msg, prob_error);
	#endif

	// send the package to the transmition mean buffer
	cout << "Sending message: " << msg_temp.msg << endl;
	if (msgsnd(inputChannelId, &msg_temp, sizeof(msgbuff), 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	// clean the ack buffer, if filled with old ack's
	if (timeout_count > 0) {
		msgctl(outputChannelId, IPC_STAT, &msg_info);
		if (msg_info.msg_qnum > 0) {
			cout << "ack flushed: " << ack.ack << " - number or msgs: " << msg_info.msg_qnum << endl;
			if (msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) < 0) {
				cout << "Message Queue error: " << strerror(errno) << endl;
				exit(1);
			}
		}
		timeout_count = 0;
	}

	// wait for the ack or the timeout
	cout << "Waiting for the acknowledge" << endl;
	alarm(TIMEOUT);
	if (msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) < 0)
		if (errno != EINTR){
			cout << "Message Queue error: " << strerror(errno) << endl;
			exit(1);
		}

	cout << "ack received: " << ack.ack << endl;
	// treat the result of an ack
	if (alarm(0) > 0) {
		// crc on the ack
		if (crc(ack.ack) != 0)
			cout << "Acknowledge failed on CRC check" << endl;
		// extract slast from the ack and check it
		else if (EXTRACT_RNEXT(ack.ack) != rnext)
			cout << "Received wrong acknowledge - expected " << rnext << " but instead " << (EXTRACT_RNEXT(ack.ack)) << endl;
		else {
			cout << "Received correct acknowledge" << endl;
			// update slast
			*slast = rnext;
			return true;
		}
	} 
	// treat the result of a timeout
	else {
		cout << "Timeout!" << endl;
		timeout_count++;
	}
	
	return false;

}

int StopNWait::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;
	
	#ifdef MANUAL_ERROR
	float prob_error;
	#endif

	ack.mtype = 1;
	msg_temp.mtype = 1;

	// receive message from the transmition mean buffer
	cout << "Waiting message..."<< endl;
	
	if (msgrcv(inputChannelId, &msg_temp, sizeof(msgbuff), 0, 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
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
	else if (*rnext == EXTRACT_SLAST(msg_temp.msg)) {
		// if ok send updated rnext
		cout << "Message received - Sending acknowledge" << endl;
		*rnext = !(*rnext);
		ack.ack = *rnext;
		ack.ack <<= CRC_SIZE;
		ack.ack += crc(ack.ack);
		#ifdef AUTO_ERROR
		ack.ack = apply_error(ack.ack, PROB_ERROR);
		#endif
		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		ack.ack = apply_error(ack.ack, prob_error);
		#endif

		if (msgsnd(outputChannelId, &ack, sizeof(ackbuff), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
		*msg = EXTRACT_MSG(msg_temp.msg);

		return true;
	}
	// otherwise send actual rnext
	else {
		cout << "Message received isn't the requested - expected " << *rnext << " but received " << (EXTRACT_SLAST(msg_temp.msg)) << " - Sending acknowledge" << endl;
		ack.ack = *rnext;
		ack.ack <<= CRC_SIZE;
		ack.ack += crc(ack.ack);
		#ifdef AUTO_ERROR
		ack.ack = apply_error(ack.ack, PROB_ERROR);
		#endif
		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		ack.ack = apply_error(ack.ack, prob_error);
		#endif
		if (msgsnd(outputChannelId, &ack, sizeof(MSG_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
	}
	
	return false;

}