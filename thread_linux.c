#include "thread.h"
#include "syscalls_linux_amd64.h"
#include "runtime_linux.h"

extern isize __clone3(rawptr, isize, isize(*)(rawptr), rawptr);
extern isize __go_unmap_urself(rawptr, isize, isize);

typedef struct {
  rawptr      stack;
  rawptr      tls;
  isize       stack_size;
  isize       tls_size;
  rawptr      arg;
  Thread_Proc func;
  isize       _padding[2];
} _Thread_Creation_Context;

extern Tid get_thread_id() {
  return syscall(SYS_gettid);
}

internal isize _thread_start_proc(void *data) {
  _Thread_Creation_Context ctx = *(_Thread_Creation_Context *)data;

  *((uintptr *)((uintptr)ctx.tls + ctx.tls_size - 2 * size_of(rawptr))) = (uintptr)ctx.tls + ctx.tls_size - 2 * size_of(rawptr);
  isize err = syscall(SYS_arch_prctl, ARCH_SET_FS, (uintptr)ctx.tls + ctx.tls_size - 2 * size_of(rawptr));
  assert(err == 0);

  __thread_init();
  ctx.func(ctx.arg);
  __thread_cleanup();

  syscall(SYS_munmap, ctx.tls, ctx.tls_size);
  __go_unmap_urself(ctx.stack, ctx.stack_size, 0);

  __builtin_trap();
}

extern OS_Result_Tid thread_create(Thread_Proc func, void* arg, isize stack_size, isize tls_size) {
  OS_Result_Tid result = {0};
  rawptr stack = (rawptr)syscall_or_return(SYS_mmap, 0, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  rawptr tls   = (rawptr)syscall_or_return(SYS_mmap, 0, tls_size,   PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  
  struct clone_args {
     u64    flags;        /* Flags bit mask */
     u64    pidfd;        /* Where to store PID file descriptor (int *) */
     u64    child_tid;    /* Where to store child TID, in child's memory (pid_t *) */
     u64    parent_tid;   /* Where to store child TID, in parent's memory (pid_t *) */
     u64    exit_signal;  /* Signal to deliver to parent on child termination */
     rawptr stack;        /* Pointer to lowest byte of stack */
     u64    stack_size;   /* Size of stack */
     rawptr tls;          /* Location of new TLS */
     u64    set_tid;      /* Pointer to a pid_t array (since Linux 5.5) */
     u64    set_tid_size; /* Number of elements in set_tid (since Linux 5.5) */
     u64    cgroup;       /* File descriptor for target cgroup of child (since Linux 5.7) */
  };

  struct clone_args args = {0};
  args.flags      = CLONE_THREAD | CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_SYSVSEM;
  args.stack      = stack;
  args.stack_size = stack_size;

  _Thread_Creation_Context *ctx = (_Thread_Creation_Context *)tls;
  ctx->stack      = args.stack;
  ctx->stack_size = stack_size;
  ctx->tls        = tls;
  ctx->tls_size   = tls_size;
  ctx->arg        = arg;
  ctx->func       = func;

  result.value = __clone3(&args, size_of(args), _thread_start_proc, ctx);
  if (result.value == 0) {
    __builtin_trap();
  }
  if (result.value < 0) {
    result.err = __errno_unwrap(result.value);
  }
  return result;
}

// extern OS_Error _thread_join(Tid tid) {
//   syscall_or_return_err(SYS_wait4, tid, nil, __WCLONE, nil);
//   return OSE_None;
// }
