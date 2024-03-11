#include "axNetSocket.h"
#include "axNetEngine.h"

axNetSocket::axNetSocket(axNetEngine* engine)
	: _engine(engine)
{
}

axNetSocket::~axNetSocket() {
//	AX_ASSERT(_userData == nullptr);
}

void axNetSocket::close() {
	_status = Status::Closing;
	_sock.close();
}

bool axNetSocket::sendBytes(axByteSpan buf) {
	if (auto* tail = _sendBuckets.tail()) {
		if (tail->tryAppend(buf))
			return true;
	}
	auto bucket = _engine->getBucket();
	AX_ASSERT(bucket->dataSize() == 0);
	bucket->appendData(buf);
	_sendBuckets.append(ax_move(bucket));
	return true;
}

bool axNetSocket::_onRecvPacket(PacketHeader& outHdr, axByteSpan& outBuf) {
	outBuf = axByteSpan();

	auto* p = _recvBuckets.head();
	if (!p) {
		auto u = _engine->getBucket();
		AX_ASSERT(u->dataSize() == 0);
		u->reset();
		p = _recvBuckets.append(ax_move(u));
	}

	const auto reqLen = ax_sizeof(PacketHeader::Len::Value);

	if (p->dataSize() < reqLen) {
		if (!fillBuffer(p->buf, reqLen))
			return false;
	}

	PacketHeader::Len packetLen = 0;
	{
		axBinDeserializer se(p->buf);
		se.io(packetLen);
	}

	if (packetLen.value > Packet::kMaxSize || packetLen.value < reqLen)
		throw axError_Undefined(AX_LOC);
	
	if (!fillBuffer(p->buf, packetLen.value))
		return false;

	{
		axBinDeserializer se(p->buf);
		se.io(outHdr);
	}

	outBuf = p->buf;
	return true;
}

axPollFlags axNetSocket::_pollFlags() {
	auto m = axPollFlags::None;

	switch (_status) {
		case Status::Listening: {
			m |= axPollFlags::In;
		}break;

		case Status::Connecting: {
			m |= axPollFlags::In | axPollFlags::Out;
		}break;

		case Status::Connected: {
			m |= axPollFlags::In;
			if (_sendBuckets.size() > 0) {
				m |= axPollFlags::Out;
			}
		}break;
	}

	return m;
}