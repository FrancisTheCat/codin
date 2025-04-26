#include "codin.h"

#include "strings.h"

#include "runtime_linux.h"
#include "syscalls_linux_amd64.h"

extern void __write_cstring(cstring str) {
  syscall(SYS_write, 2, str, cstring_len(str));
}

extern void __write_string(String str) {
  syscall(SYS_write, 2, str.data, str.len);
}
