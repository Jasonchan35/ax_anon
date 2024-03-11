#pragma once

#include "../base/ax_basic_types.h"
#include "axTimestamp.h"

#define axWeekday_EnumList(E) \
	E(Sunday,) \
	E(Monday,) \
	E(Tuesday,) \
	E(Wednesday,) \
	E(Thursday,) \
	E(Friday,) \
	E(Saturday,) \
//----
ax_ENUM_CLASS(axWeekday, axInt8)

class axDateTime {
public:
	using Nanoseconds = axUtcTime::Nanoseconds;

	static const axInt kMinToSec	= 60;
	static const axInt kHourToSec	= 60 * kMinToSec;
	static const axInt kDayToSec	= 24 * kHourToSec;
	static const axInt kWeekToSec	= 7  * kDayToSec;

	void reset() { _t = {}; }

	axDateTime() = default;
	explicit axDateTime(const axLocalTime& t) { set(t); }
	explicit axDateTime(const axUtcTime&   t) { set(t); }

	void set(const axLocalTime& t);
	void set(const axUtcTime& t);

	void setToNow() { set(axLocalTime::s_now()); }

	axLocalTime	toLocalTime() const;
	axUtcTime	toUtcTime() const;

	axInt	year() const			{ return _t.tm_year + 1900; }
	void	setYear(axInt v)		{ _t.tm_year = static_cast<int>(v) - 1900; }
	
	axInt	month() const			{ return _t.tm_mon + 1; }
	void	setMonth(axInt v)		{ _t.tm_mon = static_cast<int>(v) - 1; }
	
	axInt	day() const				{ return _t.tm_mday; }
	void	setDay(axInt v)			{ _t.tm_mday = static_cast<int>(v); }
	
	axInt	hour() const			{ return _t.tm_hour; }
	void	setHour(axInt v)		{ _t.tm_hour = static_cast<int>(v); }
	
	axInt	minute() const			{ return _t.tm_min; }
	void	setMinute(axInt v)		{ _t.tm_min = static_cast<int>(v); }
	
	axInt	second() const			{ return _t.tm_sec; }
	void	setSecond(axInt v)		{ _t.tm_sec = static_cast<int>(v); }
	
	bool 	isDaylightSavingTime	() const { return _t.tm_isdst != 0; }
	void	setIsDaylightSavingTime	(bool b) { _t.tm_isdst = b ? 1 : 0; }

	//!days since Sunday - [0,6]
	axWeekday	weekday() const { return axWeekday(static_cast<axEnum<axWeekday>::IntType>(_t.tm_wday)); }

	void setWeekday(axWeekday w) { _t.tm_wday = ax_enum_int(w); }

	void setDate(axInt y, axInt m, axInt d) { setYear(y); setMonth(m);  setDay(d);    }
	void setTime(axInt h, axInt m, axInt s) { setHour(h); setMinute(m); setSecond(s); }

	axInt	yearInDays() const { return _t.tm_yday; }

	void addSeconds	(axInt sec ) { set(toUtcTime() + Nanoseconds(sec)); }
	void addMinutes	(axInt min ) { addSeconds(min  * kMinToSec); }
	void addHours	(axInt hour) { addSeconds(hour * kHourToSec); }
	void addDays	(axInt day ) { addSeconds(day  * kDayToSec); }
	void addWeeks	(axInt week) { addSeconds(week * kWeekToSec); }

	void onFormat(axFormat& f) const;

	bool tryParse(axStrView str);
	void parse(axStrView str) { if (!tryParse(str)) throw axError_Undefined(AX_LOC); }

private:
	struct tm _t = {};
	Nanoseconds _nanoSec{0};
};
