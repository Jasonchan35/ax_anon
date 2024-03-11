#pragma once
#include "axSockAddr.h"
#include "../other/ax_poll.h"

#define axSocketType_EnumList(E) \
	E(Unknown,) \
	E(Stream,			= SOCK_STREAM) \
	E(Datagram,			= SOCK_DGRAM) \
	E(Raw,				= SOCK_RAW) \
	E(ReliableDatagram,	= SOCK_RDM) \
	E(SeqPacket,		= SOCK_SEQPACKET) \
//--------
ax_ENUM_CLASS(axSocketType, int)

//! cross-platform BSD socket class
class axSocket : public axNonCopyable {
	using This = axSocket;
public:
	using Family	= axSockAddrFamily;
	using Type		= axSocketType;
	using SockLen	= axSockAddr::SockLen;
	
#ifdef AX_OS_WINDOWS
	typedef SOCKET	Handle;
	static const Handle kInvalidHandle = INVALID_SOCKET;
#else
	typedef int		Handle;
	static const Handle kInvalidHandle = -1;
#endif
	
	~axSocket();

	Handle	handle()	{ return _h; }

	bool	isValid() const		{ return _h != kInvalidHandle; }
	explicit operator bool() const { return isValid(); }
	
	void	close				();
	void	shutdown			(bool read, bool write);

	void	setNonBlocking		(bool b);

	bool		pollRead		();
	axPollFlags	poll			(axInt timeoutMilliseconds = 0, axPollFlags eventFlags = axPollFlags::In | axPollFlags::Out);
	
	//! Gets the amount of data that has been received from the network and is available to be read.
	axInt 	availableBytes		();

	//! this value must set before connect / accept
	//! ( set the listen socket before bind for affect any socket created by accept )
    void	setRecvBufferSize	(axInt n);
	axInt	getRecvBufferSize	();

	//! this value must set before connect / accept
	//! ( set the listen socket before bind for affect any socket created by accept )
    void	setSendBufferSize	(axInt n);
	axInt	getSendBufferSize	();

	class PlatformInit {
	public:
		PlatformInit();
		~PlatformInit();
	};	
	static	void platformInit();

	AX_INLINE axInt	totalSendBytes() const { return _totalSendBytes; }
	AX_INLINE axInt	totalRecvBytes() const { return _totalRecvBytes; }
protected:
	axSocket() = default;

	void	_checkValid			();

	bool	_create				(Family domain, Type type, int protocol);

	axInt	_send				(axByteSpan buf, int flags = 0);
	axInt	_recv				(axByteMutSpan buf, int flags = 0);
	axInt	_appendRecv			(axIByteArray& buf, axInt maxRecvSize, int flags = 0);
	axInt	_appendRecv			(axIString8&   buf, axInt maxRecvSize, int flags = 0);
	
	axInt	_sendto				(const axSockAddr &addr, axByteSpan buf, int flags = 0);
	axInt	_recvfrom			(axSockAddr &addr, axByteMutSpan buf, int flags);
	axInt	_appendRecvfrom		(      axSockAddr &addr, axIByteArray& buf, axInt maxRecvSize, int flags = 0);

	void	_setsockopt			(int level, int optname, const void* optval, SockLen  optlen);
	void	_getsockopt			(int level, int optname,       void* optval, SockLen* optlen);
	int		_getSockError		();
    void	_checkSockError		();

	void	_shutdown			(int how);

	void	_move				(axSocket& r) { close(); _h = r._h; r._h = kInvalidHandle; }

	int 	_truncate_to_int(axInt v) {
		const auto max = axNumLimit<int>::max();
		return v < max ? static_cast<int>(v) : max;
	}

	size_t 	_truncate_to_size_t(axInt v) {
		if (v < 0) { AX_ASSERT(false); return 0; }
		const auto max = axNumLimit<size_t>::max();
		auto t = static_cast<size_t>(v);
		return t < max ? t : max;
	}

	axInt 	_to_axInt(int v) {
		AX_STATIC_ASSERT(sizeof(axInt) >= sizeof(v));
		return static_cast<axInt>(v);
	}

	axInt	_totalSendBytes = 0;
	axInt	_totalRecvBytes = 0;
	Handle _h = kInvalidHandle;
};

