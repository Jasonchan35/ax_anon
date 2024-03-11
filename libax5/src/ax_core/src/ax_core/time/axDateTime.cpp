#include "axDateTime.h"

void axDateTime::set(const axLocalTime& ts) {
	time_t tmp = ts.toSeconds();
#if AX_COMPILER_VC
	_nanoSec = Nanoseconds(ts.toNanoseconds() % 1000000000LL);

	if (::gmtime_s(&_t, &tmp) != 0) {
		throw axError_Undefined(AX_LOC);
	}

#elif AX_COMPILER_GCC | AX_COMPILER_CLANG

	if (::gmtime_r(&tmp, &_t) == nullptr)
		throw axError_Undefined(AX_LOC);

#else
	#error "Unsupported compiler"
#endif
}

void axDateTime::set(const axUtcTime& ts) {
	time_t tmp = ts.toSeconds();
#if AX_COMPILER_VC
	_nanoSec = Nanoseconds(ts.toNanoseconds() % 1000000000LL);

	if (::gmtime_s(&_t, &tmp) != 0) {
		throw axError_Undefined(AX_LOC);
	}

#elif AX_COMPILER_GCC | AX_COMPILER_CLANG

	if (::gmtime_r(&tmp, &_t) == nullptr)
		throw axError_Undefined(AX_LOC);

#else
	#error "Unsupported compiler"
#endif
}

axLocalTime axDateTime::toLocalTime() const {
//	auto t = ::mktime(ax_const_cast(&_t)); // seconds since 1970
#if AX_COMPILER_VC
	auto t = ::_mkgmtime(ax_const_cast(&_t)); // seconds since 1970
#else
	auto t = ::timegm(ax_const_cast(&_t));
#endif

	auto ts = axLocalTime(Nanoseconds(t * 1000000000LL)) + Nanoseconds(_nanoSec);
	return ts;
}

axUtcTime axDateTime::toUtcTime() const {
#if AX_COMPILER_VC
	auto t = ::_mkgmtime(ax_const_cast(&_t)); // seconds since 1970
#else
	auto t = ::timegm(ax_const_cast(&_t));
#endif
	auto ts = axUtcTime(Nanoseconds(t * 1000000000LL)) + Nanoseconds(_nanoSec);
	return ts;
}

void axDateTime::onFormat(axFormat& f) const {
	f.format("{?,4:0}-{?,2:0}-{?,2:0} {?,2:0}:{?,2:0}:{?,2:0}"
		, year(), month(), day()
		, hour(), minute(), second());
}

bool axDateTime::tryParse(axStrView str) {
	int Y,M,D;
	int h,m;
	double s;

#if AX_COMPILER_VC
	int ret = _snscanf(str.data(), str.size(), "%d-%d-%d %d:%d:%lf", &Y, &M, &D, &h, &m, &s);
#else
	axTempString tmpStr {str};
	int ret = sscanf(tmpStr.c_str(), "%d-%d-%d %d:%d:%lf", &Y, &M, &D, &h, &m, &s);
#endif

	if( ret != 6 )
		return false;

	setYear(Y);
	setMonth(M);
	setDay(D);

	setHour(h);
	setMinute(m);
	setSecond(static_cast<axInt>(s));

	return true;
}

