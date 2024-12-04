#include "codin.h"

typedef void (*Thread_Proc)(rawptr);

#ifdef linux
  #include "thread_linux.h"
#endif

internal Tid get_thread_id() {
  return syscall(SYS_gettid);
}

internal OS_Result_Tid create_thread(Thread_Proc proc, rawptr user_data) {
  return _create_thread(proc, user_data, 2<<12, 2<<12);
}

// internal b8 join_thread(Tid thread) { return _join_thread(thread); }
