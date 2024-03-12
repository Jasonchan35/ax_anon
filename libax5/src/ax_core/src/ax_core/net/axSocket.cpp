#include <ax_core/net/axSocket.h>
#include <ax_core/other/axEnum.h>
#include <ax_core/other/axGlobalSingleton.h>

axSocket::~axSocket() {
	close();
}


void axSocket::_checkValid() {
	if (!isValid())
		throw axSocketError_InvalidHandle();
}

bool axSocket::_create(Family domain, Type type, int protocol) {
	axSocket::platformInit();
	close();
	_h = ::socket(ax_enum_int(domain), ax_enum_int(type), protocol);
	if (!isValid()) {
		throw axError_Undefined(AX_LOC);
	}
	return true;
}

void axSocket::shutdown(bool read, bool write) {
	int how = 0;
#if AX_OS_WINDOWS
	if (read ) how |= SD_RECEIVE;
	if (write) how |= SD_SEND;
#else
	if (read ) how |= SHUT_RD;
	if (write) how |= SHUT_WR;
#endif
	_shutdown(how);
}

int axSocket::_getSockError() {
	int errorCode = 0;
	SockLen optlen = sizeof(errorCode);
	_getsockopt(SOL_SOCKET, SO_ERROR, &errorCode, &optlen);
	return errorCode;
}

axPollFlags axSocket::poll(axInt timeoutMilliseconds, axPollFlags eventFlags) {
	_checkValid();

	axPollFd p;
	p.fd = _h;
	p.events = ax_enum_int(eventFlags);
	p.revents = 0;

	int ret = ax_poll(&p, 1, _truncate_to_int(timeoutMilliseconds));
	if (ret <= 0)
		return axPollFlags(0);

	return axPollFlags(p.revents);
}

void axSocket::setRecvBufferSize(axInt n) {
	_checkValid();

	int optval = _truncate_to_int(n);
	_setsockopt(SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval));
}

axInt axSocket::getRecvBufferSize() {
	_checkValid();

	int optval = 0;
	SockLen optlen = sizeof(optval);
	_getsockopt( SOL_SOCKET, SO_RCVBUF, &optval, &optlen);
	return _to_axInt(optval);
}

void axSocket::setSendBufferSize(axInt n) {
	_checkValid();

	int optval = _truncate_to_int(n);
	_setsockopt(SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval));
}

axInt axSocket::getSendBufferSize() {
	_checkValid();

	int optval = 0;
	SockLen optlen = sizeof(optval);
	_getsockopt( SOL_SOCKET, SO_SNDBUF, &optval, &optlen);
	return _to_axInt(optval);
}

axInt axSocket::_send(axByteSpan buf, int flags) {
	_checkValid();

#if AX_OS_WINDOWS
	int len = _truncate_to_int(buf.size());
	int ret = 0;
#else
	size_t len = _truncate_to_size_t(buf.size());
	ssize_t ret = 0;
#endif
	
	ret = ::send(_h, (const char*)buf.data(), len, flags);
	if (ret < 0) {
		_checkSockError();
		return 0;
	}
	
	axInt outLen = 0;
	ax_safe_assign(outLen, ret);

	_totalSendBytes += outLen;
	return outLen;
}

axInt axSocket::_sendto(const axSockAddr &addr, axByteSpan buf, int flags) {
	_checkValid();

#if AX_OS_WINDOWS
	int len = _truncate_to_int(buf.size());
	int ret = 0;
#else
	size_t len = _truncate_to_size_t(buf.size());
	ssize_t ret = 0;
#endif

	ret = ::sendto(_h, (const char*)buf.data(), len, flags, addr._sockaddr(), addr._addrlen());
	if (ret < 0) {
		_checkSockError();
		return 0;
	}
	
	axInt outLen = 0;
	ax_safe_assign(outLen, ret);

	_totalSendBytes += outLen;
	return outLen;
}

axInt axSocket::_recv(axByteMutSpan buf, int flags) {

#if AX_OS_WINDOWS
	int len = _truncate_to_int(buf.size());
	int ret = 0;
#else
	size_t len = _truncate_to_size_t(buf.size());
	ssize_t ret = 0;
#endif

	ret = ::recv(_h, (char*)buf.data(), len, flags);
	if (ret < 0) {
		_checkSockError();
		return 0;
	}

	axInt outLen = 0;
	ax_safe_assign(outLen, ret);

	_totalRecvBytes += outLen;
	return outLen;
}

