#include "io.h"

#include "strings.h"

extern Maybe_Int write_cstring(const Writer *w, cstring s) {
  return w->proc(w->data, (Byte_Slice){.data = (byte *)s, .len = cstring_len(s)});
}
