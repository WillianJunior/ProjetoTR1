#include "util.h"

#include <iostream>
using namespace std;

MSG_TYPE crc (MSG_TYPE package) {

  int size = 0;
  MSG_TYPE gen;
  MSG_TYPE checker;
  MSG_TYPE package_temp = package;

  for (int i=0;  i<FULL_PACK_SIZE; i++) {
    if (package_temp % 2)
      size = i;
    package_temp >>= 1;
  }

  //cout << "size: " << size << endl;
  gen = CRC_GEN_POL << (size - CRC_SIZE);
  //cout << "gen: " << gen << endl;
  checker = 1 << size;
  //cout << "checker: " << checker << endl;

  for (int i=0; i<(size - CRC_SIZE + 1); i++) {
    if (package >= checker)
      package ^= gen;
    gen >>= 1;
    checker >>= 1;
  }

  return package;
}

MSG_TYPE apply_error (MSG_TYPE package, float prob_error) {

  int i;
  float error;
  MSG_TYPE temp;
  srand(RAND_SEED);

  //cout << "[ApplyError] Error chance: " << prob_error << endl;

  for (i=FULL_PACK_SIZE-1;i>=0;i--) {
    
    error = (((float)(rand()%10000))/10000);
    if (error < prob_error) {
      // get the current bit mask
      temp = 0x1 << i;

      // inverted the current bit
      package = package ^ temp;
    }
      
    //cout << "Erro: " << error << endl;
  }

  return package;
}

void message_pretty_print (MSG_TYPE msg, ID_TYPE id_size) {
  cout << "Message: " << (EXTRACT_MSG_FROM_MSG(msg, id_size)) << " | " << (EXTRACT_ID_FROM_MSG(msg, id_size)) << endl;
}

void alarm_dummy (int dummy) {};

float stopnwait_eff (FULL_PACK_SIZE,PACK_ID_SIZE,CRC_SIZE,PROB_ERROR, MAX_DELAY) {
  float no, na, nf, pf, t_total, r;

  no = (FULL_PACK_SIZE - PACK_ID_SIZE - CRC_SIZE);
  na = no;
  nf = FULL_PACK_SIZE;
  t_total = (2 * MAX_DELAY);
  r = (FULL_PACK_SIZE/MAX_DELAY);
  pf = PROB_ERROR;
  
  efficiencySW = ( (1 - ( no / nf ) ) / (1 + ( na / nf ) + ( (t_total * r) / nf ) ) ) * (1 - pf);

  return efficiencySW;
}

float gobackn_eff (FULL_PACK_SIZE,PACK_ID_SIZE,CRC_SIZE,PROB_ERROR, WINDOW_SIZE) {
  float no, nf, pf, ws;

  no = (FULL_PACK_SIZE - PACK_ID_SIZE - CRC_SIZE);
  nf = FULL_PACK_SIZE;
  pf = PROB_ERROR;
  ws = WINDOW_SIZE;

  efficiencyGB = ( (1 - ( no / nf) ) / (1 + (ws - 1) * pf) ) * (1 - pf)

  return efficiencyGB;
}

float selectiverepeat_eff (FULL_PACK_SIZE,PACK_ID_SIZE,CRC_SIZE,PROB_ERROR) {
  float no, nf, pf;

  no = (FULL_PACK_SIZE - PACK_ID_SIZE - CRC_SIZE);
  nf = FULL_PACK_SIZE;
  pf = PROB_ERROR;
  
  efficiencySR = ( (1 - ( no / nf ) )  * (1 - pf);

  return efficiencySR;
}
