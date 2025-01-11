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

struct Time {
  i64 nsec;
};

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

internal void time_precise_clock(struct Time time, isize *hour, isize *min, isize *sec, isize *nanos);
internal void time_date(struct Time t, b8 full, isize *year, isize *month, isize *day, isize *yday);

internal void time_get_value(struct Time time, Time_Value *value) {
  time_precise_clock(time, &value->hour, &value->minute, &value->second, &value->nanosecond);
  value->microsecond = (value->nanosecond / Microsecond) % 1000;
  value->millisecond = (value->nanosecond / Millisecond) % 1000;
  isize _yday;
  time_date(time, true, &value->year, &value->month, &value->day, &_yday);
}

[[nodiscard]]
internal struct Time time_now() {
  struct {
    i64 seconds;
    i64 nanoseconds;
  } _time = {0};
  u64 status = syscall(SYS_clock_gettime, 0, &_time);
  assert(status == 0);
  return (struct Time){.nsec = _time.nanoseconds + Second * _time.seconds};
}

internal void time_sleep(Duration duration) {
  struct {
    i64 seconds;
    i64 nanoseconds;
  } _time = {
    .seconds     = duration / Second,
    .nanoseconds = duration % Second,
  };
  u64 status = syscall(SYS_nanosleep, &_time, nil);
  assert(status == 0);
}

/*
Number of seconds in a minute (without leap seconds).
*/
#define SECONDS_PER_MINUTE (60l)

/*
Number of seconds in an hour (without leap seconds).
*/
#define SECONDS_PER_HOUR   (60l * SECONDS_PER_MINUTE)

/*
Number of seconds in a day (without leap seconds).
*/
#define SECONDS_PER_DAY    (24l * SECONDS_PER_HOUR)

/*
Number of seconds in a week (without leap seconds).
*/
#define SECONDS_PER_WEEK   ( 7l * SECONDS_PER_DAY)

/*
Days in 400 years, with leap days.
*/
#define DAYS_PER_400_YEARS (365l*400l + 97l)

/*
Days in 100 years, with leap days.
*/
#define DAYS_PER_100_YEARS (365l*100l + 24l)

/*
Days in 4 years, with leap days.
*/
#define DAYS_PER_4_YEARS   (365l*4l   + 1l)

#define ABSOLUTE_ZERO_YEAR   (-292277022399l)
#define ABSOLUTE_TO_INTERNAL (-9223371966579724800l)
#define INTERNAL_TO_ABSOLUTE (-ABSOLUTE_TO_INTERNAL)

#define UNIX_TO_INTERNAL ((1969*365 + 1969/4 - 1969/100 + 1969/400) * SECONDS_PER_DAY)
#define INTERNAL_TO_UNIX (-UNIX_TO_INTERNAL)

#define WALL_TO_INTERNAL ((1884*365 + 1884/4 - 1884/100 + 1884/400) * SECONDS_PER_DAY)
#define INTERNAL_TO_WALL (-WALL_TO_INTERNAL)

#define UNIX_TO_ABSOLUTE (UNIX_TO_INTERNAL + INTERNAL_TO_ABSOLUTE)
#define ABSOLUTE_TO_UNIX (-UNIX_TO_ABSOLUTE)

internal void time_precise_clock(struct Time time, isize *hour, isize *min, isize *sec, isize *nanos) {
	isize _sec   = time.nsec / 1e9;
  isize _nanos = time.nsec % (isize)1e9;
	_sec  += INTERNAL_TO_ABSOLUTE;
	*nanos = (isize)(_nanos);
	*sec   = (isize)(_sec  % SECONDS_PER_DAY);
	*hour  = *sec  / SECONDS_PER_HOUR;
	*sec  -= *hour * SECONDS_PER_HOUR;
	*min   = *sec  / SECONDS_PER_MINUTE;
	*sec  -= *min  * SECONDS_PER_MINUTE;
  return;
}

[[nodiscard]]
internal u64 _time_abs(struct Time t) {
	return (u64)(t.nsec/(isize)1e9 + UNIX_TO_ABSOLUTE);
}

[[nodiscard]]
internal b8 _time_is_leap_year (isize year){
	return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

internal i32 _days_before[13] = {
	0,
	31,
	31 + 28,
	31 + 28 + 31,
	31 + 28 + 31 + 30,
	31 + 28 + 31 + 30 + 31,
	31 + 28 + 31 + 30 + 31 + 30,
	31 + 28 + 31 + 30 + 31 + 30 + 31,
	31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
	31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
	31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
	31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30,
	31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30 + 31,
};

internal void _abs_date (u64 abs, b8 full, isize *year, isize *month, isize *day, isize *yday) {
	isize d = abs / SECONDS_PER_DAY;

	// 400 year cycles
	isize n = d / DAYS_PER_400_YEARS;
	isize y = 400 * n;
	d -= DAYS_PER_400_YEARS * n;

	// Cut-off 100 year cycles
	n = d / DAYS_PER_100_YEARS;
	n -= n >> 2;
	y += 100 * n;
	d -= DAYS_PER_100_YEARS * n;

	// Cut-off 4 year cycles
	n = d / DAYS_PER_4_YEARS;
	y += 4 * n;
	d -= DAYS_PER_4_YEARS * n;

	n = d / 365;
	n -= n >> 2;
	y += n;
	d -= 365 * n;

	*year = (isize)((i64)(y) + ABSOLUTE_ZERO_YEAR);
	*yday = (isize)(d);

	if (!full) {
		return;
	}

	*day = *yday;

	if (_time_is_leap_year(*year)) {
		if (*day > 31+29-1) {
			*day -= 1;
		} else if (*day == 31+29-1) {
			*month = Month_February;
			*day = 29;
			return;
		}
	}

	*month = (Month)(*day / 31);
	isize end = (isize)(_days_before[(isize)(*month)+1]);
	isize begin = 0;
	if (*day >= end) {
		*(isize*)(&month) += 1;
		begin = end;
	} else {
		begin = (isize)(_days_before[*month]);
	}
	*(isize*)(&month) += 1; // January is 1
	*day = *day - begin + 1;
	return;
}

internal void time_date(struct Time t, b8 full, isize *year, isize *month, isize *day, isize *yday)  {
	 _abs_date(_time_abs(t), full, year, month, day, yday);
}

internal Duration time_diff(struct Time a, struct Time b) {
	return (Duration)(a.nsec - b.nsec);
}

internal Duration time_since(struct Time t) {
	return time_diff(time_now(), t);
}
