#include "axProfiler.h"
#include "axProfilerPackets.h"
#include <ax_core/other/axGlobalSingleton.h>
#include <ax_core/thread/axThread.h>
#include <ax_core/thread/axCondQueue.h>
#include <ax_core/net/engine/axNetEngine.h>
#include <ax_core/data_format/axBinSerializer.h>

void axProfiler_init() {
	axProfiler::s_instance();
}

class axProfiler::Worker : public axNonCopyable {
public:
	using Time		= axProfilerTime;
	using EntryId   = axProfilerEntryId;
	using PacketCmd = axProfiler_PacketCmd;

	class Engine : public axNetEngine {
	public:
		Worker* _worker = nullptr;
	};

	Worker(axProfiler* profiler, axUInt16 port)
		: _profiler(profiler)
	{
		{
			auto md = _mdata.scopedLock();
			for (auto& p : _packetBuf) {
				md->_freePackets.append(&p);
			}
		}

		_running = 1;
		_thread.start("axProfiler", [this, port](){
			_engine.listen(port);

			while(_running) {
				_engine.update(5);

				axSList<Packet> packets;
				{
					auto md = _mdata.scopedLock();
					ax_swap(packets, md->_outPackets);
				}

				for (auto& p : packets) {
					switch (p.cmd) {
						case PacketCmd::EntryBegin: {
							axProfilerPacket::EntryBegin dst;
							auto& src = p.payload.entryBegin;
							dst.entryId = src.entryId;
							dst.file = axStrView_c_str(src.loc.file);
							dst.func = axStrView_c_str(src.loc.func);
							dst.line = src.loc.line;
							dst.name = src.name;
							dst.tag  = src.tag;
							dst.startTime = src.startTime;
							_engine.sendPacketToAll(dst);
						}break;
						case PacketCmd::EntryEnd: {
							axProfilerPacket::EntryEnd dst;
							auto& src = p.payload.entryEnd;
							dst.entryId = src.entryId;
							dst.endTime = src.endTime;
							_engine.sendPacketToAll(dst);
						}break;
					}
				}

				{
					auto md = _mdata.scopedLock();
					while (auto p = packets.popHead()) {
						md->_freePackets.append(ax_move(p));
					}
				}
			}
		});
	}

	~Worker() {
		_running = 0;
		_thread.join();
		{
			auto md = _mdata.scopedLock();
			md->_freePackets.removeAll();
			md->_outPackets.removeAll();
		}
	}

	class Packet : public axSListNode<Packet> {
	public:
		PacketCmd	cmd;

		struct EntryBegin : public axNonCopyable {
			EntryId			entryId;
			axSourceLoc		loc;
			axStrLiteral	name;
			axStrLiteral	tag;
			Time			startTime;

			template<class SE>
			void onSerialize(SE& se) {
				se.named_io("entryId",   entryId);
				se.named_io("loc",       loc);
				se.named_io("name",      name);
				se.named_io("tag",       tag);
				se.named_io("startTime", startTime);
			}
		};

		struct EntryEnd : public axNonCopyable {
			EntryId			entryId;
			Time			endTime;

			template<class SE>
			void onSerialize(SE& se) {
				se.named_io("entryId",   entryId);
				se.named_io("loc",       endTime);
			}
		};

		union Payload {
			Payload() {}
			EntryBegin	entryBegin;
			EntryEnd	entryEnd;
		};
		Payload payload;

		template<class SE>
		void onSerialize(SE& se) {
			se.named_io("cmd", cmd);
			switch (cmd) {
				case PacketCmd::EntryBegin:	se.named_io("payload", payload.entryBegin);	break;
				case PacketCmd::EntryEnd:	se.named_io("payload", payload.entryEnd);	break;
			}
		}
	};

	axUPtr<Packet> newPacket() {
		auto md = _mdata.scopedLock();
		auto p = md->_freePackets.popHead();
		//AX_ASSERT(p);
		return p;
	}

	void sendPacket(axUPtr<Packet> && p) {
		auto md = _mdata.scopedLock();
		md->_outPackets.append(ax_move(p));
	}

	axProfiler* profiler() { return _profiler; }

private:
	struct MData {
		axSList<Packet>	_outPackets;
		axSList<Packet>	_freePackets;
	};
	axInternalMutexProtected<MData>	_mdata;

	//TODO replace by linear allocator
	axFixedArray<Packet, 1024> _packetBuf;

	axProfiler*	_profiler = nullptr;
	axAtomicInt _running;
	Engine		_engine;
	axThread	_thread;
};

#if 0
#pragma mark ------------------------------------------
#endif

struct axProfiler::PrivateData {
	struct MData {
		EntryId			_nextEntryId = 0;
		axUPtr<Worker>	_worker;
	};
	axInternalMutexProtected<MData>	_mdata;
};

axProfiler* axProfiler::s_instance() {
	static axGlobalSingleton<This> s;
	return s.ptr();
}

axProfiler::axProfiler() {
	_privateData.newObject();
}

axProfiler::~axProfiler() {
	{
		auto md = _privateData->_mdata.scopedLock();
		md->_worker.unref();
	}
	_privateData.deleteObject();
}

void axProfiler::enableDebugPort(axUInt16 port) {
	auto md = _privateData->_mdata.scopedLock();
	if (md->_worker) {
		AX_ASSERT(false); // already started
		return;
	}
	auto o = axUPtr_new(Worker(this, port));
	md->_worker = ax_move(o);
}

axProfilerEntryId axProfiler::entryBegin(const axSourceLoc& loc, axStrLiteral name, axStrLiteral tag, Time startTime) {
	auto md = _privateData->_mdata.scopedLock();
	if (!md->_worker)
		return 0;

	auto eid = ++md->_nextEntryId;

	auto pkt = md->_worker->newPacket();
	if (!pkt) return 0;

	pkt->cmd = Worker::PacketCmd::EntryBegin;
	auto& payload = pkt->payload.entryBegin;
	payload.entryId = eid;
	payload.loc = loc;
	payload.name = name;
	payload.tag  = tag;
	payload.startTime = startTime;

	md->_worker->sendPacket(ax_move(pkt));
	return eid;
}

void axProfiler::entryEnd(EntryId eid, Time endTime) {
	auto md = _privateData->_mdata.scopedLock();
//	auto deltaTime = endTime.diffInNanoSeconds(e->startTime);
	if (!md->_worker)
		return;

	auto pkt = md->_worker->newPacket();
	if (!pkt) return;

	pkt->cmd = Worker::PacketCmd::EntryEnd;
	auto& payload = pkt->payload.entryEnd;
	payload.entryId = eid;
	payload.endTime = endTime;

	md->_worker->sendPacket(ax_move(pkt));
}
