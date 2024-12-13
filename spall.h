#include "codin.h"

// SPDX-FileCopyrightText: © 2023 Phillip Trudeau-Tavara <pmttavara@protonmail.com>
// SPDX-License-Identifier: MIT

/*

TODO: Optional Helper APIs:

  - Compression API: would require a mutexed lockable context (yuck...)
      - Either using a ZIP library, a name cache + TIDPID cache, or both (but ZIP is likely more than enough!!!)
      - begin()/end() writes compressed chunks to a caller-determined destination
          - The destination can be the buffered-writing API or a custom user destination
      - Ultimately need to take a lock with some granularity... can that be the caller's responsibility?

  - Counter Event: should allow tracking arbitrary named values with a single event, for memory and frame profiling

  - Ring-buffer API
        spall_ring_init
        spall_ring_emit_begin
        spall_ring_emit_end
        spall_ring_flush
*/

#ifndef SPALL_H
#define SPALL_H

#if !defined(_MSC_VER) || defined(__clang__)
    #define SPALL_NOINSTRUMENT __attribute__((no_instrument_function))
    #define SPALL_FORCEINLINE __attribute__((always_inline))
#else
    #define _CRT_SECURE_NO_WARNINGS
    #define SPALL_NOINSTRUMENT // Can't noinstrument on MSVC!
    #define SPALL_FORCEINLINE __forceinline
#endif

#define SPALL_FN static inline SPALL_NOINSTRUMENT

#define SPALL_MIN(a, b) (((a) < (b)) ? (a) : (b))

typedef struct SpallHeader {
    u64 magic_header; // = 0x0BADF00D
    u64 version; // = 1
    f64   timestamp_unit;
    u64 must_be_0;
} __attribute__ ((packed)) SpallHeader;

enum {
    SpallEventType_Invalid             = 0,
    SpallEventType_Custom_Data         = 1, // Basic readers can skip this.
    SpallEventType_StreamOver          = 2,

    SpallEventType_Begin               = 3,
    SpallEventType_End                 = 4,
    SpallEventType_Instant             = 5,

    SpallEventType_Overwrite_Timestamp = 6, // Retroactively change timestamp units - useful for incrementally improving RDTSC frequency.
    SpallEventType_Pad_Skip            = 7,
};

typedef struct SpallBeginEvent {
    u8 type; // = SpallEventType_Begin
    u8 category;

    u32 pid;
    u32 tid;
    f64 when;

    u8 name_length;
    u8 args_length;
} __attribute__ ((packed)) SpallBeginEvent;

typedef struct SpallBeginEventMax {
    SpallBeginEvent event;
    byte name_bytes[255];
    byte args_bytes[255];
} __attribute__ ((packed)) SpallBeginEventMax;

typedef struct SpallEndEvent {
    u8  type; // = SpallEventType_End
    u32 pid;
    u32 tid;
    f64   when;
} __attribute__ ((packed)) SpallEndEvent;

typedef struct SpallPadSkipEvent {
    u8  type; // = SpallEventType_Pad_Skip
    u32 size;
} __attribute__ ((packed)) SpallPadSkipEvent;

typedef struct SpallProfile SpallProfile;

// Important!: If you define your own callbacks, mark them SPALL_NOINSTRUMENT!
typedef b8 (*SpallWriteCallback)(SpallProfile *self, const void *data, isize length);
typedef b8 (*SpallFlushCallback)(SpallProfile *self);
typedef void (*SpallCloseCallback)(SpallProfile *self);

struct SpallProfile {
    f64 timestamp_unit;
    SpallWriteCallback write;
    SpallFlushCallback flush;
    SpallCloseCallback close;
    rawptr data;
};

// Important!: If you are writing Begin/End events, then do NOT write
//             events for the same PID + TID pair on different buffers!!!
typedef struct SpallBuffer {
    void *data;
    isize length;

    // Internal data - don't assign this
    isize head;
    SpallProfile *ctx;
} SpallBuffer;

#if defined(SPALL_BUFFER_PROFILING) && !defined(SPALL_BUFFER_PROFILING_GET_TIME)
    #error "You must #define SPALL_BUFFER_PROFILING_GET_TIME() to profile buffer flushes."
#endif

