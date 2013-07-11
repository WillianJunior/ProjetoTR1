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

int StopNWait::sendMsg (MSG_TYPE *msg, int *slast) {

	ACK_TYPE ack;
	int rnext = !(*slast);

	signal(SIGALRM, alarm_dummy);

	// append the slast
	// msg ++ slast

	// apply theoretical error and CRC
	// msg = error(msg ++ crc(msg))
		
	// send the package to the transmition mean buffer
	cout << "Sending message: " << *msg << endl;
	if (msgsnd(inputChannelId, msg, sizeof(MSG_TYPE), 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	// wait for the ack or the timeout
	cout << "Waiting for the acknowledge" << endl;
	alarm(TIMEOUT);
	if (msgrcv(outputChannelId, &ack, sizeof(ACK_TYPE), 0, 0) < 0)
		if (errno != EINTR){
			cout << "Message Queue error: " << strerror(errno) << endl;
			exit(1);
		}

	// treat the result of an ack
	if (alarm(0) > 0) {
		// crc on the ack
		/*if (!crc(ack))
			cout << "Acknowledge failed on CRC check" << endl;
		else if (EXTRACT_SLAST....*/

		// extract slast from the ack and check it
		if (EXTRACT_SLAST(ack) != rnext)
			cout << "Received wrong acknowledge" << endl;
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

int StopNWait::recvMsg (MSG_TYPE *msg, int *rnext) {

	ACK_TYPE ack;

	// receive message from the transmition mean buffer
	cout << "Waiting message: " << *msg << endl;
	if (msgrcv(inputChannelId, msg, sizeof(MSG_TYPE), 0, 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	// CRC check
	/*if (!crc(msg)) {
		// discart package
		return false;
	}*/

	// otherwise check the slast
	if (*rnext == EXTRACT_SLAST(*msg)) {
		// if ok send updated rnext
		cout << "Message received - Sending acknowledge" << endl;
		*rnext = !(*rnext);
		ack = *rnext;
		// ack = ack ++ crc(ack)
		if (msgsnd(outputChannelId, &ack, sizeof(ACK_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}

		return true;
	}
	// otherwise send actual rnext
	else {
		cout << "Message received is corrupted - Sending acknowledge" << endl;
		ack = *rnext;
		// ack = ack ++ crc(ack)
		if (msgsnd(outputChannelId, &ack, sizeof(ACK_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
	}
	
	
	return false;

}

void alarm_dummy (int dummy) {}