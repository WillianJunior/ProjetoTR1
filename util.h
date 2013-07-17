#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basic_types.h"

#define EXTRACT_SLAST(A, S) (A << (FULL_PACK_SIZE - S - CRC_SIZE)) >> (FULL_PACK_SIZE - S)
#define EXTRACT_RNEXT(A) A >> CRC_SIZE
#define EXTRACT_MSG(A, S) A >> (CRC_SIZE + S)

MSG_TYPE crc (MSG_TYPE package);

MSG_TYPE apply_error (MSG_TYPE package, float prob_error);

void alarm_dummy (int dummy);

#endif