axInt axSocket::_recvfrom(axSockAddr &addr, axByteMutSpan buf, int flags) {
#if AX_OS_WINDOWS
	int len = _truncate_to_int(buf.size());
	int ret = 0;
#else
	size_t len = _truncate_to_size_t(buf.size());
	ssize_t ret = 0;
#endif

	SockLen addrlen = addr._addrlen();
	ret = ::recvfrom(_h, (char*)buf.data(), len, flags, addr._sockaddr(), &addrlen);
	if (ret < 0) {
		_checkSockError();
		return 0;
	}

	axInt outLen = 0;
	ax_safe_assign(outLen, ret);

	_totalRecvBytes += outLen;
	return outLen;
}

axInt axSocket::_appendRecv(axIByteArray& buf, axInt maxRecvSize, int flags) {
	_checkValid();

	if (maxRecvSize <  0) { AX_ASSERT(false); return 0; }
	if (maxRecvSize <= 0) return 0;

	auto oldSize = buf.size();
	auto newSize = oldSize + maxRecvSize;
	if (newSize < oldSize)
		return 0; //overflow

	buf.resize(newSize);
	axInt outLen = _recv(buf.sliceFrom(oldSize), flags);
	if (outLen < 0) {
		buf.resize(oldSize);
		return 0;
	}
	buf.resize(oldSize + outLen);
	return outLen;
}

axInt axSocket::_appendRecv(axIString8& buf, axInt maxRecvSize, int flags) {
	_checkValid();

	if (maxRecvSize <  0) { AX_ASSERT(false); return 0; }
	if (maxRecvSize <= 0) return 0;

	auto oldSize = buf.size();
	auto newSize = oldSize + maxRecvSize;
	if (newSize < oldSize)
		return 0; //overflow

	buf.resize(newSize);
	axInt outLen = _recv(buf.sliceFrom(oldSize).byteSpan(), flags);
	if (outLen < 0) {
		buf.resize(oldSize);
		return 0;
	}
	buf.resize(oldSize + outLen);
	return outLen;
}

axInt axSocket::_appendRecvfrom(axSockAddr &addr, axIByteArray& buf, axInt maxRecvSize, int flags) {
	_checkValid();

	if (maxRecvSize <  0) { AX_ASSERT(false); return 0; }
	if (maxRecvSize <= 0) return 0;

	auto oldSize = buf.size();
	auto newSize = oldSize + maxRecvSize;
	if (newSize < oldSize)
		return 0; //overflow

	buf.resize(newSize);	
	axInt outLen = _recvfrom(addr, buf.sliceFrom(oldSize), flags);
	if (outLen < 0) {
		buf.resize(oldSize);
		return 0;
	}
	buf.resize(oldSize + outLen);
	return outLen;
}

void axSocket::_shutdown(int how) {
	int ret = ::shutdown(_h, how);
	if (ret != 0)
		AX_ASSERT(false);
}

void axSocket::_setsockopt(int level, int optname, const void* optval, SockLen optlen) {
	_checkValid();

	int ret = ::setsockopt(_h, level, optname, (const char*)optval, optlen );
	if (ret != 0)
		throw axError_Undefined(AX_LOC);
}

void axSocket::_getsockopt(int level, int optname, void* optval, SockLen* optlen) {
	_checkValid();

	int ret = ::getsockopt(_h, level, optname, (char*) optval, optlen);
	if (ret != 0)
		throw axError_Undefined(AX_LOC);
}

axSockAddr axTcpSocket::getpeername() {
	_checkValid();

	axSockAddr outAddr;

	SockLen namelen = axSockAddr::kAddrBufSize;
	int ret = ::getpeername(_h, outAddr._sockaddr(), &namelen);
	if (ret != 0) {
		AX_ASSERT(false);
		return axSockAddr();
	}
	return outAddr;
}

void axTcpSocket::bind(const axSockAddr &addr) {
	if (!isValid()) {
		_create(addr.family(), Type::Stream, 0);
	}

	setReuseAddr(true);

	int ret = ::bind(_h, addr._sockaddr(), addr._addrlen());
	if (ret < 0) {
		throw axError_Undefined(AX_LOC);
	}
}

void axTcpSocket::listen(axInt backlog) {
	_checkValid();

	int ret = ::listen(_h, _truncate_to_int(backlog));
	if (ret < 0) {
		throw axError_Undefined(AX_LOC);
	}
}

bool axTcpSocket::fillBuffer(axIByteArray &buf, axInt reqLen) {
	_checkValid();

	auto oldSize = buf.size();
	if (oldSize > reqLen) {
		AX_ASSERT(false);
		return true;
	}

	auto remain = reqLen - oldSize;
	if (remain <= 0) return true;

	axInt recvLen = appendRecv(buf, remain, 0);
	if (recvLen < remain) {
		return false;
	}
	return true;
}

bool axTcpSocket::accept(axTcpSocket& client) {
	_checkValid();

	client.close();
	client._h = ::accept(_h, nullptr, nullptr);
	if (client._h == kInvalidHandle) {
		return false;
	}
	return true;
}

