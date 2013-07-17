#include <iostream>

#include "basic_types.h"
#include "trans_protocols.h"

using namespace std;

MSG_TYPE stream[10] = {1,2,3,4,5,6,7,8,9,10};

int main () {
	
	TransProt *sender = 0;
	int protocol;

	while (1) {
		cout << endl << "Select your protocol:" << endl
		<< "1 - Stop and Wait" << endl
		<< "2 - Go Back N" << endl
		<< "9 - Exit" << endl << endl;

		cin >> protocol;

		switch (protocol) {

			case STOPNWAIT:
				cout << "stopnwait" << endl << endl;
				sender = new StopNWait();
				sender->sendMsgStream(stream, 10);
				delete sender;
				break;

			case GOBACKN:
				cout << "go back n" << endl << endl;
				sender = new GoBackN();
				sender->sendMsgStream(stream, 10);
				delete sender;
				break;

			case EXIT:
				cout << "Bye" << endl << endl;
				return 0;

			default:
				cout << "This options does not exist" << endl << endl;

		}
	}

}
