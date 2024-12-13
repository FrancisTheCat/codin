#include "codin.h"

#define EPERM    1	/* Operation not permitted */
#define ENOENT   2	/* No such file or directory */
#define ESRCH    3	/* No such process */
#define EINTR    4	/* Interrupted system call */
#define EIO      5	/* I/O error */
#define ENXIO    6	/* No such device or address */
#define E2BIG    7	/* Argument list too long */
#define ENOEXEC  8	/* Exec format error */
#define EBADF    9	/* Bad file number */
#define ECHILD  10	/* No child processes */
#define EAGAIN  11	/* Try again */
#define ENOMEM  12	/* Out of memory */
#define EACCES  13	/* Permission denied */
#define EFAULT  14	/* Bad address */
#define ENOTBLK 15	/* Block device required */
#define EBUSY   16	/* Device or resource busy */
#define EEXIST  17	/* File exists */
#define EXDEV   18	/* Cross-device link */
#define ENODEV  19	/* No such device */
#define ENOTDIR 20	/* Not a directory */
#define EISDIR  21	/* Is a directory */
#define EINVAL  22	/* Invalid argument */
#define ENFILE  23	/* File table overflow */
#define EMFILE  24	/* Too many open files */
#define ENOTTY  25	/* Not a typewriter */
#define ETXTBSY 26	/* Text file busy */
#define EFBIG   27	/* File too large */
#define ENOSPC  28	/* No space left on device */
#define ESPIPE  29	/* Illegal seek */
#define EROFS   30	/* Read-only file system */
#define EMLINK  31	/* Too many links */
#define EPIPE   32	/* Broken pipe */
#define EDOM    33	/* Math argument out of domain of func */
#define ERANGE  34	/* Math result not representable */

#define	EBADFD  77	/* File descriptor in bad state */

#define O_ACCMODE  0003
#define O_RDONLY   00
#define O_WRONLY   01
#define O_RDWR     02
#define O_CREAT    0100
#define O_EXCL     0200
#define O_NOCTTY   0400
#define O_TRUNC    01000
#define O_APPEND   02000
#define O_NONBLOCK 04000
#define O_NDELAY   O_NONBLOCK
#define O_SYNC     04010000
#define O_FSYNC    O_SYNC
#define O_ASYNC    020000
#define O_CLOEXEC	 02000000

#define S_IFDIR 0040000

#define AT_REMOVEDIR 0x200

struct stat {
    usize st_dev;
    usize st_ino;
    usize st_nlink;
    u32   st_mode;
    u32   st_uid;
    u32   st_gid;
    i32   __pad0;
    usize st_rdev;
    isize st_size;
    isize st_blksize;
    isize st_blocks;
    isize st_atime;
    usize st_atimensec;
    isize st_mtime;
    usize st_mtimensec;
    isize st_ctime;
    usize st_ctimensec;
    isize __reserved[3];
};

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
  }
  return OSE_Other;
}

internal OS_Error _close_file(Fd fd) {
  syscall_or_return_err(SYS_close, fd);
  return OSE_None;
}

internal OS_Result_Fd _open_file(String path, isize permissions) {
  OS_Result_Fd result = {0};
  usize flags = 0;
  usize mode  = 0;

#define FLAG(perm, o)                                                          \
  if (perm & permissions) {                                                    \
    flags |= o;                                                                \
  }

  FLAG(FP_Read,     O_RDONLY)
  FLAG(FP_Write,    O_RDWR)
  FLAG(FP_Create,   O_CREAT)
  FLAG(FP_Truncate, O_TRUNC)

  if (permissions & FP_Create) {
    mode = 00444 | 00222;
  }

#undef FLAG

  cstring cstr = string_to_cstring_clone(path, context.temp_allocator);

  result.value = syscall_or_return(SYS_open, cstr, flags, mode);
  return result;
}

internal OS_Result_Fd _open_file_at(Fd dir, String path, isize permissions) {
  OS_Result_Fd result = {0};
  usize flags = 0;

#define FLAG(perm, o)                                                          \
  if (perm & permissions) {                                                    \
    flags |= o;                                                                \
  }

  FLAG(FP_Write,    O_RDWR)
  FLAG(FP_Create,   O_CREAT)
  FLAG(FP_Truncate, O_TRUNC)

#undef FLAG

  cstring cstr = string_to_cstring_clone(path, context.temp_allocator);

  result.value = syscall_or_return(SYS_openat, dir, cstr, flags);
  return result;
}

internal OS_Error _stat(Fd fd, File_Info *fi) {
  struct stat s;
  syscall_or_return_err(SYS_fstat, fd, &s);

  fi->size = s.st_size;
  fi->acces_time.nsec        = s.st_atimensec + s.st_atime * Second;
  fi->creation_time.nsec     = s.st_ctimensec + s.st_ctime * Second;
  fi->modification_time.nsec = s.st_mtimensec + s.st_mtime * Second;
  fi->is_dir = !!(s.st_mode & S_IFDIR);

  return OSE_None;
}

internal OS_Result_Int _read(Fd fd, Byte_Slice buf) {
  OS_Result_Int result = {0};
  result.value = syscall_or_return(SYS_read, fd, buf.data, buf.len);
  return result;
}

internal OS_Result_Int _write(Fd fd, Byte_Slice buf) {
  OS_Result_Int result = {0};
  result.value = syscall_or_return(SYS_write, fd, buf.data, buf.len);
  return result;
}

