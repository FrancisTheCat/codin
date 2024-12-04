#include "codin.h"

#define atomic_compare_exchange(a, expected, value) __extension__ ({                            \
  __auto_type __atomic_compare_exchange_ptr = (a);                                              \
  __typeof__ ((void)0, *__atomic_compare_exchange_ptr) __atomic_compare_exchange_tmp = (value); \
  __atomic_compare_exchange (__atomic_compare_exchange_ptr, (expected),                         \
                             &__atomic_compare_exchange_tmp, 0, (5), (5));                      \
})

typedef enum {
  MUTEX_Unlocked = 0,
  MUTEX_Locked   = 1,
  // MUTEX_Waiting  = 2,
} _Mutex_State;

typedef struct {
  _Mutex_State impl;
} Mutex;

// internal i64 futex(u32 *u_addr, i32 futex_op, u32 value) {
//   return syscall(SYS_futex, FUTEX_PRIVATE_FLAG, u_addr, futex_op, value, nil,
//                  nil, 0);
// }

internal void mutex_lock(Mutex *m) {
  _Mutex_State expected = MUTEX_Unlocked;
  while (true) {
    atomic_compare_exchange(&m->impl, &expected, MUTEX_Locked);
    if (expected == MUTEX_Unlocked) {
      break;
    }
    expected = MUTEX_Unlocked;
  }
}

internal b8 mutex_try_lock(Mutex *m) {
  _Mutex_State expected = MUTEX_Unlocked;
  atomic_compare_exchange(&m->impl, &expected, MUTEX_Locked);
  return expected == MUTEX_Unlocked;
}

internal void mutex_unlock(Mutex *m) {
  _Mutex_State expected = MUTEX_Locked;
  atomic_compare_exchange(&m->impl, &expected, MUTEX_Unlocked);
}

#define MUTEX_GUARD(mutex, block)                                              \
  {                                                                            \
    mutex_lock(mutex);                                                         \
    { block; }                                                                 \
    mutex_unlock(mutex);                                                       \
  }
