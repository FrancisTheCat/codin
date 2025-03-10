#include "syscalls_linux_amd64.h"

extern long __syscall6(long syscall, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);
extern long __syscall5(long syscall, long arg0, long arg1, long arg2, long arg3, long arg4           );
extern long __syscall4(long syscall, long arg0, long arg1, long arg2, long arg3                      );
extern long __syscall3(long syscall, long arg0, long arg1, long arg2                                 );
extern long __syscall2(long syscall, long arg0, long arg1                                            );
extern long __syscall1(long syscall, long arg0                                                       );
extern long __syscall0(long syscall                                                                  );

extern long __clone3(void*, long, long(*)(void*), void*);
extern long __go_unmap_urself(void*, long, long);

extern long syscall(long syscall_number, ...) {
  long ret;
  __builtin_va_list args;
  __builtin_va_start(args, syscall_number);

  long arg0, arg1, arg2, arg3, arg4, arg5;

  __builtin_va_end(args);

  switch (syscall_number) {
  case SYS_fork:
  case SYS_getgid:
  case SYS_gettid:
  case SYS_sched_yield:
    ret = __syscall0(syscall_number);
    break;
  case SYS_exit:
  case SYS_exit_group:
  case SYS_close:
  case SYS_time:
  case SYS_unlink:
    arg0 = __builtin_va_arg(args, long);
    ret  = __syscall1(syscall_number, arg0);
    break;
  case SYS_arch_prctl:
  case SYS_nanosleep:
  case SYS_munmap:
  case SYS_listen:
  case SYS_fstat:
  case SYS_getcwd:
  case SYS_dup2:
  case SYS_clock_settime:
  case SYS_clock_gettime:
  case SYS_clock_getres:
  case SYS_shutdown:
  case SYS_memfd_create:
  case SYS_ftruncate:
  case SYS_pipe2:
  case SYS_kill:
  case SYS_tkill:
    arg0 = __builtin_va_arg(args, long);
    arg1 = __builtin_va_arg(args, long);
    ret  = __syscall2(syscall_number, arg0, arg1);
    break;
  case SYS_read:
  case SYS_write:
  case SYS_socket:
  case SYS_accept:
  case SYS_bind:
  case SYS_open:
  case SYS_getdents:
  case SYS_execve:
  case SYS_unlinkat:
  case SYS_mprotect:
  case SYS_connect:
  case SYS_sendmsg:
  case SYS_recvmsg:
  case SYS_fcntl:
  case SYS_tgkill:
  case SYS_poll:
    arg0 = __builtin_va_arg(args, long);
    arg1 = __builtin_va_arg(args, long);
    arg2 = __builtin_va_arg(args, long);
    ret = __syscall3(syscall_number, arg0, arg1, arg2);
    break;
  case SYS_openat:
  case SYS_wait4:
    arg0 = __builtin_va_arg(args, long);
    arg1 = __builtin_va_arg(args, long);
    arg2 = __builtin_va_arg(args, long);
    arg3 = __builtin_va_arg(args, long);
    ret = __syscall4(syscall_number, arg0, arg1, arg2, arg3);
    break;
  case SYS_waitid:
  case SYS_setsockopt:
    arg0 = __builtin_va_arg(args, long);
    arg1 = __builtin_va_arg(args, long);
    arg2 = __builtin_va_arg(args, long);
    arg3 = __builtin_va_arg(args, long);
    arg4 = __builtin_va_arg(args, long);
    ret = __syscall5(syscall_number, arg0, arg1, arg2, arg3, arg4);
    break;
  case SYS_mmap:
  case SYS_sendto:
  case SYS_recvfrom:
  case SYS_futex:
    arg0 = __builtin_va_arg(args, long);
    arg1 = __builtin_va_arg(args, long);
    arg2 = __builtin_va_arg(args, long);
    arg3 = __builtin_va_arg(args, long);
    arg4 = __builtin_va_arg(args, long);
    arg5 = __builtin_va_arg(args, long);
    ret = __syscall6(syscall_number, arg0, arg1, arg2, arg3, arg4, arg5);
    break;
  default:
    __syscall3(SYS_write, 2, (long)(char*)"INVALID SYSCALL\n", sizeof("INVALID SYSCALL\n"));
    __builtin_trap();
  }

  return ret;
}

extern void __stack_chk_fail() {
  syscall(SYS_write, 2, "stack smashing detected\n", 24);
  __builtin_trap();
}
