#include "net.h"

#include "syscalls_linux_amd64.h"
#include "runtime_linux.h"

#define SOCK_NONBLOCK 00004000
#define MSG_NOSIGNAL  0x4000

internal u16 htons(u16 x) {
  return x << 8 | (x >> 8);
}

extern Net_Result_Socket socket_create(u16 port, b8 blocking) {
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

extern Net_Result_Connection socket_accept(Socket socket) {
  Net_Result_Connection result = {0};

  isize addr_len = size_of(result.value.address);
  result.value.socket = syscall(SYS_accept, socket, (struct sockaddr *)&result.value.address, &addr_len);

  if (result.value.socket < 0) {
    // OS_Error err = __errno_unwrap(result.value.socket);
    // if (err == OSE_Block) {
    //   result.err = NE_Would_Block;
    // } else {
      result.err = NE_Other;
    // }
  }

  return result;
}

extern Net_Result_Int socket_read(Socket socket, Byte_Slice buf) {
  Net_Result_Int result = {0};
  result.value = syscall(SYS_recvfrom, socket, buf.data, buf.len, 0, 0, 0);
  if (result.value < 0) {
    result.err = NE_Other;
  }
  return result;
}

extern Net_Result_Int socket_write(Socket socket, Byte_Slice buf) {
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

extern Net_Error socket_close(Socket socket) {
  return file_close(socket) != OSE_None ? NE_Other : NE_None;
}

extern Net_Error socket_poll(Socket socket, isize events, Duration timeout) {
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

extern Net_Error socket_shutdown(Socket socket, Socket_Shutdown shutdown) {
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
