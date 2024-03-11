#pragma once

#include "../string/axFormat.h"

template<class CLOCK>
class axTimestampT {
	using This = axTimestampT;
public:
	using clock = CLOCK;
	using time_point = typename CLOCK::time_point;
	using Int = axInt64;

	using Nanoseconds = std::chrono::nanoseconds;
	AX_STATIC_ASSERT(axTypeTraits::isSame<typename clock::duration::rep, Int>::value);
	AX_STATIC_ASSERT(axTypeTraits::isSame<typename clock::duration, std::chrono::nanoseconds>::value);


	axTimestampT() = default;
	constexpr axTimestampT(const Nanoseconds& v) : _value(v) {}

			  static axTimestampT	s_now()  noexcept { return clock::now().time_since_epoch(); }
	constexpr static axTimestampT	s_zero() noexcept { return Nanoseconds(0); }

	static const Int kMinToSec	= 60;
	static const Int kHourToSec	= 60 * kMinToSec;
	static const Int kDayToSec	= 24 * kHourToSec;
	static const Int kWeekToSec	= 7  * kDayToSec;

	constexpr static axTimestampT	s_date_1970_to_2000() noexcept { return Nanoseconds((365*30+7) * kDayToSec * 1000000000LL); } // 1970 year to 2000 year

	static axTimestampT s_timezone() noexcept;

	constexpr Nanoseconds	value() const { return _value; }

	constexpr static This s_seconds		(Int    v) noexcept { return Nanoseconds(v * 1000000000LL); }
	constexpr static This s_milliseconds(Int    v) noexcept { return Nanoseconds(v * 1000000LL); }
	constexpr static This s_microseconds(Int    v) noexcept { return Nanoseconds(v * 1000LL); }
	constexpr static This s_nanoseconds	(Int    v) noexcept { return Nanoseconds(v); }

	constexpr static This s_seconds		(double v) noexcept { return Nanoseconds(static_cast<Int>(v * 1000000000LL)); }
	constexpr static This s_milliseconds(double v) noexcept { return Nanoseconds(static_cast<Int>(v * 1000000LL)); }
	constexpr static This s_microseconds(double v) noexcept { return Nanoseconds(static_cast<Int>(v * 1000LL)); }
	constexpr static This s_nanoseconds	(double v) noexcept { return Nanoseconds(static_cast<Int>(v)); }

			  void setToNow			() noexcept			{ *this = s_now(); }
	constexpr void setSeconds		(Int    v) noexcept	{ *this = s_seconds(v); }
	constexpr void setMilliseconds	(Int    v) noexcept	{ *this = s_milliseconds(v); }
	constexpr void setMicroseconds	(Int    v) noexcept	{ *this = s_microseconds(v); }
	constexpr void setNanoseconds	(Int    v) noexcept	{ *this = s_nanoseconds(v); }
	constexpr void setInt			(Int	v) noexcept { _value = Nanoseconds(v); }

	constexpr void		setSecondsInFloat64	(axFloat64 v) noexcept	{ setInt(static_cast<Int>(v / 1000000000.0)); }
	constexpr axFloat64 toSecondsInFloat64	() const noexcept		{     return _value.count() / 1000000000.0; }

	constexpr Int	toSeconds		() const noexcept { return _value.count() / 1000000000LL; }
	constexpr Int	toMilliseconds	() const noexcept { return _value.count() / 1000000LL; }
	constexpr Int	toMicroseconds	() const noexcept { return _value.count() / 1000LL; }
	constexpr Int	toNanoseconds	() const noexcept { return _value.count(); }

	constexpr Int	toInt() const noexcept { return _value.count(); }

	constexpr bool operator==(const This& r) const noexcept { return _value == r._value; }
	constexpr bool operator!=(const This& r) const noexcept { return _value != r._value; }
	constexpr bool operator< (const This& r) const noexcept { return _value <  r._value; }
	constexpr bool operator<=(const This& r) const noexcept { return _value <= r._value; }
	constexpr bool operator> (const This& r) const noexcept { return _value >  r._value; }
	constexpr bool operator>=(const This& r) const noexcept { return _value >= r._value; }

	constexpr This operator+ (const This& r) const noexcept { return _value + r._value; }
	constexpr This operator- (const This& r) const noexcept { return _value - r._value; }

	void operator+=(const This& r) noexcept { *this = *this + r; }
	void operator-=(const This& r) noexcept { *this = *this - r; }

	void onFormat(axFormat& f) const { f << toNanoseconds() << "ns"; }

private:
	Nanoseconds	_value;
};

