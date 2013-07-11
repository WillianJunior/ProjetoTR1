#include "trans_protocols.h"

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

int StopNWait::sendMsg (MSG_TYPE msg, int size) {

	// apply theoretical error

	// send the package to the transmition mean buffer

	// wait for the ack or the timeout

	// treat the result of the ack or timeout

	// repeat all

	return 0;

}