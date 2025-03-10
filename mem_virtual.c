#include "mem_virtual.h"

extern OS_Result_Bytes map_entire_file(Fd file) {
  return map_file(file, 0, -1);
}
