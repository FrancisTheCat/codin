#include "codin.h"

extern isize __clone3(void*, isize, isize(*)(void*), void*);
extern isize __go_unmap_urself(void*, isize, isize);

typedef struct {
  void *stack;
  void *tls;
  isize stack_size;
  isize tls_size;
  void *arg;
  Thread_Proc func;
  isize _padding[2];
} __Thread_Creation_Context;

internal isize __thread_start_proc(void *data) {
  __Thread_Creation_Context ctx = *(__Thread_Creation_Context *)data;

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

internal OS_Result_Tid _create_thread(Thread_Proc func, void* arg, isize stack_size, isize tls_size) {
  OS_Result_Tid result = {0};
  void *stack = (void *)syscall_or_return(SYS_mmap, 0, stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  void *tls   = (void *)syscall_or_return(SYS_mmap, 0, tls_size,   PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  
  struct clone_args {
     u64 flags;        /* Flags bit mask */
     u64 pidfd;        /* Where to store PID file descriptor
                          (int *) */
     u64 child_tid;    /* Where to store child TID,
                          in child's memory (pid_t *) */
     u64 parent_tid;   /* Where to store child TID,
                          in parent's memory (pid_t *) */
     u64 exit_signal;  /* Signal to deliver to parent on
                          child termination */
     u64 stack;        /* Pointer to lowest byte of stack */
     u64 stack_size;   /* Size of stack */
     u64 tls;          /* Location of new TLS */
     u64 set_tid;      /* Pointer to a pid_t array
                          (since Linux 5.5) */
     u64 set_tid_size; /* Number of elements in set_tid
                          (since Linux 5.5) */
     u64 cgroup;       /* File descriptor for target cgroup
                          of child (since Linux 5.7) */
  };

  struct clone_args args = {0};
  args.flags      = CLONE_THREAD | CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_SYSVSEM;
  args.stack      = (long)stack;
  args.stack_size = stack_size;

  __Thread_Creation_Context *ctx = (__Thread_Creation_Context *)((uintptr)(tls));
  ctx->stack      = (void *)args.stack;
  ctx->stack_size = stack_size;
  ctx->tls        = (void *)tls;
  ctx->tls_size   = tls_size;
  ctx->arg        = arg;
  ctx->func       = func;

  result.value = __clone3(&args, size_of(args), __thread_start_proc, ctx);
  if (result.value == 0) {
    __builtin_trap();
  }
  if (result.value < 0) {
    result.err = __errno_unwrap(result.value);
  }
  return result;
}

