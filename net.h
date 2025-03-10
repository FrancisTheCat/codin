#pragma once
#include "codin.h"

#include "io.h"
#include "time.h"

typedef i64 Socket;

#define NET_ERRORS(X) \
  X(NE_None)          \
  X(NE_Bad_Socket)    \
  X(NE_Would_Block)   \
  X(NE_Hung_Up)       \
  X(NE_Other)         \

X_ENUM(Net_Error, NET_ERRORS);

struct in_addr {
  u32 s_addr;
};

struct sockaddr_in {
  u16            sin_family;
  u16            sin_port;
  struct in_addr sin_addr;
  u8             __pad[10];
};

typedef struct {
  struct sockaddr_in address;
  Socket             socket;
} Connection;

#define SOCKET_SHUTDOWNS(X)                                                    \
  X(Socket_Shutdown_Read,       1)                                             \
  X(Socket_Shutdown_Write,      2)                                             \
  X(Socket_Shutdown_Read_Write, 3)                                             \

X_ENUM_EXPLICIT(Socket_Shutdown, SOCKET_SHUTDOWNS)

#define POLL_EVENTS(X)                                                         \
  X(Poll_Event_Read,  1)                                                       \
  X(Poll_Event_Write, 2)                                                       \

X_ENUM_EXPLICIT(Poll_Event, POLL_EVENTS)

typedef Result(isize,      Net_Error) Net_Result_Int;
typedef Result(Connection, Net_Error) Net_Result_Connection;
typedef Result(Socket,     Net_Error) Net_Result_Socket;

[[nodiscard]]
extern Net_Result_Socket socket_create(u16 port, b8 blocking);
[[nodiscard]]
extern Net_Result_Connection socket_accept(Socket socket);
[[nodiscard]]
extern Net_Result_Int socket_read(Socket socket, Byte_Slice buf);
[[nodiscard]]
extern Net_Result_Int socket_write(Socket socket, Byte_Slice buf);
extern Net_Error socket_shutdown(Socket socket, Socket_Shutdown shutdown);
extern Net_Error socket_close(Socket socket);

[[nodiscard]]
extern Reader    reader_from_socket(Socket socket);
extern Maybe_Int reader_socket_proc(rawptr data, Byte_Slice buf);

[[nodiscard]]
extern Writer    writer_from_socket(Socket socket);
extern Maybe_Int writer_socket_proc(rawptr data, Byte_Slice buf);

extern Net_Error socket_poll(Socket socket, isize events, Duration timeout);
