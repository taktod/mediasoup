#define MS_CLASS "DepTimer"

#include "DepTimer.hpp"
#include "Logger.hpp"

timeval_t startTime;

void DepTimer::ClassInit() {
  gettimeofday(&startTime, NULL);
}

void DepTimer::ClassDestroy() {
  // 特にすることはない
}

uint64_t DepTimer::GetTime() {
  timeval_t currentTime;
  gettimeofday(&currentTime, NULL);
  time_t diffsec = difftime(currentTime.tv_sec, startTime.tv_sec);
  suseconds_t diffsub = currentTime.tv_usec - startTime.tv_usec;
  double milisec = diffsec * 1e3 + diffsub * 1e-3;
  return (uint64_t)(milisec);
}
