#include "codin.h"

#define SOCK_NONBLOCK 00004000
#define MSG_NOSIGNAL  0x4000

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
  Socket             socket;
} Connection;

#define NET_ERRORS(X) \
  X(NE_None)          \
  X(NE_Bad_Socket)    \
  X(NE_Would_Block)   \
  X(NE_Hung_Up)       \
  X(NE_Other)         \

X_ENUM(Net_Error, NET_ERRORS);

typedef Result(isize,      Net_Error) Net_Result_Int;
typedef Result(Connection, Net_Error) Net_Result_Connection;
typedef Result(Socket,     Net_Error) Net_Result_Socket;

[[nodiscard]]
internal u16 htons(u16 x) {
  return x << 8 | (x >> 8);
}

[[nodiscard]]
internal Net_Result_Socket socket_create(u16 port, b8 blocking) {
  Net_Result_Socket result = {0};
  u64 type = SOCK_STREAM;
  if (!blocking) {
    type |= SOCK_NONBLOCK;
  }
  Socket s = syscall(SYS_socket, AF_INET, type, 0);
  if (s < 0) {
    result.err = NE_Other;
    return result;
  }

  b8 ok;
  b32 reuse = true;
  ok = syscall(SYS_setsockopt, s, SOL_SOCKET, SO_REUSEADDR, &reuse, size_of(reuse)) == 0;
  if (!ok) {
    result.err = NE_Other;
    return result;
  }
  ok = syscall(SYS_setsockopt, s, SOL_SOCKET, SO_REUSEPORT, &reuse, size_of(reuse)) == 0;
  if (!ok) {
    result.err = NE_Other;
    return result;
  }

  struct sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(port);
  addr.sin_addr.s_addr = 0;

  if (syscall(SYS_bind, s, (struct sockaddr *)&addr, size_of(addr)) < 0) {
    result.err = NE_Other;
    return result;
  }
  if (syscall(SYS_listen, s, 1000) < 0) {
    result.err = NE_Other;
    return result;
  }

  result.value = s;
  return result;
}

[[nodiscard]]
internal Net_Result_Connection socket_accept(Socket socket) {
  Net_Result_Connection result = {0};

  isize addr_len = size_of(result.value.address);
  result.value.socket = syscall(SYS_accept, socket, (struct sockaddr *)&result.value.address, &addr_len);

  if (result.value.socket < 0) {
    OS_Error err = __errno_unwrap(result.value.socket);
    if (err == OSE_Block) {
      result.err = NE_Would_Block;
    } else {
      result.err = NE_Other;
    }
  }

  return result;
}

[[nodiscard]]
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

[[nodiscard]]
internal Reader reader_from_socket(Socket socket) {
  return (Reader){
      .data = transmute(rawptr, socket),
      .proc = reader_socket_proc,
  };
}

[[nodiscard]]
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
    result.err = NE_Other;
  }
  return result;
}

internal Maybe_Int writer_socket_proc(rawptr data, Byte_Slice buf) {
  return result_to_maybe(Maybe_Int, socket_write(transmute(Socket, data), buf));
}

[[nodiscard]]
internal Writer writer_from_socket(Socket socket) {
  return (Writer){
      .data = transmute(rawptr, socket),
      .proc = writer_socket_proc,
  };
}

internal Net_Error socket_close(Socket socket) {
  return file_close(socket) != OSE_None ? NE_Other : NE_None;
}

#define POLL_EVENTS(X)                                                         \
  X(Poll_Event_Read,  1)                                                       \
  X(Poll_Event_Write, 2)                                                       \

X_ENUM_EXPLICIT(Poll_Event, POLL_EVENTS)

internal Net_Error socket_poll(Socket socket, isize events, Duration timeout) {
  i16 _events = 0;
  if (events & Poll_Event_Read) {
    _events |= 1;
  }
  if (events & Poll_Event_Read) {
    _events |= 4;
  }
  struct {
    i32 fd;
    i16 events, revents;
  } pollfd = {
    .fd     = (i32)socket,
    .events = _events,
  };
  isize result = syscall(SYS_poll, &pollfd, 1, timeout > 0 ? timeout / Millisecond : -1);
  if (pollfd.revents & 0x10) {
    return NE_Hung_Up;
  }
  if (result < 0) {
    return NE_Other;
  }
  return nil;
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
