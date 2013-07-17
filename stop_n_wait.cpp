#include "trans_protocols.h"

StopNWait::StopNWait (int timeout, float prob_error): SendRecv(timeout, 1, 1, prob_error) {
	timeout_count = 0;
}

int StopNWait::sendMsgStream (MSG_TYPE *stream, int size) {
	
	ackbuff ack;
	ACK_TYPE slast = 0;

	for (int i=0; i<size; i++) {
		if (sendMsg(stream[i], &slast) && acknowledge(&ack)) {
			// extract slast from the ack and check it
			if (EXTRACT_RNEXT(ack.ack) == slast) {
				cout << "Received wrong acknowledge - expected " << !slast << " but instead " << (EXTRACT_RNEXT(ack.ack)) << endl;
				i--;
			} else {
				cout << "Received correct acknowledge" << endl;
				// update slast
				slast = !slast;
			}
		} else
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

int StopNWait::acknowledge (ackbuff *ack) {

	struct msqid_ds msg_info;

	signal(SIGALRM, alarm_dummy);

	ack->mtype = 1;

	// clean the ack buffer, if filled with old ack's
	if (timeout_count > 0) {
		msgctl(outputChannelId, IPC_STAT, &msg_info);
		if (msg_info.msg_qnum > 0) {
			cout << "ack flushed: " << ack->ack << " - number or msgs: " << msg_info.msg_qnum << endl;
			if (msgrcv(outputChannelId, ack, sizeof(ackbuff), 0, 0) < 0) {
				cout << "Message Queue error: " << strerror(errno) << endl;
				exit(1);
			}
		}
		timeout_count = 0;
	}

	// wait for the ack or the timeout
	cout << "Waiting for the acknowledge" << endl;
	alarm(timeout);
	if (msgrcv(outputChannelId, ack, sizeof(ackbuff), 0, 0) < 0)
		if (errno != EINTR){
			cout << "Message Queue error: " << strerror(errno) << endl;
			exit(1);
		}

	cout << "ack received: " << ack->ack << endl;
	// treat the result of an ack
	if (alarm(0) > 0) {
		// crc on the ack
		if (crc(ack->ack) != 0)
			cout << "Acknowledge failed on CRC check" << endl;
		else
			return true;
	} 
	// treat the result of a timeout
	else {
		cout << "Timeout!" << endl;
		//timeout++;
	}
	
	return false;
}