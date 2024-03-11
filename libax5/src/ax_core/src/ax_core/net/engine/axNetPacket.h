#pragma once

#include "axNetBucket.h"

#define axNetPacket_CLASS(T, BASE) \
	using This = T; \
	using Base = BASE; \
public: \
	virtual void _onSerialize(axBinSerializer&   se) override { _onSerializeHeader(se); onSerialize(se); } \
	virtual void _onSerialize(axBinDeserializer& se) override { _onSerializeHeader(se); onSerialize(se); } \
//------

class axNetPacketHeader {
public:
	using Cmd = axUInt32;

	struct Len {
		using Value = axUInt16;

		Len(Value v = 0) : value(v) {}

		operator Len() const { return value; }

		static const Value kKey = 0x1A59;

		void onSerialize(axBinSerializer& se) {
			Value tmp = value ^ kKey;
			se.io_fixed(tmp);
		}

		void onSerialize(axBinDeserializer& se) {
			Value tmp = 0;
			se.io_fixed(tmp);
			value = tmp ^ kKey;
		}

		void onFormat(axFormat& f) const { f << value; }

		Value value;
	};

	Len len = 0;
	Cmd cmd = 0;

	template<class SE>
	void onSerialize(SE& se) {
		se.io(len);
		se.io(cmd);
	}

	void onFormat(axFormat& f) const {
		f.format("[len:{?}, cmd:{?}]", len, cmd);
	}
};

class axNetPacket : public axNonCopyable {
public:
	using Header    = axNetPacketHeader;
	using PacketLen = decltype(Header::len);

	static const axInt kMaxSize = 8 * 1000;
	using Buffer = axByteArray_<kMaxSize>;

	axNetPacket(Header::Cmd cmd) { _header.cmd = cmd; }

	template<class ENUM>
	axNetPacket(ENUM cmd) { _header.cmd = ax_enum_int(cmd); }

	const Header& header() const { return _header; }

	void writeBuffer(axIByteArray& buf, axInt extraPayloadLen = 0);
	void readBuffer(axByteSpan buf);

	virtual ~axNetPacket() {}

	template<class SE>
	void _onSerializeHeader(SE &se) {
		se.io(_header);
	}

	template<class SE> 
	void onSerialize(SE& se) {}

protected:
	inline virtual void _onSerialize(axBinSerializer&   se) { _onSerializeHeader(se); onSerialize(se); }
	inline virtual void _onSerialize(axBinDeserializer& se) { _onSerializeHeader(se); onSerialize(se); }

private:
	Header	_header;
};

