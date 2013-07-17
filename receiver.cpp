#include <iostream>
#include "trans_protocols.h"

int main () {

	TransProt *receiver = 0;
	MSG_TYPE stream[10];
	int protocol;

	while (1) {
		cout << endl << "Select your protocol:" << endl
		<< "1 - Stop and Wait" << endl
		<< "2 - Go Back N" << endl
		<< "3 - Selective Repeat" << endl
		<< "9 - Exit" << endl << endl;

		cin >> protocol;

		switch (protocol) {

			case STOPNWAIT:
				cout << "stopnwait" << endl << endl;
				receiver = new StopNWait(TIMEOUT, PROB_ERROR);
				receiver->recvMsgStream(stream, 10);
				
				cout << "Stream: " << endl;
				for (int i=0; i<10; i++) {
					cout << "stream[" << i << "] = " << stream[i] << endl;
				}

				delete receiver;
				break;

			case GOBACKN:
				cout << "go back n" << endl << endl;
				receiver = new GoBackN(TIMEOUT, PACK_ID_SIZE, PACK_ID_SIZE, PROB_ERROR, WINDOW_SIZE);
				receiver->recvMsgStream(stream, 10);

				cout << "Stream: " << endl;
				for (int i=0; i<10; i++) {
					cout << "stream[" << i << "] = " << stream[i] << endl;
				}

				delete receiver;
				break;

			case SELECTIVEREPEAT:
				cout << "selective repeat" << endl << endl;
				receiver = new SelectiveRepeat();
				receiver->recvMsgStream(stream, 10);

				cout << "Stream: " << endl;
				for (int i=0; i<10; i++) {
					cout << "stream[" << i << "] = " << stream[i] << endl;
				}

				delete receiver;
				break;

			case EXIT:
				cout << "Bye" << endl << endl;
				return 0;

			default:
				cout << "This options does not exist" << endl << endl;

		}
	}
}