#include "codin.h"

typedef Slice(String) Process_Args;
typedef Slice(String) Process_Env;

internal Process_Args os_args;
internal Process_Env  os_env;

[[nodiscard]]
internal String get_env(String key) {
  slice_iter(os_env, str, _i, {
    isize i = string_index(*str, key);
    if (i == -1 || i + key.len >= str->len || str->data[i + key.len] != '=') {
      continue;
    }
    return slice_range(*str, key.len + 1, str->len);
  })
  return (String) {.len = 0};
}

#define stdin __stdin
#define stdout __stdout
#define stderr __stderr

Reader __stdin;
Writer __stdout;
Writer __stderr;

typedef enum {
  OSE_None = 0,
  OSE_No_Perm,
  OSE_No_Child,
  OSE_No_File,
  OSE_Bad_Fd,
  OSE_Bad_Arguments,
  OSE_Is_Dir,
  OSE_No_Mem,
  OSE_Partial_Write,
  OSE_Partial_Read,
  OSE_Other,
} OS_Error;

ENUM_TO_STRING_PROC_DECL(OS_Error, err) {
  switch (err) {
  case OSE_None:
    return LIT("OSE_None");
  case OSE_No_Perm:
    return LIT("OSE_No_Perm");
  case OSE_No_Child:
    return LIT("OSE_No_Child");
  case OSE_No_File:
    return LIT("OSE_No_File");
  case OSE_Bad_Fd:
    return LIT("OSE_Bad_Fd");
  case OSE_Bad_Arguments:
    return LIT("OSE_Bad_Arguments");
  case OSE_Is_Dir:
    return LIT("OSE_Is_Dir");
  case OSE_No_Mem:
    return LIT("OSE_No_Mem");
  case OSE_Partial_Write:
    return LIT("OSE_Partial_Write");
  case OSE_Partial_Read:
    return LIT("OSE_Partial_Read");
  case OSE_Other:
    return LIT("OSE_Other");
  }
  return LIT("OSE_INVALID");
}

typedef struct {
  String full_path;
  String name;
  isize size;
  struct Time acces_time;
  struct Time creation_time;
  struct Time modification_time;
  b8 is_dir;
} File_Info;

typedef enum {
  FP_Read = 0b0001,
  FP_Write = 0b0010,
  FP_Create = 0b0100,
  FP_Truncate = 0b1000,
  FP_Read_Write = 0b0011,
} File_Permission;

typedef Slice(File_Info) Directory;

typedef Result(rawptr,     OS_Error) OS_Result_Ptr;
typedef Result(isize,      OS_Error) OS_Result_Int;
typedef Result(Fd,         OS_Error) OS_Result_Fd;
typedef Result(Tid,        OS_Error) OS_Result_Tid;
typedef Result(Pid,        OS_Error) OS_Result_Pid;
typedef Result(Directory,  OS_Error) OS_Result_Dir;
typedef Result(Byte_Slice, OS_Error) OS_Result_Bytes;
typedef Result(String,     OS_Error) OS_Result_String;

internal OS_Result_Int _read(Fd fd, Byte_Slice buf);
internal OS_Result_Dir _read_dir(Fd fd, Allocator allocator);

internal OS_Result_Int _write(Fd fd, Byte_Slice buf);

internal OS_Result_Fd _open_file(String path, isize permissions);
internal OS_Result_Fd _open_file_at(Fd dir, String path, isize permissions);

internal OS_Error _close_file(Fd fd);

internal OS_Error _stat(Fd fd, File_Info *fi);

internal OS_Error _remove(String path);
internal OS_Error _remove_at(Fd dir, String path);
internal OS_Error _remove_dir(String path);
internal OS_Error _remove_dir_at(Fd dir, String path);

internal b8 _file_exists(String path);

typedef struct {
  Process_Args args;
  Process_Env  env;
  Fd           stdin;
  Fd           stdout;
  Fd           stderr;
} Process_Creation_Args;

internal OS_Result_Pid _create_process(String path, Process_Creation_Args const *args);

#ifdef linux
  #include "os_linux.h"
#endif

[[nodiscard]]
internal OS_Result_Int file_read(Fd fd, Byte_Slice buf) {
  return _read(fd, buf);
}

