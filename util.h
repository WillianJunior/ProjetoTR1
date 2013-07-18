#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basic_types.h"

#define EXTRACT_SLAST(A, S) ((A << (FULL_PACK_SIZE - S - CRC_SIZE)) >> (FULL_PACK_SIZE - S))
#define EXTRACT_RNEXT(A) (A >> CRC_SIZE)
#define EXTRACT_MSG(A, S) (A >> (CRC_SIZE + S))

#define EXTRACT_MSG_FROM_MSG(A,S) (A >> (S + CRC_SIZE))
#define EXTRACT_ID_FROM_MSG(A, S) ((A << (FULL_PACK_SIZE - S - CRC_SIZE)) >> (FULL_PACK_SIZE - S))

#define EXTRACT_ACK_TYPE_FROM_ACK(A, S) (A >> (CRC_SIZE + S))
#define EXTRACT_ID_FROM_ACK(A, S) ((A << (FULL_PACK_SIZE - S - CRC_SIZE)) >> (FULL_PACK_SIZE - S))

MSG_TYPE crc (MSG_TYPE package);

MSG_TYPE apply_error (MSG_TYPE package, float prob_error);

void message_pretty_print (MSG_TYPE msg, ID_TYPE id_size);

void alarm_dummy (int dummy);

#endif

