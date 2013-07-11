#include <iostream>

#include "basic_types.h"
#include "trans_protocols.h"

using namespace std;


int main () {
	
	SendProt *sender;
	int protocol;
	MSG_TYPE teste[3] = {1,2,3};
	ACK_TYPE rnext = 0;

	while (1) {
		cout << endl << "Select your protocol:" << endl
		<< "1 - Stop and Wait" << endl
		<< "2 - Sliding Window" << endl
		<< "9 - Exit" << endl << endl;

		cin >> protocol;

		switch (protocol) {

			case STOPNWAIT:
				cout << "stopnwait" << endl << endl;
				sender = new StopNWait();
				sender->sendMsg(teste[0], &rnext);
				break;

			case SLIDINGWINDOW:
				cout << "sliding window" << endl << endl;
				break;

			case EXIT:
				cout << "Bye" << endl << endl;
				delete sender;
				return 0;

			default:
				cout << "This options does not exist" << endl << endl;

		}
	}

}