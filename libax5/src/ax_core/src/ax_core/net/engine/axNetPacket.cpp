#include "axNetPacket.h"

void axNetPacket::writeBuffer(axIByteArray& buf, axInt extraPayloadLen) {
	buf.clear();
	{
		axBinSerializer se(buf);
		_onSerialize(se);
	}

	PacketLen len;
	ax_safe_assign(len.value, buf.size() + extraPayloadLen);
	{
		using V = PacketLen::Value;
		axByteArray_<ax_sizeof(V)> tmp;
		axBinSerializer se(tmp);
		se.io(len);

		if (buf.size() < tmp.size())
			throw axError_Undefined(AX_LOC);
		
		*reinterpret_cast<V*>(buf.data()) = *reinterpret_cast<V*>(tmp.data());
	}
}

void axNetPacket::readBuffer(axByteSpan buf) {
	axBinDeserializer se(buf);

	auto cmd = _header.cmd;
	_onSerialize(se);
	AX_ASSERT(cmd == _header.cmd);
	_header.cmd = cmd;
}

