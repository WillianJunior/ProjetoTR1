#include "trans_protocols.h"

void alarm_dummy (int dummy);

SendProt::SendProt () {
	
	// create/access the input msg queue
	if ((inputChannelId = msgget(IN_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		cout << "Error obtaining the input msg queue: " << strerror(errno) << endl;
		exit(1);
	}
	// create/access the output msg queue
	if ((outputChannelId = msgget(OUT_MSGQ_KEY, IPC_CREAT|0x1FF)) < 0) {
		cout << "Error obtaining the output msg queue: " << strerror(errno) << endl;
		exit(1);
	}
}

SendProt::~SendProt () {

	// free input queue
	if ((msgctl(inputChannelId, IPC_RMID, NULL)) < 0) {
		cout << "Error freeing the input msg queue: " << strerror(errno) << endl;
		exit(1);
	}
    
    // free output queue
    if ((msgctl(outputChannelId, IPC_RMID, NULL)) < 0) {
		cout << "Error freeing the output msg queue: " << strerror(errno) << endl;
		exit(1);
	}
}

int StopNWait::sendMsg (MSG_TYPE msg, ACK_TYPE *slast) {

	ackbuff ack;
	msgbuff msg_temp;
	ACK_TYPE rnext = !(*slast);
	float prob_error;

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

	// wait for the ack or the timeout
	cout << "Waiting for the acknowledge" << endl;
	alarm(TIMEOUT);
	if (msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) < 0)
		if (errno != EINTR){
			cout << "Message Queue error: " << strerror(errno) << endl;
			exit(1);
		}

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
	else
		cout << "Timeout!" << endl;
		// return the counter to send it again

	return false;

}

int StopNWait::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;
	float prob_error;

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
		ack.ack = ack.ack << CRC_SIZE;
		ack.ack += crc(ack.ack);
		#ifdef AUTO_ERROR
		ack.ack = apply_error(ack.ack, PROB_ERROR);
		#endif
		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		ack.ack = apply_error(ack.ack, prob_error);
		#endif

		if (msgsnd(outputChannelId, &ack, sizeof(ACK_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
		*msg = msg_temp.msg;

		return true;
	}
	// otherwise send actual rnext
	else {
		cout << "Message received isn't the requested - expected " << *rnext << " but received " << (EXTRACT_SLAST(msg_temp.msg)) << " - Sending acknowledge" << endl;
		ack.ack = *rnext;
		// ack = ack ++ crc(ack)
		ack.ack = ack.ack << CRC_SIZE; // simulation of crc
		if (msgsnd(outputChannelId, &ack, sizeof(MSG_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
	}
	
	return false;

}

void alarm_dummy (int dummy) {}
