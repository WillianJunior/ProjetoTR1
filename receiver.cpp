#include <iostream>
#include "trans_protocols.h"

int main () {

	TransProt *receiver;
	MSG_TYPE stream[10];
	int protocol;
	
	receiver = new StopNWait();

	while (1) {
		cout << endl << "Select your protocol:" << endl
		<< "1 - Stop and Wait" << endl
		<< "2 - Sliding Window" << endl
		<< "9 - Exit" << endl << endl;

		cin >> protocol;

		switch (protocol) {

			case STOPNWAIT:
				cout << "stopnwait" << endl << endl;
				if (receiver == 0)
					receiver = new StopNWait();
				receiver->recvMsgStream(stream, 10);
				cout << "Stream: " << endl;
				for (int i=0; i<10; i++) {
					cout << "stream[" << i << "] = " << stream[i] << endl;
				}
				break;

			case SLIDINGWINDOW:
				cout << "sliding window" << endl << endl;
				break;

			case EXIT:
				cout << "Bye" << endl << endl;
				delete receiver;
				return 0;

			default:
				cout << "This options does not exist" << endl << endl;

		}
	}
}