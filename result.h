#pragma once

#include "codin.h"

#define Maybe(T)                                                               \
  struct {                                                                     \
    T value;                                                                   \
    bool ok;                                                                   \
  }

#define or_else(_maybe, other)                                                 \
  ({                                                                           \
    type_of(_maybe) maybe = _maybe;                                            \
    type_of(maybe.value) ret = maybe.value;                                    \
    if (!maybe.ok) {                                                           \
      ret = other;                                                             \
    }                                                                          \
    ret;                                                                       \
  })

#define or_return(_maybe, other)                                               \
  ({                                                                           \
    type_of(_maybe) maybe = (_maybe);                                          \
    type_of(maybe.value) ret;                                                  \
    if (!maybe.ok) {                                                           \
      return other;                                                            \
    } else {                                                                   \
      ret = maybe.value;                                                       \
    }                                                                          \
    ret;                                                                       \
  })

#define or_goto(_maybe, label)                                                 \
  ({                                                                           \
    type_of(_maybe) maybe = _maybe;                                            \
    typeof(maybe.value) ret;                                                   \
    if (maybe.ok) {                                                            \
      ret = maybe.value;                                                       \
    } else {                                                                   \
      goto label;                                                              \
    }                                                                          \
    ret;                                                                       \
  })

#define or_break(_maybe)                                                       \
  ({                                                                           \
    type_of(_maybe) maybe = _maybe;                                            \
    typeof(maybe.value) ret;                                                   \
    if (maybe.ok) {                                                            \
      ret = maybe.value;                                                       \
    } else {                                                                   \
      break;                                                                   \
    }                                                                          \
    ret;                                                                       \
  })

#define or_continue(_maybe)                                                    \
  ({                                                                           \
    type_of(_maybe) maybe = _maybe;                                            \
    typeof(maybe.value) ret;                                                   \
    if (maybe.ok) {                                                            \
      ret = maybe.value;                                                       \
    } else {                                                                   \
      continue;                                                                \
    }                                                                          \
    ret;                                                                       \
  })

#define or_goto_err(_result, label)                                            \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    typeof(result.value) ret;                                                  \
    if (result.err) {                                                          \
      goto label;                                                              \
    } else {                                                                   \
      ret = result.value;                                                      \
    }                                                                          \
    ret;                                                                       \
  })

#define or_break_err(_result)                                                  \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    typeof(result.value) ret;                                                  \
    if (result.err) {                                                          \
      break;                                                                   \
    } else {                                                                   \
      ret = result.value;                                                      \
    }                                                                          \
    ret;                                                                       \
  })

#define or_continue_err(_result)                                               \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    typeof(result.value) ret;                                                  \
    if (result.err) {                                                          \
      continue;                                                                \
    } else {                                                                   \
      ret = result.value;                                                      \
    }                                                                          \
    ret;                                                                       \
  })

#define unwrap(_maybe)                                                         \
  ({                                                                           \
    type_of(_maybe) maybe = _maybe;                                            \
    assert((maybe).ok);                                                        \
    (maybe).value;                                                             \
  })

#define Result(T, E)                                                           \
  struct {                                                                     \
    T value;                                                                   \
    E err;                                                                     \
  }

#define or_else_err(_result, other)                                            \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    type_of(result.value) ret = result.value;                                  \
    if (result.err) {                                                          \
      ret = other;                                                             \
    }                                                                          \
    ret;                                                                       \
  })

#define or_return_err(_result)                                                 \
  ({                                                                           \
    type_of(_result) local_result = _result;                                   \
    if (local_result.err) {                                                    \
      result.err = local_result.err;                                           \
      return result;                                                           \
    }                                                                          \
    local_result.value;                                                        \
  })

#define or_return_err_v(_result)                                               \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    if (result.err) {                                                          \
      return result.err;                                                       \
    }                                                                          \
    result.value;                                                              \
  })

#define unwrap_err(_result)                                                    \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    if (result.err) {                                                          \
      __write_cstring("Unwrapped error: 0x");                                  \
      __write_usize_hex((usize)result.err);                                    \
      __write_cstring("\n");                                                   \
      panic("");                                                               \
    }                                                                          \
    result.value;                                                              \
  })

#define unwrap_err_msg(_result, msg)                                           \
  ({                                                                           \
    type_of(_result) result = _result;                                         \
    if (result.err) {                                                          \
      __write_cstring(msg);                                                    \
      __write_cstring("\n");                                                   \
      __write_cstring("Unwrapped error: 0x");                                  \
      __write_usize_hex((usize)result.err);                                    \
      __write_cstring("\n");                                                   \
      panic("");                                                               \
    }                                                                          \
    result.value;                                                              \
  })

#define or_return_bool(cond)                                                   \
  if (!(cond)) {                                                               \
    return false;                                                              \
  }

typedef Maybe(isize) Maybe_Int;

#define result_to_maybe(maybe_t, _result)                                      \
  ({                                                                           \
    maybe_t maybe;                                                             \
    type_of((_result)) result = (_result);                                     \
    if (result.err) {                                                          \
      maybe.ok = false;                                                        \
    } else {                                                                   \
      maybe.value = result.value;                                              \
      maybe.ok = true;                                                         \
    }                                                                          \
    maybe;                                                                     \
  })

#define if_let(_maybe, name, if_block, else_block)                             \
  {                                                                            \
    type_of(_maybe) __maybe = _maybe;                                          \
    if (__maybe.ok) {                                                          \
      type_of(__maybe.value) name = __maybe.value;                             \
      { if_block }                                                             \
    } else {                                                                   \
      { else_block }                                                           \
    }                                                                          \
  }

#define if_let_err(_result, value_name, ok_block, error_name, err_block)       \
  {                                                                            \
    type_of(_result) __result = _result;                                       \
    if (__result.err) {                                                        \
      type_of(__result.err) error_name = __result.err;                         \
      { err_block }                                                            \
    } else {                                                                   \
      type_of(__result.value) value_name = __result.value;                     \
      { ok_block }                                                             \
    }                                                                          \
  }

// you have to diverge out of this block
#define or_do(_maybe, block)                                                   \
  ({                                                                           \
    type_of(_maybe) __maybe = _maybe;                                          \
    if (!__maybe.ok) {                                                         \
      block;                                                                   \
      unreachable();                                                           \
    }                                                                          \
    __maybe.value;                                                             \
  })

// you have to diverge out of this block
#define or_do_err(_result, error_name, block)                                  \
  ({                                                                           \
    type_of(_result) __result = _result;                                       \
    if (__result.err) {                                                        \
      type_of(__result.err) error_name = __result.err;                         \
      block;                                                                   \
      unreachable();                                                           \
    }                                                                          \
    __result.value;                                                            \
  })
