#include "trans_protocols.h"

GoBackN::GoBackN (int timeout, int slast_size, int rnext_size, float prob_error, int window): SendRecv(timeout, slast_size, rnext_size, prob_error) {
 this->window = window;
}

//int GoBackN::acknowledge (ackbuff *ack) {
  
 //ackbuff ack_temp;
//	int ack_count = 0;

//	while (msgrcv(outputChannelId, ack, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0)
//	 ack_count++;
//
//	if (ack_count == 0)
//	 return false;
//
//	if (crc(ack->ack) != 0)
//	 cout << "Acknowledge failed on CRC check" << endl;
 	  
//	return true;
//}

int GoBackN::sendMsgStream (MSG_TYPE *stream, int size) {
  
 ackbuff ack;
 ackbuff* bolacha;
 ACK_TYPE i, j;

 // the first window is the window 0
 i = 0;

bolacha = (ackbuff*)malloc(sizeof(ackbuff));

 	// 
 while (i<(ACK_TYPE)size) {
 	// set the window timeout
 	// alarm(TIMEOUT);

 	j = i;

 	// send a window
 	for (;j-i<WINDOW_SIZE && j<(ACK_TYPE)size; j++) {
 	 cout << "Sending package " << j << endl;
 	 sendMsg(stream[j], &j);
 	}

 	// wait for the transmition timeout
 	sleep(timeout);

 	int ack_count = 0;

 	while (msgrcv(outputChannelId, bolacha, sizeof(ackbuff), 0, IPC_NOWAIT) >= 0)
 	ack_count++;

 	// if no ack or nack, poll, to check if the receiver is still there
 	if (ack_count == 0) {
 	 cout << "No acks nor nacks received" << endl;
 	} else if (crc(bolacha->ack) != 0)
 	 cout << "Acknowledge failed on CRC check" << endl; 
 	 else {
 	 cout << "Window closed. Checking the acks" << endl;
 	 cout << "Ack received: " << (EXTRACT_RNEXT(ack.ack)) << endl;
 	 if (EXTRACT_RNEXT(ack.ack) < j) {
 	 cout << "Nack found: " << (EXTRACT_RNEXT(ack.ack)) << endl;
 	 i = EXTRACT_RNEXT(ack.ack);
 	 }
 	 // ack only
 	 else {
 	 cout << "Setting next window" << endl;
 	 i = j;
 	 }
 	}
 }

 return 0;
}

int GoBackN::recvMsgStream (MSG_TYPE *stream, int size) {

 ACK_TYPE rnext = 0;

 for (int i=0; i<size; i++) {
 	if (!recvMsg(&stream[i], &rnext))
 	 i--;
 }

 return 0;

}