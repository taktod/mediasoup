#ifndef MS_DEP_TIMER_HPP
#define MS_DEP_TIMER_HPP

// windowsだと別のを利用しないといけない。
// とりあえず面倒なので、今回はlinux(MacOSのやつのみ)
#include "common.hpp"
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>

typedef struct timeval timeval_t;

class DepTimer
{
public:
  static void ClassInit();
  static void ClassDestroy();
  static uint64_t GetTime();
};

#endif