class axTcpSocket : public axSocket {
	using Base = axSocket;
	using This = axTcpSocket;
public:
	axTcpSocket() = default;
	axTcpSocket(axTcpSocket && r) noexcept { _move(r); }

		axInt	send			(axByteSpan buf, int flags = 0) { return _send(buf, flags); }
		axInt	send			(axStrView  str, int flags = 0) { return _send(str.byteSpan(), flags); }

		axInt	recv			(axIByteArray& buf, axInt maxRecvSize, int flags = 0) { buf.clear(); return _appendRecv(buf, maxRecvSize, flags); }
		axInt	recv			(axIString8&   buf, axInt maxRecvSize, int flags = 0) { buf.clear(); return _appendRecv(buf, maxRecvSize, flags); }

		axInt	appendRecv		(axIByteArray& buf, axInt maxRecvSize, int flags = 0) {              return _appendRecv(buf, maxRecvSize, flags); }
		axInt	appendRecv		(axIString8&   buf, axInt maxRecvSize, int flags = 0) {              return _appendRecv(buf, maxRecvSize, flags); }

		axInt	peekRecv		(axInt maxLen, axIByteArray& outBuf)	{ return recv(outBuf, MSG_PEEK); }


		//!  try recv, until buf.size() reach "reqLen" then return true
		bool	fillBuffer		(axIByteArray &buf, axInt reqLen);

		bool	connect			(const axSockAddr &addr);
		bool	connect			(axStrView hostname, axUInt16 port)		{ return connect(axSockAddr(hostname,  port)); }
		bool	connect			(axStrView hostnameAndPort)				{ return connect(axSockAddr(hostnameAndPort)); }

		bool	nonBlockingConnect(const axSockAddr& addr);
		bool	nonBlockingConnect(axStrView hostname, axUInt16 port)	{ return nonBlockingConnect(axSockAddr(hostname,  port)); }
		bool	nonBlockingConnect(axStrView hostnameAndPort)			{ return nonBlockingConnect(axSockAddr(hostnameAndPort)); }

		bool	isDisconnected();

		void	bind			(const axSockAddr &addr);
		void	bind			(axUInt16 port)						{ bind(axSockAddr("0.0.0.0", port)); }
		void	bind			(axStrView hostname, axUInt16 port)	{ bind(axSockAddr(hostname,  port)); }
		void	bind			(axStrView hostnameAndPort)			{ bind(axSockAddr(hostnameAndPort)); }

		void	listen			(axInt backlog = 64);
		bool	accept			(axTcpSocket& client);
	axSockAddr	getpeername		();

		void	bindAndListen	(const axSockAddr &addr)			{ bind(addr); listen(); }
		void	bindAndListen	(axUInt16 port)						{ bindAndListen(axSockAddr("0.0.0.0", port)); }
		void	bindAndListen	(axStrView hostname, axUInt16 port)	{ bindAndListen(axSockAddr(hostname,  port)); }
		void	bindAndListen	(axStrView hostnameAndPort)			{ bindAndListen(axSockAddr(hostnameAndPort)); }

		void	setReuseAddr	(bool b);
		void	setNoDelay		(bool b);
		void	setKeepAlive	(bool b);
};

class axUdpSocket : public axSocket {
	using Base = axSocket;
public:
	axUdpSocket() = default;
	axUdpSocket(axUdpSocket && r) noexcept { _move(r); }

	bool	createIPv4()	{ return _create(Family::IPv4, Type::Datagram, 0); }
	bool	createIPv6()	{ return _create(Family::IPv6, Type::Datagram, 0); }

	axInt	sendTo			(const axSockAddr& addr, axByteSpan buf, int flags = 0) { return _sendto(addr, buf, flags); }
	axInt	recvFrom		(      axSockAddr& addr, axIByteArray& buf, axInt maxRecvSize, int flags = 0) { buf.clear(); return _appendRecvfrom(addr, buf, maxRecvSize, flags); }
	axInt	appendRecvFrom	(      axSockAddr& addr, axIByteArray& buf, axInt maxRecvSize, int flags = 0) {              return _appendRecvfrom(addr, buf, maxRecvSize, flags); }
};
