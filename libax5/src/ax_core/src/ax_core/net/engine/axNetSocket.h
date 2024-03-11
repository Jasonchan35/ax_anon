#pragma once

#include "axNetPacket.h"

class axNetSocket : protected axDListNode<axNetSocket> {
	using This = axNetSocket;
public:
	using Bucket = axNetBucket;
	using Packet = axNetPacket;
	using PacketHeader = axNetPacketHeader;

	axNetSocket(axNetEngine* engine);
	virtual ~axNetSocket();

	void close();
	bool sendBytes(axByteSpan buf);

	template<class PACKET>
	bool sendPacket(PACKET& packet) {
		Packet::Buffer buf;
		packet.writeBuffer(buf);
		return sendBytes(buf);
	}

	void recv	(axIByteArray& buf, axInt maxRecvSize)	{ _sock.recv(buf, maxRecvSize); }
	void recv	(axIString&    buf, axInt maxRecvSize)	{ _sock.recv(buf, maxRecvSize); }

	bool fillBuffer	(axIByteArray& buf, axInt maxRecvSize)	{ return _sock.fillBuffer(buf, maxRecvSize); }

	AX_INLINE axInt	totalSendBytes() const { return _sock.totalSendBytes(); }
	AX_INLINE axInt	totalRecvBytes() const { return _sock.totalRecvBytes(); }

	enum class Status {
		Unknown,
		Listening,
		Connecting,
		Connected,
		Closing,
	};

	Status	status() const { return _status; }

	void setUserData(axTypedBase* p) { _userData = p; }
	axTypedBase* userData() { return _userData; }

friend class axDList<This>;
friend class axNetEngine;
protected:
	bool _onRecvPacket(PacketHeader& outHdr, axByteSpan& outBuf);

	axPollFlags		_pollFlags();
	axDList<Bucket>	_recvBuckets;
	axDList<Bucket>	_sendBuckets;
	axTcpSocket		_sock;
	Status			_status = Status::Unknown;
	axNetEngine*	_engine = nullptr;
	axTypedBase*	_userData = nullptr;
};

