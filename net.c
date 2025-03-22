#include "net.h"

extern Maybe_Int reader_socket_proc(rawptr data, Byte_Slice buf) {
  return result_to_maybe(Maybe_Int, socket_read(transmute(Socket, data), buf));
}

[[nodiscard]]
extern Reader reader_from_socket(Socket socket) {
  return (Reader){
      .data = transmute(rawptr, socket),
      .proc = reader_socket_proc,
  };
}

extern Maybe_Int writer_socket_proc(rawptr data, Byte_Slice buf) {
  return result_to_maybe(Maybe_Int, socket_write(transmute(Socket, data), buf));
}

[[nodiscard]]
extern Writer writer_from_socket(Socket socket) {
  return (Writer){
      .data = transmute(rawptr, socket),
      .proc = writer_socket_proc,
  };
}
