#include "time.h"

#include "runtime_linux.h"

[[nodiscard]]
extern Timestamp time_now() {
  struct {
    i64 seconds;
    i64 nanoseconds;
  } _time = {0};
  u64 status = syscall(SYS_clock_gettime, 0, &_time);
  assert(status == 0);
  return _time.nanoseconds + Second * _time.seconds;
}

extern void time_sleep(Duration duration) {
  struct {
    i64 seconds;
    i64 nanoseconds;
  } _time = {
    .seconds     = duration / Second,
    .nanoseconds = duration % Second,
  };
  u64 status = syscall(SYS_nanosleep, &_time, nil);
  assert(status == 0);
}