SPALL_FN SPALL_FORCEINLINE void spall__buffer_profile(SpallProfile *ctx, SpallBuffer *wb, f64 spall_time_begin, f64 spall_time_end, const char *name, int name_len);
#ifdef SPALL_BUFFER_PROFILING
    #define SPALL_BUFFER_PROFILE_BEGIN() f64 spall_time_begin = (SPALL_BUFFER_PROFILING_GET_TIME())
    // Don't call this with anything other than a string literal
    #define SPALL_BUFFER_PROFILE_END(name) spall__buffer_profile(ctx, wb, spall_time_begin, (SPALL_BUFFER_PROFILING_GET_TIME()), "" name "", sizeof("" name "") - 1)
#else
    #define SPALL_BUFFER_PROFILE_BEGIN()
    #define SPALL_BUFFER_PROFILE_END(name)
#endif

SPALL_FN SPALL_FORCEINLINE b8 spall__buffer_flush(SpallProfile *ctx, SpallBuffer *wb) {
    // precon: wb
    // precon: wb->data
    // precon: wb->head <= wb->length
    // precon: !ctx || ctx->write
    if (wb->head && ctx) {
        SPALL_BUFFER_PROFILE_BEGIN();
        if (!ctx->write) return false;
        if (!ctx->write(ctx, wb->data, wb->head)) return false;
        SPALL_BUFFER_PROFILE_END("Buffer Flush");
    }
    wb->head = 0;
    return true;
}

SPALL_FN SPALL_FORCEINLINE b8 spall__buffer_write(SpallProfile *ctx, SpallBuffer *wb, void *p, isize n) {
    // precon: !wb || wb->head < wb->length
    // precon: !ctx || ctx->write
    if (!wb) return ctx->write && ctx->write(ctx, p, n);
    if (wb->head + n > wb->length && !spall__buffer_flush(ctx, wb)) return false;
    if (n > wb->length) {
        SPALL_BUFFER_PROFILE_BEGIN();
        if (!ctx->write || !ctx->write(ctx, p, n)) return false;
        SPALL_BUFFER_PROFILE_END("Unbuffered Write");
        return true;
    }
    mem_copy((byte *)wb->data + wb->head, p, n);
    wb->head += n;
    return true;
}

SPALL_FN b8 spall_buffer_flush(SpallProfile *ctx, SpallBuffer *wb) {
    if (!spall__buffer_flush(ctx, wb)) return false;
    return true;
}

SPALL_FN b8 spall_buffer_init(SpallProfile *ctx, SpallBuffer *wb) {
    if (!spall_buffer_flush(NULL, wb)) return false;
    wb->ctx = ctx;
    return true;
}
SPALL_FN b8 spall_buffer_quit(SpallProfile *ctx, SpallBuffer *wb) {
    if (!spall_buffer_flush(ctx, wb)) return false;
    wb->ctx = NULL;
    return true;
}

SPALL_FN b8 spall_buffer_abort(SpallBuffer *wb) {
    if (!wb) return false;
    wb->ctx = NULL;
    if (!spall__buffer_flush(NULL, wb)) return false;
    return true;
}

SPALL_FN isize spall_build_header(void *buffer, isize rem_size, f64 timestamp_unit) {
    isize header_size = sizeof(SpallHeader);
    if (header_size > rem_size) {
        return 0;
    }

    SpallHeader *header = (SpallHeader *)buffer;
    header->magic_header = 0x0BADF00D;
    header->version = 1;
    header->timestamp_unit = timestamp_unit;
    header->must_be_0 = 0;
    return header_size;
}
SPALL_FN SPALL_FORCEINLINE isize spall_build_begin(void *buffer, isize rem_size, const char *name, signed long name_len, const char *args, signed long args_len, f64 when, u32 tid, u32 pid) {
    SpallBeginEventMax *ev = (SpallBeginEventMax *)buffer;
    u8 trunc_name_len = (u8)SPALL_MIN(name_len, 255); // will be interpreted as truncated in the app (?)
    u8 trunc_args_len = (u8)SPALL_MIN(args_len, 255); // will be interpreted as truncated in the app (?)

    isize ev_size = sizeof(SpallBeginEvent) + trunc_name_len + trunc_args_len;
    if (ev_size > rem_size) {
        return 0;
    }

    ev->event.type = SpallEventType_Begin;
    ev->event.category = 0;
    ev->event.pid = pid;
    ev->event.tid = tid;
    ev->event.when = when;
    ev->event.name_length = trunc_name_len;
    ev->event.args_length = trunc_args_len;
    mem_copy((rawptr)(ev->name_bytes                 ), (rawptr)name, trunc_name_len);
    mem_copy((rawptr)(ev->name_bytes + trunc_name_len), (rawptr)args, trunc_args_len);

    return ev_size;
}
SPALL_FN SPALL_FORCEINLINE isize spall_build_end(void *buffer, isize rem_size, f64 when, u32 tid, u32 pid) {
    isize ev_size = sizeof(SpallEndEvent);
    if (ev_size > rem_size) {
        return 0;
    }

    SpallEndEvent *ev = (SpallEndEvent *)buffer;
    ev->type = SpallEventType_End;
    ev->pid = pid;
    ev->tid = tid;
    ev->when = when;

    return ev_size;
}

