#pragma once

#include <ax_core/string/axStrLiteral.h>
#include <ax_core/pointer/axUPtr.h>
#include "axProfilerPackets.h"

void axProfiler_init();

class axProfiler;

class axProfilerEntry {
public:
	using Time		= axProfilerTime;
	using EntryId   = axProfilerEntryId;

	axProfilerEntry(const axSourceLoc& loc, axStrLiteral name, axStrLiteral tag, Time minDeltaTime);
	~axProfilerEntry();

private:
	axSourceLoc		_loc;
	axStrLiteral	_name;
	axStrLiteral	_tag;

	Time			_startTime;
	EntryId			_entryId;
	Time			_minDeltaTime;
};

// Performance Profiler
class axProfiler : public axNonCopyable {
	using This = axProfiler;
public:
	static axProfiler* s_instance();
	using EntryId = axProfilerEntryId;
	using Time = axProfilerTime;

	static const axUInt16 kDefaultTcpPort = 31008;

	axProfiler();
	~axProfiler();

	void enableDebugPort(axUInt16 port = kDefaultTcpPort);

	EntryId	entryBegin	(const axSourceLoc& loc, axStrLiteral name, axStrLiteral tag, Time startTime);
	void	entryEnd	(EntryId eid, Time endTime);

private:
	class Worker;

	struct PrivateData;
	axPrivateObject<PrivateData, 256>	_privateData;
};

AX_INLINE
axProfilerEntry::axProfilerEntry(const axSourceLoc& loc, axStrLiteral name, axStrLiteral tag, Time minDeltaTime)
	: _loc(loc)
	, _name(name)
	, _tag(tag)
{
	auto* m = axProfiler::s_instance();
	_minDeltaTime = minDeltaTime;

	if (_minDeltaTime == Time::s_zero()) {
		auto startTime = Time::s_now();
		_entryId = m->entryBegin(loc, name, tag, startTime);
	}
}

AX_INLINE
axProfilerEntry::~axProfilerEntry() {
	auto endTime = Time::s_now();
	auto* m = axProfiler::s_instance();
	auto deltaTime = endTime - _startTime;

	if (_minDeltaTime == Time::s_zero()) {
		m->entryEnd(_entryId, endTime);

	} else if (deltaTime >= _minDeltaTime) {
		_entryId = m->entryBegin(_loc, _name, _tag, _startTime);
		m->entryEnd(_entryId, endTime);
	}
}

#define ax_PROFILER_CPU_TIMING(name)	axProfilerEntry AX_UNIQUE_NAME(_axProfiler_)(AX_LOC, name, "CPU", axProfilerTime::s_zero());
