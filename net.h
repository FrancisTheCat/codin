#include "codin.h"

#define MSG_NOSIGNAL 0x4000

struct in_addr {
  u32 s_addr;
};

struct sockaddr_in {
  u16            sin_family;
  u16            sin_port;
  struct in_addr sin_addr;
  u8             __pad[10];
};

typedef i64 Socket;

typedef struct {
  struct sockaddr_in address;
  Socket   socket;
} Connection;

typedef enum {
  NE_None = 0,
  NE_Bad_Socket,
  NE_Would_Block,
  NE_Other,
} Net_Error;

internal String net_error_string(Net_Error err) {
  switch (err) {
  case NE_None:
    return LIT("NE_None");
  case NE_Bad_Socket:
    return LIT("NE_Bad_Socket");
  case NE_Would_Block:
    return LIT("NE_Would_Block");
  case NE_Other:
    return LIT("NE_Other");
  }
  return LIT("NE_Invalid_Enum_Value");
}

typedef Result(isize, Net_Error) Net_Result_Int;
typedef Result(Connection, Net_Error) Net_Result_Connection;
typedef Result(Socket, Net_Error) Net_Result_Socket;

internal u16 htons(u16 x) {
  return x << 8 | (x >> 8);
}

internal Socket socket_create(u16 port) {
  Socket s = syscall(SYS_socket, AF_INET, SOCK_STREAM, 0);

  b32 reuse = true;
  assert(syscall(SYS_setsockopt, s, SOL_SOCKET, SO_REUSEADDR, &reuse, size_of(reuse)) == 0);
  assert(syscall(SYS_setsockopt, s, SOL_SOCKET, SO_REUSEPORT, &reuse, size_of(reuse)) == 0);

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = 0;

  if (syscall(SYS_bind, s, (struct sockaddr *)&addr, size_of(addr)) < 0) {
    panic("Failed to bind socket");
  }
  if (syscall(SYS_listen, s, 1000) < 0) {
    panic("Failed to listen on socket");
  }

  return s;
}

internal Net_Result_Connection socket_accept(Socket socket) {
  Net_Result_Connection result = {0};

  isize addr_len = size_of(result.value.address);
  result.value.socket = syscall(SYS_accept, socket, (struct sockaddr *)&result.value.address, &addr_len);

  return result;
}

internal Net_Result_Int socket_read(Socket socket, Byte_Slice buf) {
  Net_Result_Int result = {0};
  result.value = syscall(SYS_recvfrom, socket, buf.data, buf.len, 0, 0, 0);
  if (result.value < 0) {
    result.err = NE_Other;
  }
  return result;
}

internal Maybe_Int reader_socket_proc(rawptr data, Byte_Slice buf) {
  return result_to_maybe(Maybe_Int, socket_read(transmute(Socket, data), buf));
}

internal Reader reader_from_socket(Socket socket) {
  return (Reader){
      .data = transmute(rawptr, socket),
      .proc = reader_socket_proc,
  };
}

internal Net_Result_Int socket_write(Socket socket, Byte_Slice buf) {
  Net_Result_Int result = {0};

  struct iovec {
    rawptr base;
    usize  len;
  } iovec = {
    .base = buf.data,
    .len  = (usize)buf.len,
  };

  struct msghdr {
    rawptr        name;       /* Optional address */
    u32           namelen;    /* Size of address */
    struct iovec *iov;        /* Scatter/gather array */
    usize         iovlen;     /* # elements in msg_iov */
    void         *control;    /* Ancillary data, see below */
    usize         controllen; /* Ancillary data buffer len */
    i32           flags;      /* Flags (unused) */
  } socket_msg = {
    .iov        = &iovec,
    .iovlen     = 1,
  };

  result.value = syscall(SYS_sendmsg, socket, &socket_msg, MSG_NOSIGNAL | 0x40, 0, 0);
  if (result.value < 0) {
    // log_errorf(LIT("Errno: 0x%x"), -result.value);
    // result.err = NE_Other;
  }
  return result;
}

internal Maybe_Int writer_socket_proc(rawptr data, Byte_Slice buf) {
  return result_to_maybe(Maybe_Int, socket_write(transmute(Socket, data), buf));
}

internal Writer writer_from_socket(Socket socket) {
  return (Writer){
      .data = transmute(rawptr, socket),
      .proc = writer_socket_proc,
  };
}

internal Net_Error socket_close(Socket socket) {
  return file_close(socket) != OSE_None ? NE_Other : NE_None;
}

typedef enum {
  Socket_Shutdown_Read       = 1,
  Socket_Shutdown_Write      = 2,
  Socket_Shutdown_Read_Write = 3,
} Socket_Shutdown;

internal Net_Error socket_shutdown(Socket socket, Socket_Shutdown shutdown) {
  i32 how = 0;
  switch (shutdown) {
  case Socket_Shutdown_Read:
    how = 0;
  case Socket_Shutdown_Write:
    how = 1;
  case Socket_Shutdown_Read_Write:
    how = 2;
  }
  return syscall(SYS_shutdown, socket, how) ? NE_Other : NE_None;
}
