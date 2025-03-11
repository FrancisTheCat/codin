#pragma once

#include "codin.h"

typedef Maybe_Int (*Writer_Proc)(rawptr, Byte_Slice);

typedef struct {
  rawptr      data;
  Writer_Proc proc;
} Writer;

/// Maybe_Int write_any(const Writer *w, const T *v)
#define write_any(w, _v)                                                       \
  write_bytes(w, (Byte_Slice){.data = (byte *)_v, .len = size_of(*_v)})

internal Maybe_Int write_bytes(const Writer *w, Byte_Slice data) {
  return w->proc(w->data, data);
}

internal Maybe_Int write_byte(const Writer *w, byte data) {
  return w->proc(w->data, (Byte_Slice){&data, 1});
}

extern Maybe_Int write_cstring(const Writer *w, cstring s);

internal Maybe_Int write_string(const Writer *w, String s) {
  return w->proc(w->data, (Byte_Slice){.data = (byte *)s.data, .len = s.len});
}

internal Maybe_Int writer_buffer_proc(rawptr wdata, Byte_Slice data) {
  vector_append_slice((Byte_Buffer *)wdata, data);
  return (Maybe_Int){.value = data.len, .ok = true};
}

[[nodiscard]]
internal Writer writer_from_buffer(Byte_Buffer *buffer) {
  Writer w;
  w.data = buffer;
  w.proc = writer_buffer_proc;
  return w;
}

internal Maybe_Int writer_builder_proc(rawptr wdata, Byte_Slice data) {
  vector_append_slice((Builder *)wdata, data);
  return (Maybe_Int){.value = data.len, .ok = true};
}

[[nodiscard]]
internal Writer writer_from_builder(Builder *builder) {
  Writer w;
  w.data = builder;
  w.proc = writer_builder_proc;
  return w;
}

internal Maybe_Int null_writer_proc(rawptr _data, Byte_Slice data) {
  (void)_data;
  return (Maybe_Int){.value = data.len, .ok = true};
}

[[nodiscard]]
internal Writer null_writer() {
  return (Writer){
      .data = nil,
      .proc = null_writer_proc,
  };
}

typedef Maybe_Int (*Reader_Proc)(rawptr, Byte_Slice);

typedef struct {
  rawptr      data;
  Reader_Proc proc;
} Reader;

/// Maybe_Int read_any(const Reader *r, const T *v)
#define read_any(r, _v)                                                       \
  read_bytes(r, (Byte_Slice){.data = (byte *)_v, .len = size_of(*_v)})

/// T read_t(const Reader *r, type T)
#define read_t(r, T) ({                                                    \
  T __read_t_result;                                                       \
  read_bytes(                                                              \
    r,                                                                     \
    (Byte_Slice) {                                                         \
      .data = (byte *)&__read_t_result,                                    \
      .len  = size_of(__read_t_result)                                     \
    }                                                                      \
  );                                                                       \
  __read_t_result;                                                         \
})

[[nodiscard]]
internal Maybe_Int read_bytes(const Reader *reader, Byte_Slice buf) {
  return reader->proc(reader->data, buf);
}

internal Maybe_Int buffer_reader_proc(rawptr data, Byte_Slice buf) {
  Byte_Slice *src = (Byte_Slice *)data;
  if (!src->len) {
    return (Maybe_Int){.value = 0, .ok = true};
  }
  isize n = bytes_copy(buf, *src);
  src->data += n;
  src->len -= n;
  return (Maybe_Int){.value = n, .ok = true};
}

[[nodiscard]]
internal Reader buffer_reader(Byte_Slice *buffer) {
  return (Reader){
      .data = buffer,
      .proc = buffer_reader_proc,
  };
}