SPALL_FN void spall_quit(SpallProfile *ctx) {
    if (!ctx) return;
    if (ctx->close) ctx->close(ctx);

    *ctx = (SpallProfile){0};
}

SPALL_FN SpallProfile spall_init_callbacks(f64 timestamp_unit,
                                           SpallWriteCallback write,
                                           SpallFlushCallback flush,
                                           SpallCloseCallback close,
                                           void *userdata) {
    SpallProfile ctx = {0};
    if (timestamp_unit < 0) return ctx;
    ctx.timestamp_unit = timestamp_unit;
    ctx.data = userdata;
    ctx.write = write;
    ctx.flush = flush;
    ctx.close = close;

    SpallHeader header;
    isize len = spall_build_header(&header, sizeof(header), timestamp_unit);
    if (!ctx.write(&ctx, &header, len)) {
        spall_quit(&ctx);
        return ctx;
    }

    return ctx;
}

SPALL_FN b8 spall_flush(SpallProfile *ctx) {
    if (!ctx->flush || !ctx->flush(ctx)) return false;
    return true;
}

SPALL_FN SPALL_FORCEINLINE b8 spall_buffer_begin_args(SpallProfile *ctx, SpallBuffer *wb, const char *name, signed long name_len, const char *args, signed long args_len, f64 when, u32 tid, u32 pid) {

    if ((wb->head + sizeof(SpallBeginEventMax)) > wb->length) {
        if (!spall__buffer_flush(ctx, wb)) {
            return false;
        }
    }

    wb->head += spall_build_begin((char *)wb->data + wb->head, wb->length - wb->head, name, name_len, args, args_len, when, tid, pid);

    return true;
}

SPALL_FN SPALL_FORCEINLINE b8 spall_buffer_begin_ex(SpallProfile *ctx, SpallBuffer *wb, const char *name, signed long name_len, f64 when, u32 tid, u32 pid) {
    return spall_buffer_begin_args(ctx, wb, name, name_len, "", 0, when, tid, pid);
}

SPALL_FN b8 spall_buffer_begin(SpallProfile *ctx, SpallBuffer *wb, const char *name, signed long name_len, f64 when) {
    return spall_buffer_begin_args(ctx, wb, name, name_len, "", 0, when, 0, 0);
}

SPALL_FN SPALL_FORCEINLINE b8 spall_buffer_end_ex(SpallProfile *ctx, SpallBuffer *wb, f64 when, u32 tid, u32 pid) {
    if ((wb->head + sizeof(SpallEndEvent)) > wb->length) {
        if (!spall__buffer_flush(ctx, wb)) {
            return false;
        }
    }

    wb->head += spall_build_end((char *)wb->data + wb->head, wb->length - wb->head, when, tid, pid);

    return true;
}

SPALL_FN b8 spall_buffer_end(SpallProfile *ctx, SpallBuffer *wb, f64 when) { return spall_buffer_end_ex(ctx, wb, when, 0, 0); }

SPALL_FN SPALL_FORCEINLINE void spall__buffer_profile(SpallProfile *ctx, SpallBuffer *wb, f64 spall_time_begin, f64 spall_time_end, const char *name, int name_len) {
    // precon: ctx
    // precon: ctx->write
    char temp_buffer_data[2048];
    SpallBuffer temp_buffer = { temp_buffer_data, sizeof(temp_buffer_data) };
    if (!spall_buffer_begin_ex(ctx, &temp_buffer, name, name_len, spall_time_begin, (u32)(uintptr)wb->data, 4222222222)) return;
    if (!spall_buffer_end_ex(ctx, &temp_buffer, spall_time_end, (u32)(uintptr)wb->data, 4222222222)) return;
    if (ctx->write) ctx->write(ctx, temp_buffer_data, temp_buffer.head);
}

#endif // SPALL_H
