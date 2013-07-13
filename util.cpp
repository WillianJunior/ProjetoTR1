#include "util.h"

//#include <iostream>
//using namespace std;

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
    
    error = (((float)(rand()%100))/100);
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