internal OS_Result_Int file_write(Fd fd, Byte_Slice buf) {
  return _write(fd, buf);
}

internal OS_Error file_close(Fd fd) { return _close_file(fd); }

internal OS_Result_Fd file_open(String path, isize permissions) {
  return _open_file(path, permissions);
}

internal OS_Error file_stat(Fd file, File_Info *info) {
  return _stat(file, info);
}

[[nodiscard]]
internal OS_Result_Bytes read_entire_file_fd(Fd fd, Allocator allocator) {
  OS_Result_Bytes result = {0};
  File_Info fi;
  result.err = _stat(fd, &fi);
  if (result.err) {
    return result;
  }
  slice_init(&result.value, fi.size, allocator);

  result.value.len = or_return_err(file_read(fd, result.value));

  return result;
}

[[nodiscard]]
internal OS_Result_Bytes read_entire_file_path(String path,
                                               Allocator allocator) {
  OS_Result_Bytes result = {0};
  Fd file = or_return_err(file_open(path, FP_Read));
  result.value = or_return_err(read_entire_file_fd(file, allocator));
  result.err = file_close(file);
  if (result.err) {
    slice_delete(result.value, allocator);
  }
  return result;
}

internal OS_Error write_entire_file_fd(Fd fd, Byte_Slice data) {
  isize n = or_return_err_v(_write(fd, data));
  return n == data.len ? OSE_None : OSE_Partial_Write;
}

internal OS_Error write_entire_file_path(String path, Byte_Slice data) {
  Fd fd = or_return_err_v(file_open(path, FP_Write | FP_Create | FP_Truncate));
  OS_Error err = write_entire_file_fd(fd, data);
  if (err) {
    return err;
  }
  err = file_close(fd);
  return err;
}

[[nodiscard]]
internal OS_Result_Dir read_directory_fd(Fd fd, Allocator allocator) {
  return _read_dir(fd, allocator);
}

[[nodiscard]]
internal OS_Result_Dir read_directory_path(String path, Allocator allocator) {
  OS_Result_Dir result = {0};
  Fd fd = or_return_err(file_open(path, FP_Read));
  result = _read_dir(fd, allocator);
  if (result.err) {
    file_close(fd);
    return result;
  }
  result.err = file_close(fd);
  if (result.err) {
    slice_delete(result.value, allocator);
  }
  return result;
}

internal void directory_delete(Directory dir, Allocator allocator) {
  slice_iter(dir, file, i, { string_delete(file->name, allocator); });
  slice_delete(dir, allocator);
}

internal OS_Result_Pid process_create(String path, Process_Creation_Args const *args) {
  return _create_process(path, args);
}

internal OS_Error process_wait(Pid pid) {
  return _wait_process(pid);
}

internal Maybe_Int writer_file_proc(rawptr handle, Byte_Slice data) {
  return result_to_maybe(Maybe_Int, _write(*(Fd *)&handle, data));
}

[[nodiscard]]
internal Writer writer_from_handle(Fd handle) {
  Writer w;
  w.data = *(rawptr *)&handle;
  w.proc = writer_file_proc;
  return w;
}

internal Maybe_Int file_reader_proc(rawptr handle, Byte_Slice buf) {
  assert(buf.data);
  assert(buf.len);
  Fd fd = (Fd)handle;
  return result_to_maybe(Maybe_Int, file_read(fd, buf));
}

[[nodiscard]]
internal Reader reader_from_handle(Fd handle) {
  return (Reader){
      .data = *(rawptr *)&handle,
      .proc = file_reader_proc,
  };
}

internal void __os_init() {
  stdin = reader_from_handle(0);
  stdout = writer_from_handle(1);
  stderr = writer_from_handle(2);
}

[[nodiscard]]
internal b8 file_exists(String path) { return _file_exists(path); }

internal diverging os_exit(isize code) {
  _os_exit(code);
}

[[nodiscard]]
internal rawptr os_allocate_pages(isize n) {
  return (rawptr)syscall(
    SYS_mmap,
    0,
    n * OS_PAGE_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_PRIVATE | MAP_ANONYMOUS,
    0,
    0
  );
}

internal b8 os_deallocate_pages(rawptr p, isize n) {
  return syscall(SYS_munmap, p, n * OS_PAGE_SIZE) == 0;
}