template<class CLOCK>
axTimestampT<CLOCK> axTimestampT<CLOCK>::s_timezone() noexcept {
#if AX_COMPILER_VC
	FILETIME utc = {}, local = {};
	FileTimeToLocalFileTime (&utc, &local);
	uint64_t u = ( (uint64_t) local.dwHighDateTime << 32) | local.dwLowDateTime; // u is 100-nanosecond
	return Nanoseconds(u * 100);
#else
	struct timeval	tv = {};
	struct timezone	z  = {};
	gettimeofday( &tv, &z );
	return Nanoseconds(z.tz_minuteswest * 60 * 1000000000LL);
#endif
}

class axLocalClock {
public:
	using duration = std::chrono::nanoseconds;
	using time_point = std::chrono::time_point<axLocalClock>;

	static time_point now() {
		auto t = std::chrono::system_clock::now().time_since_epoch() + s_timezone();
		auto d = std::chrono::duration_cast<duration>(t);
		return time_point(d);
	}

	static duration s_timezone() {
	#if AX_COMPILER_VC
		FILETIME utc = {}, local = {};
		FileTimeToLocalFileTime (&utc, &local);
		int64_t u = ((int64_t) local.dwHighDateTime << 32) | local.dwLowDateTime; // u is 100-nanosecond
		return duration(u * 100);
	#else
		struct timeval	tv = {};
		struct timezone	z  = {};
		gettimeofday( &tv, &z );
		return duration(z.tz_minuteswest * 60 * 1000000000LL);
	#endif
	}
};

class axUtcClock {
public:
	using duration = std::chrono::nanoseconds;
	using time_point = std::chrono::time_point<axUtcClock>;

	static time_point now() {
		auto t = std::chrono::system_clock::now().time_since_epoch();
		auto d = std::chrono::duration_cast<duration>(t);
		return time_point(d);
	}
};

using axUtcTime   = axTimestampT<axUtcClock>;
using axLocalTime = axTimestampT<axLocalClock>;
using axHiResTime = axTimestampT<std::chrono::high_resolution_clock>;

inline axLocalTime axUtcToLocalTime(const axUtcTime& utc) {
	axLocalTime o;
	o.setInt(utc.toInt());
	o += axLocalTime::s_timezone();
	return o;
}

inline axUtcTime axLocalToUtcTime(const axLocalTime& ts) {
	axUtcTime o;
	o.setInt(ts.toInt());
	o -= axUtcTime::s_timezone();
	return o;
}

#if AX_OS_WINDOWS

inline axUtcTime axUtcTime_make(const FILETIME &ft) {
	uint64_t u = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime; // based on 1601-01-01
	u -= 116444736000000000; // based on 1970-01-01
	return axUtcTime::Nanoseconds(u * 100);
}

inline FILETIME axFILETIME_make(const axUtcTime& t) {
	uint64_t u = t.toNanoseconds() / 100;
	u += 116444736000000000; // based on 1970-01-01
	
	FILETIME	ft;
	ft.dwHighDateTime = u >> 32;
	ft.dwLowDateTime  = (DWORD)u;
	return ft;
}

#endif // AX_OS_WINDOWS

inline axUtcTime axUtcTime_make(const time_t& t) {
	axUtcTime o;
	o.setSeconds(t);
	return o;
}

inline axUtcTime axUtcTime_make(const timespec & t) {
	axInt64 v = static_cast<axInt64>(t.tv_sec) * 1000000000 + t.tv_nsec;
	axUtcTime o;
	o.setInt(v);
	return o;
}

inline timespec ax_timespec_make(const axUtcTime& t) {
	axInt64 i = t.toInt();
	timespec o;
	o.tv_sec  = static_cast<time_t>(i / 1000000000LL);
	o.tv_nsec = static_cast<long  >(i % 1000000000LL);
	return o;
}

inline axUtcTime axUtcTime_make(const timeval & t) {
	axInt64 v = static_cast<axInt64>(t.tv_sec)  * 1000000000LL + static_cast<axInt64>(t.tv_usec) * 1000LL;
	axUtcTime o;
	o.setInt(v);
	return o;
}

inline timeval ax_timeval_make(const axUtcTime& t) {
	axInt64 i = t.toInt();
	timeval	o;
	o.tv_sec  = static_cast<decltype(o.tv_sec)>(  i / 1000000000LL);
	o.tv_usec = static_cast<decltype(o.tv_usec)>((i % 1000000000LL) / 1000LL);
	return o;
}

#if AX_OS_MACOSX || AX_OS_IOS

inline axUtcTime axUtcTime_make(NSDate* ns) {
	axUtcTime o;
	o.setSecondsInFloat64([ns timeIntervalSince1970]);
	return o;
}

#endif
