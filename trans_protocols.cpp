#include "trans_protocols.h"

void alarm_dummy (int dummy);

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

int StopNWait::sendMsg (MSG_TYPE msg, ACK_TYPE *slast) {

	ackbuff ack;
	msgbuff msg_temp;
	ACK_TYPE rnext = !(*slast);

	signal(SIGALRM, alarm_dummy);

	// append the slast
	cout << "msg: " << msg << endl;
	msg_temp.msg = msg << SLAST_SIZE;
	msg_temp.msg += *slast;
	cout << "msg + slast: " << msg_temp.msg << endl;

	// apply theoretical error and CRC
	msg_temp.msg = msg_temp.msg << CRC_SIZE ;
	msg_temp.msg += crc(msg, GENERATOR);
	msg_temp.msg = apply_error(msg_temp.msg);

	// send the package to the transmition mean buffer
	cout << "Sending message: " << msg_temp.msg << endl;
	if (msgsnd(inputChannelId, &msg_temp, sizeof(msgbuff), 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	// wait for the ack or the timeout
	cout << "Waiting for the acknowledge" << endl;
	alarm(TIMEOUT);
	if (msgrcv(outputChannelId, &ack, sizeof(ackbuff), 0, 0) < 0)
		if (errno != EINTR){
			cout << "Message Queue error: " << strerror(errno) << endl;
			exit(1);
		}

	// treat the result of an ack
	if (alarm(0) > 0) {
		// crc on the ack
		if (crc(ack.ack, GENERATOR) != 0)
			cout << "Acknowledge failed on CRC check" << endl;
		// extract slast from the ack and check it
		else if (EXTRACT_RNEXT(ack.ack) != rnext)
			cout << "Received wrong acknowledge - expected " << rnext << " but instead " << (EXTRACT_RNEXT(ack.ack)) << endl;
		else {
			cout << "Received correct acknowledge" << endl;
			// update slast
			*slast = rnext;
			return true;
		}
	} 
	// treat the result of a timeout
	else
		cout << "Timeout!" << endl;
		// return the counter to send it again

	return false;

}

int StopNWait::recvMsg (MSG_TYPE *msg, ACK_TYPE *rnext) {

	ackbuff ack;
	msgbuff msg_temp;

	// receive message from the transmition mean buffer
	cout << "Waiting message..."<< endl;
	
	if (msgrcv(inputChannelId, &msg_temp, sizeof(msgbuff), 0, 0) < 0) {
		cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
		exit(1);
	}

	cout << "Received message: " << msg_temp.msg << endl;

	// CRC check
	if (crc(msg_temp.msg, GENERATOR) != 0) {
		// discart package
		cout << "Error during transmition...Exiting..." << endl;
	}
	// check the slast
	else if (*rnext == EXTRACT_SLAST(msg_temp.msg)) {
		// if ok send updated rnext
		cout << "Message received - Sending acknowledge" << endl;
		*rnext = !(*rnext);
		ack.ack = *rnext;
		// ack = ack ++ crc(ack)
		ack.ack = ack.ack << CRC_SIZE; // simulation of crc
		if (msgsnd(outputChannelId, &ack, sizeof(ACK_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
		*msg = msg_temp.msg;

		return true;
	}
	// otherwise send actual rnext
	else {
		cout << "Message received isn't the requested - expected " << *rnext << " but received " << (EXTRACT_SLAST(msg_temp.msg)) << " - Sending acknowledge" << endl;
		ack.ack = *rnext;
		// ack = ack ++ crc(ack)
		ack.ack = ack.ack << CRC_SIZE; // simulation of crc
		if (msgsnd(outputChannelId, &ack, sizeof(MSG_TYPE), 0) < 0) {
			cout << "Error sending package through the msg queue: " << strerror(errno) << endl;
			exit(1);
		}
	}
	
	return false;

}

void alarm_dummy (int dummy) {}

MSG_TYPE crc (MSG_TYPE package, MSG_TYPE gen) {

        long int gen_temp;
        long int rem;
        long int checker;

        gen_temp = gen << (PACKAGE_SIZE - REDUNDANCY - 1);
        rem = package ^ gen_temp;
        checker = pow(2,PACKAGE_SIZE-1);
        while (gen_temp > gen) {
                gen_temp = gen_temp >> 1;
                checker = checker >> 1;
                if (rem >= checker)
                        rem = rem ^ gen_temp;
        }

        return rem;
}

MSG_TYPE apply_error (MSG_TYPE package) {

	long int i, j, w;
	int r=0;
	char buffer[PACKAGE_SIZE];
	float error;
	int set;
	srand(RAND_SEED);
	for (i=PACKAGE_SIZE-1;i>=0;i--) {
                                    #ifdef PROB_ERROR
                                    error = (((float)(rand()%100))/100);
				    if (error <= PROB_ERROR)
					set = 1;
				    else
					set = 0;
				    cout << "Erro: " << error << endl;
                                    #endif
                                    #ifdef PROB_GIVEN
                                    error = (rand()%100)>PROB_ERROR;
                                    #endif
                                    //printf("error: %d, packmod2: %d\n", error, package);
                                    buffer[i] = (package%2 ^ set);
                                    package = package >> 1;
        }
	

	for(i=0,j=PACKAGE_SIZE-1;i<PACKAGE_SIZE;i++,j--) {
		w = pow(2, i);
	        r+=(buffer[j])*w;
	}

	return r;


}

