#pragma once

#include "codin.h"
#include "os.h"

#define THREAD_STACK_DEFAULT (8 * 1024)
#define THREAD_TLS_DEFAULT   (8 * 1024)

typedef void (*Thread_Proc)(rawptr);

[[nodiscard]]
extern Tid get_thread_id();

extern OS_Result_Tid thread_create(Thread_Proc proc, rawptr user_data, isize stack_size, isize tls_size);

// extern OS_Error thread_join(Tid thread);
