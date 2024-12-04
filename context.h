#include "codin.h"

typedef enum {
  LL_Debug = 0,
  LL_Info,
  LL_Warn,
  LL_Error,
  LL_Fatal,
} Log_Level;

typedef void (*Logger_Proc)(rawptr, Log_Level, String, Source_Code_Location const *);

typedef struct {
  rawptr data;
  Logger_Proc proc;
} Logger;

typedef struct {
  Allocator allocator;
  Allocator temp_allocator;
  Logger logger;
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