internal OS_Result_Dir _read_dir(Fd fd, Allocator allocator) {
  OS_Result_Dir result = {0};

  typedef struct {
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
    char d_name[0];
  } Linux_Dirent;

#define BUF_SIZE 1024

  Byte_Slice buffer;
  slice_init(&buffer, BUF_SIZE, context.temp_allocator);

  File_Info current_file;
  Linux_Dirent *dirent;

  Vector(File_Info) directory;
  vector_init(&directory, 0, 8, allocator);

  for (;;) {
    isize status = syscall_or_return(SYS_getdents, fd, buffer.data, BUF_SIZE);

    if (status == 0) {
      break;
    }

    for (int i = 0; i < status; i += dirent->d_reclen) {
      dirent = (Linux_Dirent *)(buffer.data + i);
      current_file.name = cstring_to_string_clone(dirent->d_name, allocator);
      Fd child_fd =
          or_return_err(_open_file_at(fd, current_file.name, FP_Read));
      if (_stat(child_fd, &current_file)) {
        string_delete(current_file.name, allocator);
      } else {
        vector_append(&directory, current_file);
      }
    }
  }

#undef BUF_SIZE

  result.value = (Directory){.data = directory.data, .len = directory.len};
  return result;
}

internal OS_Error _remove(String path) {
  cstring cpath = string_to_cstring_clone(path, context.temp_allocator);
  syscall_or_return_err(SYS_unlink, cpath);
  return OSE_None;
}

internal OS_Error _remove_at(Fd dir, String path) {
  cstring cpath = string_to_cstring_clone(path, context.temp_allocator);
  syscall_or_return_err(SYS_unlinkat, dir, cpath, 0);
  return OSE_None;
}

internal OS_Error _remove_dir(String path) {
  cstring cpath = string_to_cstring_clone(path, context.temp_allocator);
  syscall_or_return_err(SYS_rmdir, cpath);
  return OSE_None;
}

internal OS_Error _remove_dir_at(Fd dir, String path) {
  cstring cpath = string_to_cstring_clone(path, context.temp_allocator);
  syscall_or_return_err(SYS_unlinkat, dir, cpath, AT_REMOVEDIR);
  return OSE_None;
}

#define DEFAULT_PROCESS_ARGS                                                   \
  (Process_Creation_Args) { .stdin = 0, .stdout = 1, .stderr = 2, .env = os_env }

internal OS_Result_Pid _create_process(String path,
                                       Process_Creation_Args const *args) {
  OS_Result_Pid result = {0};
  int pipe_fds[2];
  syscall(SYS_pipe2, &pipe_fds, O_CLOEXEC);

  Pid pid = syscall_or_return(SYS_fork);
  if (pid) {
    syscall(SYS_close, pipe_fds[1]);
    i64 out;
    isize n = syscall(SYS_read, pipe_fds[0], &out, size_of(out));
    syscall(SYS_close, pipe_fds[0]);
    if (n == 0) {
      result.value = pid;
      return result;
    } else {
      result.err = __errno_unwrap(out);
      return result;
    }
  }

  syscall(SYS_close, pipe_fds[0]);

  syscall(SYS_dup2, args->stdin,  0);
  syscall(SYS_dup2, args->stdout, 1);
  syscall(SYS_dup2, args->stderr, 2);

  cstring c_path = string_to_cstring_clone(path, context.temp_allocator);

  Slice(cstring) c_args;
  slice_init(&c_args, args->args.len + 1, context.temp_allocator);

  Slice(cstring) c_env;
  slice_init(&c_env, args->env.len + 1, context.temp_allocator);

  c_args.data[c_args.len - 1] = nil;
  c_env.data[c_env.len - 1] = nil;

  slice_iter(args->args, arg, i,
             { c_args.data[i] = string_to_cstring_clone(*arg, context.temp_allocator); });

  slice_iter(args->env, e, i,
             { c_env.data[i] = string_to_cstring_clone(*e, context.temp_allocator); });

  i64 execve_result = syscall(SYS_execve, c_path, c_args.data, c_env.data);
  syscall(SYS_write, pipe_fds[1], &execve_result, size_of(execve_result));
  syscall(SYS_exit, 1);
}

internal OS_Error _kill_process(Pid pid) {
  syscall_or_return_err(SYS_kill, (int)pid);
  return OSE_None;
}

internal OS_Error _wait_process(Pid pid) {
  syscall_or_return_err(SYS_wait4, pid, nil, 0, nil);
  return OSE_None;
}

internal diverging _os_exit(isize code) { syscall(SYS_exit_group, (int)code); }

internal b8 _file_exists(String path) {
  struct stat stat;
  return syscall(SYS_stat,
                 string_to_cstring_clone(path, context.temp_allocator),
                 &stat) == 0;
}

internal OS_Result_String _get_current_directory(Allocator allocator) {
  OS_Result_String result = {0};
  Builder buf = vector_make(Builder, 0, 4, allocator);
  loop {
    isize status = syscall(SYS_getcwd, buf.data, buf.cap);
    if (!status) {
      result.err = __errno_unwrap(status);
      if (errno == ERANGE) {
        vector_reserve(&buf, buf.cap * 2);
        continue;
      }
      return result;
    } else {
      buf.len = cstring_len(buf.data);
      result.value = builder_to_string(buf);
      return result;
    }
  }
}

internal OS_Error _set_current_directory(String directory) {
  int status = syscall(SYS_chdir,
          string_to_cstring_clone(directory, context.temp_allocator));
  return __errno_unwrap(status);
}
