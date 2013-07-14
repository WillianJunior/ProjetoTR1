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
		<< "2 - Sliding Window" << endl
		<< "9 - Exit" << endl << endl;

		cin >> protocol;

		switch (protocol) {

			case STOPNWAIT:
				cout << "stopnwait" << endl << endl;
				if (sender == 0)
					sender = new StopNWait();
				sender->sendMsgStream(stream, 10);
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
