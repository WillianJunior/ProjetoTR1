#include <iostream>
#include "trans_protocols.h"

int main () {

	TransProt *receiver;
	receiver = new StopNWait();

	MSG_TYPE msg;
	ACK_TYPE rnext = 0;

	while(1) {
		receiver->recvMsg(&msg, &rnext);
		cout << "Received Message: " << (EXTRACT_MSG(msg)) << endl
		<< "rnext: " << rnext << endl << endl;
	}
	
}