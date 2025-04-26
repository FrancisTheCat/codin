#pragma once

#define OS_PAGE_SIZE 4096

#include "codin.h"

#include "io.h"
#include "time.h"

#define FD_STDIN  0
#define FD_STDOUT 1
#define FD_STDERR 2

extern String_Slice os_args;
extern String_Slice  os_env;

[[nodiscard]]
extern String get_env(String key);

extern Reader std_in;
extern Writer std_out;
extern Writer std_err;

#define OS_ERRORS(X)                                                           \
  X(OSE_None)                                                                  \
  X(OSE_No_Perm)                                                               \
  X(OSE_No_Child)                                                              \
  X(OSE_No_File)                                                               \
  X(OSE_Bad_Fd)                                                                \
  X(OSE_Bad_Arguments)                                                         \
  X(OSE_Is_Dir)                                                                \
  X(OSE_No_Mem)                                                                \
  X(OSE_Partial_Write)                                                         \
  X(OSE_Partial_Read)                                                          \
  X(OSE_Block)                                                                 \
  X(OSE_Other)

X_ENUM(OS_Error, OS_ERRORS)

typedef struct {
  String    full_path;
  String    name;
  isize     size;
  Timestamp acces_time;
  Timestamp creation_time;
  Timestamp modification_time;
  bool      is_dir;
  bool      readable;
  bool      writeable;
  bool      executable;
} File_Info;

typedef enum {
  FP_Read       = 0b0001,
  FP_Write      = 0b0010,
  FP_Create     = 0b0100,
  FP_Truncate   = 0b1000,
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

[[nodiscard]]
extern OS_Result_Int file_read(Fd fd, Byte_Slice buf);
extern OS_Result_Int file_write(Fd fd, Byte_Slice buf);

[[nodiscard]]
extern OS_Result_Fd file_open(String path, isize permissions);
[[nodiscard]]
extern OS_Result_Fd file_open_at(Fd dir, String path, isize permissions);

extern OS_Error file_close(Fd fd);

extern OS_Error file_stat(Fd fd, File_Info *fi);

extern OS_Error file_remove(String path);
extern OS_Error file_remove_at(Fd dir, String path);
extern OS_Error file_remove_dir(String path);
extern OS_Error file_remove_dir_at(Fd dir, String path);

[[nodiscard]]
extern bool file_exists(String path);

[[nodiscard]]
extern OS_Result_Dir directory_read_fd(Fd fd, Allocator allocator);
[[nodiscard]]
extern OS_Result_Dir directory_read_path(String path, Allocator allocator);
[[nodiscard]]
extern OS_Result_String directory_get_current(Allocator allocator);

extern void directory_delete(Directory dir, Allocator allocator);

typedef struct {
  String_Slice args;
  String_Slice env;
  Fd           stdin;
  Fd           stdout;
  Fd           stderr;
} Process_Creation_Args;

extern OS_Result_Pid process_create(String path, Process_Creation_Args const *args);
extern OS_Error process_wait(Pid pid);
extern diverging process_exit(isize code);
extern diverging thread_exit(isize code);

[[nodiscard]]
extern OS_Result_Bytes read_entire_file_fd(Fd fd, Allocator allocator);

[[nodiscard]]
extern OS_Result_Bytes read_entire_file_path(String path, Allocator allocator);

extern OS_Error write_entire_file_fd(Fd fd, Byte_Slice data);
extern OS_Error write_entire_file_path(String path, Byte_Slice data);

[[nodiscard]]
extern Writer writer_from_handle(Fd handle);
extern Maybe_Int writer_file_proc(rawptr handle, Byte_Slice data);

[[nodiscard]]
extern Reader reader_from_handle(Fd handle);
extern Maybe_Int file_reader_proc(rawptr handle, Byte_Slice buf);

[[nodiscard]]
extern rawptr os_pages_allocate(isize n);
extern bool   os_pages_deallocate(rawptr p, isize n);

extern void processor_yield();

extern void __os_init();
