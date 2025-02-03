#include "codin.h"

#ifdef linux
  #include "mem_virtual_linux.h"
#endif

internal OS_Result_Bytes map_file(Fd file, uintptr start, uintptr size) {
  return _map_file(file, start, size);
}

internal OS_Result_Bytes map_entire_file(Fd file) {
  return map_file(file, 0, -1);
}
