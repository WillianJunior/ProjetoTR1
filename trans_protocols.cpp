#include "trans_protocols.h"

TransProt::TransProt (int timeout, int slast_size, int rnext_size, float prob_error) {
	
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

	this->timeout = timeout;
	this->slast_size = slast_size;
	this->rnext_size = rnext_size;
	this->prob_error = prob_error;
}

TransProt::~TransProt () {

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



int SendRecv::sendMsg(MSG_TYPE msg, ACK_TYPE *slast) {

	msgbuff msg_temp;
	
	msg_temp.mtype = 1;

	// append the slast
	msg_temp.msg = msg << slast_size;
	msg_temp.msg += *slast;

	// apply theoretical error and CRC
	msg_temp.msg = msg_temp.msg << CRC_SIZE;
	msg_temp.msg += crc(msg_temp.msg);

	#ifdef MANUAL_ERROR
	cout << "Error chance (0-1): ";
	cin >> prob_error;
	#endif
	msg_temp.msg = apply_error(msg_temp.msg, prob_error);

	// send the package to the transmition mean buffer
	cout << "Sending message: "; 
	message_pretty_print(msg_temp.msg, slast_size);
	if (msgsnd(inputChannelId, &msg_temp, sizeof(msgbuff), 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	return true;

}

int SendRecv::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;
	
	ack.mtype = 1;
	msg_temp.mtype = 1;

	// receive message from the transmition mean buffer
	cout << "Waiting message..."<< endl;
	
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
	else if (*rnext == EXTRACT_SLAST(msg_temp.msg, slast_size)) {
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

		return true;
	}
	// otherwise send actual rnext
	else {
		cout << "Message received isn't the requested - expected " << *rnext << " but received " << (EXTRACT_SLAST(msg_temp.msg, slast_size)) << " - Sending acknowledge" << endl;
		ack.ack = *rnext;
		ack.ack <<= CRC_SIZE;
		ack.ack += crc(ack.ack);

		#ifdef MANUAL_ERROR
		cout << "Error chance (0-1): ";
		cin >> prob_error;
		#endif
		ack.ack = apply_error(ack.ack, prob_error);

		if (msgsnd(outputChannelId, &ack, sizeof(MSG_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
	}
	
	return false;

} 
