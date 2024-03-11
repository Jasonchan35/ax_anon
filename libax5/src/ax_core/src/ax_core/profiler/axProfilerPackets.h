#pragma once

#include <ax_core/net/engine/axNetPacket.h>
#include <ax_core/time/axTimestamp.h>

#define axProfiler_PacketCmd_EnumList(E) \
	E(Unknown,) \
	E(EntryBegin,) \
	E(EntryEnd,) \
//---------
ax_ENUM_CLASS(axProfiler_PacketCmd, axNetPacketHeader::Cmd)

using axProfilerEntryId = axInt64;
using axProfilerTime = axHiResTime;

namespace axProfilerPacket {

using Cmd = axProfiler_PacketCmd;

class PacketBase : public axNetPacket {
public:
	PacketBase(Cmd cmd) : axNetPacket(cmd) {}
};

#define axProfilerPacket_CLASS(T) \
	axNetPacket_CLASS(T, PacketBase) \
	T() : PacketBase(Cmd::T) {} \
//----

class EntryBegin : public PacketBase {
	axProfilerPacket_CLASS(EntryBegin)
public:
	using Time		= axProfilerTime;
	using EntryId	= axProfilerEntryId;

	EntryId			entryId;
	axString_<256>	func;
	axString_<256>	file;
	axInt			line;

	axString_<256>	name;
	axString_<256>	tag;
	Time			startTime;

	template<class SE>
	void onSerialize(SE& se) {
		Base::onSerialize(se);
		se.io(entryId);
		se.io(func);
		se.io(file);
		se.io(line);
		se.io(name);
		se.io(tag);
		se.io(startTime);
	}
};

class EntryEnd : public PacketBase {
	axProfilerPacket_CLASS(EntryEnd)
public:
	using Time		= axProfilerTime;
	using EntryId   = axProfilerEntryId;

	EntryId		entryId;
	Time		endTime;

	template<class SE>
	void onSerialize(SE& se) {
		Base::onSerialize(se);
		se.io(entryId);
		se.io(endTime);
	}
};

} // namespace
