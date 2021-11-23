#ifndef UTILITIES_ROBOT_H
#define UTILITIES_ROBOT_H

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <sstream>
#include <time.h>

#include "typedef.h"

#define MAX_LOG_FILE_SIZE     10000000

#ifdef WIN32
void gettimeofday(struct timeval *tv, void* tz);
#endif
void setSystemTime(unsigned int seconds);

ROBOT_INT64 CycleClock_Now();


ROBOT_INT64 UsecToCycles(ROBOT_INT64 usec);

double WallTime_Now();

#endif