bool axTcpSocket::connect(const axSockAddr &addr) {
	if (!isValid()) {
		_create(addr.family(), Type::Stream, 0);
	}

	int ret = ::connect(_h, addr._sockaddr(), addr._addrlen());
	if (ret < 0) {
		_checkSockError();
		return false;
	}
	return true;
}

bool axTcpSocket::nonBlockingConnect(const axSockAddr& addr) {
	if (!isValid()) {
		_create(addr.family(), Type::Stream, 0);
	}
	setNonBlocking(true);
	return connect(addr);
}

bool axTcpSocket::isDisconnected() {
	if (!isValid())
		return true;

	auto ev = ax_enum(poll(0));
	if (ev.hasFlags(axPollFlags::Hup))
		return true;

	if (ev.hasFlags(axPollFlags::In)) {
		axByte buf[1];
		auto ret = _recv(buf, MSG_PEEK);
		if (ret == 0)
			return true; // disconnected if recv len = 0
	}
	return false;
}

void axTcpSocket::setReuseAddr(bool b) {
	_checkValid();

	int a = b ? 1 : 0;

#if !AX_OS_WINDOWS
	_setsockopt(SOL_SOCKET, SO_REUSEPORT, &a, sizeof(a));
#endif
	_setsockopt(SOL_SOCKET, SO_REUSEADDR, &a, sizeof(a));
}

void axTcpSocket::setNoDelay( bool b ) {
	_checkValid();

	int a = b ? 1 : 0;
	_setsockopt(IPPROTO_TCP, TCP_NODELAY, &a, sizeof(a));
}

void axTcpSocket::setKeepAlive( bool b ) {
	_checkValid();

	int a = b ? 1 : 0;
	_setsockopt(SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(a));
}

void axSocket::close() {
	if (!isValid()) return;

#if AX_OS_WINDOWS
	::closesocket(_h);
#else
	::close(_h);
#endif
	_h = kInvalidHandle;
}

axInt axSocket::availableBytes() {
	if (!isValid()) { AX_ASSERT(false); return 0; }

#if AX_OS_WINDOWS
	unsigned long s = 0;
	int ret = ::ioctlsocket(_h, FIONREAD, &s);
#else
	int s = 0;
	int ret = ::ioctl(_h, FIONREAD, &s);
#endif
	if (ret != 0) {
		AX_ASSERT(false);
		return 0;
	}

	axInt outValue;
	ax_safe_assign(outValue, s);
	return outValue;
}

void axSocket::setNonBlocking(bool b) {
	_checkValid();

#if AX_OS_WINDOWS
	unsigned long a = b ? 1 : 0;
	int ret = ::ioctlsocket(_h, FIONBIO, &a);
#else
	int a = b ? 1 : 0;
	int ret = ::ioctl(_h, FIONBIO, &a );
#endif

	if (ret != 0) {
		AX_ASSERT(false);
	}
}

bool axSocket::pollRead() {
	auto ev = poll(0, axPollFlags::In);
	if (ax_enum(ev).hasFlags(axPollFlags::In))
		return true;
	return false;
}

void axSocket::_checkSockError() {
#if AX_OS_WINDOWS
	int	err = WSAGetLastError();
	switch( err ) {
		case 0:
		case WSAEINPROGRESS:
		case WSAENOBUFS:
		case WSAEWOULDBLOCK:
			return;

		case WSAENETDOWN:
		case WSAENETRESET:
		case WSAEHOSTUNREACH:
		case WSAECONNABORTED:
		case WSAECONNRESET:
		case WSAETIMEDOUT:
		case WSAECONNREFUSED:
		default:
			throw axError_Undefined(AX_LOC);
	}
#else
	int err = errno;
	switch( err ) {
		case 0:
	//	case EAGAIN:			// Resource temporarily unavailable
		case EWOULDBLOCK:		// Operation would block
		case EINPROGRESS:		// Operation now in progress
		case EINTR:				// Interrupt occurred, can recover
		case ENOBUFS:			// System unable to allocate buf now, can recover
			return;

		case ENOTCONN:
		case EHOSTDOWN:
		case EPIPE:
		default:
			throw axError_Undefined(AX_LOC);
	}
#endif
};

#if 0
#pragma mark ================= axSocket::PlatformInit =================
#endif

void axSocket::platformInit() {
	static axGlobalSingleton<PlatformInit> s;
}

axSocket::PlatformInit::PlatformInit() {
#if AX_OS_WINDOWS
	WSADATA	wsad;
	if (WSAStartup(WINSOCK_VERSION, &wsad) != 0) {
		AX_ASSERT(false);
	}
#endif
}

axSocket::PlatformInit::~PlatformInit() {
#if AX_OS_WINDOWS
	//if (WSACleanup() != 0) {
	//	AX_ASSERT(false);
	//}
#endif
}
