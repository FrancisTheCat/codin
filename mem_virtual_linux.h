#include "codin.h"

internal OS_Result_Bytes _map_file(Fd file, uintptr start, uintptr size) {
  if (size == -1) {
    File_Info fi;
    file_stat(file, &fi);
    size = fi.size;
  }
  usize result = syscall(SYS_mmap, nil, size, PROT_READ, MAP_PRIVATE, file, start);
  if (result > 0) {
    return (OS_Result_Bytes) { .value = (Byte_Slice) { .data = (byte *)result, .len = (isize)size} };
  }
  return (OS_Result_Bytes) { .err = __errno_unwrap(result) };
}
