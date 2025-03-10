#pragma once

#include "codin.h"

#define Nanosecond (u64)(1)
#define Microsecond (u64)(1000 * Nanosecond)
#define Millisecond (u64)(1000 * Microsecond)
#define Second (u64)(1000 * Millisecond)
#define Minute (u64)(60 * Second)
#define Hour (u64)(60 * Minute)
#define Day (u64)(24 * Hour)

typedef i64 Duration;

#define MONTHS(X)                                                              \
  X(Month_January)                                                             \
  X(Month_February)                                                            \
  X(Month_March)                                                               \
  X(Month_April)                                                               \
  X(Month_May)                                                                 \
  X(Month_June)                                                                \
  X(Month_July)                                                                \
  X(Month_August)                                                              \
  X(Month_September)                                                           \
  X(Month_October)                                                             \
  X(Month_November)                                                            \
  X(Month_December)                                                            \

X_ENUM(Month, MONTHS)

#undef MONTHS

#define WEEKDAYS(X)                                                            \
  X(WDay_Monday)                                                               \
  X(WDay_Tuesday)                                                              \
  X(WDay_Wednesday)                                                            \
  X(WDay_Thursday)                                                             \
  X(WDay_Friday)                                                               \
  X(WDay_Saturday)                                                             \
  X(WDay_Sunday)                                                               \

X_ENUM(Weekday, WEEKDAYS)

#undef WEEKDAYS

typedef i64 Timestamp;

typedef struct {
  isize year;
  isize month;
  isize day;
  isize hour;
  isize minute;
  isize second;
  isize millisecond;
  isize microsecond;
  isize nanosecond;
} Time_Value;

extern void time_precise_clock(Timestamp time, isize *hour, isize *min, isize *sec, isize *nanos);
extern void time_date(Timestamp t, b8 full, isize *year, isize *month, isize *day, isize *yday);

extern void time_get_value(Timestamp time, Time_Value *value);

[[nodiscard]]
extern Timestamp time_now();

extern void time_sleep(Duration duration);

extern void time_precise_clock(Timestamp time, isize *hour, isize *min, isize *sec, isize *nanos);

extern void time_date(Timestamp t, b8 full, isize *year, isize *month, isize *day, isize *yday);

extern Duration time_since(Timestamp t);
