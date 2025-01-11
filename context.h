#include "codin.h"

#define LOG_LEVELS(X)                                                          \
  X(LL_Debug)                                                                  \
  X(LL_Info)                                                                   \
  X(LL_Warn)                                                                   \
  X(LL_Error)                                                                  \
  X(LL_Fatal)                                                                  \

X_ENUM(Log_Level, LOG_LEVELS)

typedef void (*Logger_Proc)(rawptr, Log_Level, String, Source_Code_Location const *);

typedef struct {
  rawptr data;
  Logger_Proc proc;
} Logger;

typedef struct {
  Allocator allocator;
  Allocator temp_allocator;
  Logger    logger;
} Context;

internal _Thread_local Context context;

#define CONTEXT_PUSH                                                           \
  {                                                                            \
    Context __saved_context = context
#define CONTEXT_POP                                                            \
  context = __saved_context;                                                   \
  }
#define CONTEXT_GUARD(block)                                                   \
  {                                                                            \
    CONTEXT_PUSH;                                                              \
    block;                                                                     \
    CONTEXT_POP;                                                               \
  }
