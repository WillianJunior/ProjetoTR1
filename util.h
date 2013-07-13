#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "basic_types.h"

MSG_TYPE crc (MSG_TYPE package);

MSG_TYPE apply_error (MSG_TYPE package, float prob_error);

#endif