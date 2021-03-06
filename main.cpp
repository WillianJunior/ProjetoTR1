#include <iostream>

#include "basic_types.h"
#include "trans_protocols.h"

using namespace std;

MSG_TYPE stream[10] = {10,20,30,40,50,60,70,80,90,100};

int main () {
	
	TransProt *sender = 0;
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
				cout << "Efficiency: " << stopnwait_eff()*100 << '%' << endl;
				sender = new StopNWait(TIMEOUT, PROB_ERROR);
				sender->sendMsgStream(stream, 10);
				break;

			case GOBACKN:
				cout << "go back n" << endl << endl;
				cout << "Efficiency: " << gobackn_eff()*100 << '%' << endl;
				sender = new GoBackN(TIMEOUT, PACK_ID_SIZE, PACK_ID_SIZE, PROB_ERROR, WINDOW_SIZE);
				sender->sendMsgStream(stream, 10);
				break;

			case SELECTIVEREPEAT:
				cout << "selective repeat" << endl << endl;
				cout << "Efficiency: " << selectiverepeat_eff()*100 << '%' << endl;
				sender = new SelectiveRepeat(TIMEOUT, PACK_ID_SIZE, PROB_ERROR, WINDOW_SIZE);
				sender->sendMsgStream(stream, 10);
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
