#include "syscalls_linux_amd64.h"
#include "codin.h"
#include "os.h"

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

#define EPERM    1 /* Operation not permitted */
#define ENOENT   2 /* No such file or directory */
#define ESRCH    3 /* No such process */
#define EINTR    4 /* Interrupted system call */
#define EIO      5 /* I/O error */
#define ENXIO    6 /* No such device or address */
#define E2BIG    7 /* Argument list too long */
#define ENOEXEC  8 /* Exec format error */
#define EBADFD   9 /* Bad file number */
#define ECHILD  10 /* No child processes */
#define EAGAIN  11 /* Try again */
#define ENOMEM  12 /* Out of memory */
#define EACCES  13 /* Permission denied */
#define EFAULT  14 /* Bad address */
#define ENOTBLK 15 /* Block device required */
#define EBUSY   16 /* Device or resource busy */
#define EEXIST  17 /* File exists */
#define EXDEV   18 /* Cross-device link */
#define ENODEV  19 /* No such device */
#define ENOTDIR 20 /* Not a directory */
#define EISDIR  21 /* Is a directory */
#define EINVAL  22 /* Invalid argument */
#define ENFILE  23 /* File table overflow */
#define EMFILE  24 /* Too many open files */
#define ENOTTY  25 /* Not a typewriter */
#define ETXTBSY 26 /* Text file busy */
#define EFBIG   27 /* File too large */
#define ENOSPC  28 /* No space left on device */
#define ESPIPE  29 /* Illegal seek */
#define EROFS   30 /* Read-only file system */
#define EMLINK  31 /* Too many links */
#define EPIPE   32 /* Broken pipe */
#define EDOM    33 /* Math argument out of domain of func */
#define ERANGE  34 /* Math result not representable */

internal OS_Error __errno_unwrap(i32 errno) {
  if (errno < 0) {
    errno = -errno;
  }
  switch (errno) {
  case 0:
    return OSE_None;
  case EISDIR:
    return OSE_Is_Dir;
  case EPERM:
  case EACCES:
    return OSE_No_Perm;
  case ECHILD:
    return OSE_No_Child;
  case EINVAL:
    return OSE_Bad_Arguments;
  case EBADFD:
    return OSE_Bad_Fd;
  case ENOENT:
    return OSE_No_File;
  case EAGAIN:
    return OSE_Block;
  }
  return OSE_Other;
}

#define syscall_or_return(...)                                                 \
  ({                                                                           \
    isize ret = syscall(__VA_ARGS__);                                          \
    if (ret < 0) {                                                             \
      result.err = __errno_unwrap(ret);                                        \
      if (result.err) {                                                        \
        return result;                                                         \
      }                                                                        \
    }                                                                          \
    ret;                                                                       \
  })

#define syscall_or_return_err(...)                                             \
  ({                                                                           \
    isize ret = syscall(__VA_ARGS__);                                          \
    if (ret < 0) {                                                             \
      OS_Error err = __errno_unwrap(ret);                                      \
      if (err) {                                                               \
        return err;                                                            \
      }                                                                        \
    }                                                                          \
    ret;                                                                       \
  })
