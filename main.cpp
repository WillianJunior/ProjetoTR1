#include <iostream>

#include "basic_types.h"
#include "trans_protocols.h"

using namespace std;


int main () {
	
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
				break;

			case SLIDINGWINDOW:
				cout << "sliding window" << endl << endl;
				break;

			case EXIT:
				cout << "Bye" << endl << endl;
				return 0;

			default:
				cout << "This options does not exist" << endl << endl;

		}
	}

}