#include "trans_protocols.h"
#include <math.h>

#define PACKAGE_SIZE 32
#define REDUNDANCY 3
#define BW 200
#define FREQUENCY 10000
#define SNR 500

long int crc (long int package, long int gen);

double error ();

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
	msg_temp.msg = msg << SLAST_SIZE;
	msg_temp.msg += *slast;

	// apply theoretical error and CRC
	// msg = error(msg ++ crc(msg))
	msg_temp.msg = msg_temp.msg << CRC_SIZE; // simulation of crc
		
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
		/*if (!crc(ack))
			cout << "Acknowledge failed on CRC check" << endl;
		else if (EXTRACT_SLAST....*/

		// extract slast from the ack and check it
		if (EXTRACT_RNEXT(ack.ack) != rnext)
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

	// CRC check
	/*if (!crc(msg)) {
		// discart package
		return false;
	}*/

	cout << "Received message: " << msg_temp.msg << endl;

	// check the slast
	if (*rnext == EXTRACT_SLAST(msg_temp.msg)) {
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

long int crc (long int package, long int gen) {

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

double error () {

        // first calculate Es/N0 (energy per symbol to noise power spectral density)
        double EsN0;
        EsN0 = (double)SNR*BW/FREQUENCY;
        EsN0 = sqrt(EsN0);

        // than calculate the gaussian error function of Es/N0
        double t = 1/(1+0.3275911*EsN0);
        double errf = ((t*(0.254829592+t*(-0.284496736+t*(1.421413741+t*(-1.453152027 + t*1.061405429)))))*exp(-EsN0*EsN0));
        //printf("EsN0: %f, t: %f, erf: %f\n", EsN0, t, errf);
        return errf / 2;

}

