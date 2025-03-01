#include "syscalls.h"

extern long __syscall6(long syscall, long arg0, long arg1, long arg2, long arg3, long arg4, long arg5);
extern long __syscall5(long syscall, long arg0, long arg1, long arg2, long arg3, long arg4           );
extern long __syscall4(long syscall, long arg0, long arg1, long arg2, long arg3                      );
extern long __syscall3(long syscall, long arg0, long arg1, long arg2                                 );
extern long __syscall2(long syscall, long arg0, long arg1                                            );
extern long __syscall1(long syscall, long arg0                                                       );
extern long __syscall0(long syscall                                                                  );

extern long __clone3(void*, long, long(*)(void*), void*);
extern long __go_unmap_urself(void*, long, long);

extern long syscall(long syscall_number, ...);

extern void __stack_chk_fail();
