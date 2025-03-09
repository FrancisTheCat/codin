#include "os.h"

#include "runtime_linux.h"
#include "strings.h"

Reader std_in;
Writer std_out;
Writer std_err;

String_Slice os_args;
String_Slice os_env;

extern String get_env(String key) {
  slice_iter(os_env, str, _i, {
    isize i = string_index(*str, key);
    if (i == -1 || i + key.len >= str->len || str->data[i + key.len] != '=') {
      continue;
    }
    return slice_range(*str, key.len + 1, str->len);
  })
  return (String) {0};
}

extern OS_Result_Bytes read_entire_file_fd(Fd fd, Allocator allocator) {
  OS_Result_Bytes result = {0};
  File_Info fi;
  result.err = file_stat(fd, &fi);
  if (result.err) {
    return result;
  }
  slice_init(&result.value, fi.size, allocator);

  result.value.len = or_return_err(file_read(fd, result.value));

  return result;
}

extern OS_Result_Bytes read_entire_file_path(String path, Allocator allocator) {
  OS_Result_Bytes result = {0};
  Fd file = or_return_err(file_open(path, FP_Read));
  result.value = or_return_err(read_entire_file_fd(file, allocator));
  result.err = file_close(file);
  if (result.err) {
    slice_delete(result.value, allocator);
  }
  return result;
}

extern OS_Error write_entire_file_fd(Fd fd, Byte_Slice data) {
  isize n = or_return_err_v(file_write(fd, data));
  return n == data.len ? OSE_None : OSE_Partial_Write;
}

extern OS_Error write_entire_file_path(String path, Byte_Slice data) {
  Fd fd = or_return_err_v(file_open(path, FP_Write | FP_Create | FP_Truncate));
  OS_Error err = write_entire_file_fd(fd, data);
  if (err) {
    return err;
  }
  err = file_close(fd);
  return err;
}

extern OS_Result_Dir directory_read_path(String path, Allocator allocator) {
  OS_Result_Dir result = {0};
  Fd fd = or_return_err(file_open(path, FP_Read));
  result = directory_read_fd(fd, allocator);
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

extern void directory_delete(Directory dir, Allocator allocator) {
  slice_iter(dir, file, i, { string_delete(file->name, allocator); });
  slice_delete(dir, allocator);
}

extern void __os_init() {
  std_in  = reader_from_handle(FD_STDIN);
  std_out = writer_from_handle(FD_STDOUT);
  std_err = writer_from_handle(FD_STDERR);
}

extern Writer writer_from_handle(Fd handle) {
  Writer w;
  w.data = *(rawptr *)&handle;
  w.proc = writer_file_proc;
  return w;
}

extern Maybe_Int file_reader_proc(rawptr handle, Byte_Slice buf) {
  assert(buf.data);
  assert(buf.len);
  Fd fd = (Fd)handle;
  return result_to_maybe(Maybe_Int, file_read(fd, buf));
}

extern Reader reader_from_handle(Fd handle) {
  return (Reader){
      .data = *(rawptr *)&handle,
      .proc = file_reader_proc,
  };
}

extern rawptr os_pages_allocate(isize n) {
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

extern b8 os_pages_deallocate(rawptr p, isize n) {
  return syscall(SYS_munmap, p, n * OS_PAGE_SIZE) == 0;
}

extern Maybe_Int writer_file_proc(rawptr handle, Byte_Slice data) {
  return result_to_maybe(Maybe_Int, file_write(*(Fd *)&handle, data));
}

