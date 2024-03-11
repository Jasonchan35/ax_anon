#include <ax_core/net/axSockAddr.h>
#include <ax_core/net/axSocket.h>

axIPv6::axIPv6(	axUInt8  v0, axUInt8  v1, axUInt8  v2, axUInt8  v3,
				axUInt8  v4, axUInt8  v5, axUInt8  v6, axUInt8  v7,
				axUInt8  v8, axUInt8  v9, axUInt8 v10, axUInt8 v11,
				axUInt8 v12, axUInt8 v13, axUInt8 v14, axUInt8 v15)
{
	_data[ 0] =  v0; _data[ 1] =  v1; _data[ 2] =  v2; _data[ 3] =  v3;
	_data[ 4] =  v4; _data[ 5] =  v5; _data[ 6] =  v6; _data[ 7] =  v7;
	_data[ 8] =  v8; _data[ 9] =  v9; _data[10] = v10; _data[11] = v11;
	_data[12] = v12; _data[13] = v13; _data[14] = v14; _data[15] = v15;
}

bool axIPv6::tryResolve(axStrView hostname) {
	axSockAddr addr;
	if (!addr.tryResolve(hostname, 0, axSockAddrFamily::IPv6))
		return false;
	*this = addr.ipv6();
	return true;
}

void axIPv6::onFormat(axFormat& f) const {
	for (axInt i = 0; i < kAddrBufSize; i += 2) {
		if (i > 0) f << ':';
		f.format("{?,2:0X}{?,2:0X}", _data[i], _data[i + 1]);
	}
}

#if 0
#pragma mark -------------------------------------------
#endif

axIPv4::axIPv4(axByte v0, axByte v1, axByte v2, axByte v3) {
	_data[0] = v0; _data[1] = v1; _data[2] = v2; _data[3] = v3;
}

bool axIPv4::tryResolve(axStrView hostname) {
	axSockAddr addr;
	if (!addr.tryResolve(hostname, 0, axSockAddrFamily::IPv4))
		return false;
	*this = addr.ipv4();
	return true;
}

void axIPv4::onFormat(axFormat& f) const {
	f.append(_data[0], '.', _data[1], '.', _data[2], '.', _data[3]);
}

#if 0
#pragma mark -------------------------------------------
#endif

void axSockAddr::onFormat(axFormat& f) const {
	auto fam = family();
	f << ax_enum(fam) << "(";
	switch (fam) {
		case Family::IPv4: { f << ipv4() << ":" << port(); }break;
		case Family::IPv6: { f << ipv6() << ":" << port(); }break;
		default: AX_ASSERT(false); break;
	}
	f << ")";
}

axIPv4 axSockAddr::ipv4() const {
	if (family() != Family::IPv4) {
		return axIPv4();
	}
	
	AX_STATIC_ASSERT(sizeof(axIPv4) == sizeof(_addr_in.sin_addr));
	return *reinterpret_cast<const axIPv4*>(&_addr_in.sin_addr);
}

axIPv6 axSockAddr::ipv6() const {
	if (family() != Family::IPv6) {
		return axIPv6();
	}
	AX_STATIC_ASSERT(sizeof(axIPv6) == sizeof(_addr_in6.sin6_addr));
	return *reinterpret_cast<const axIPv6*>(&_addr_in6.sin6_addr);
}

bool axSockAddr::isValid()	{
	return family() != Family::Unknown;
}

axUInt16 axSockAddr::port() const {
	switch (family()) {
		case Family::IPv4: {
			return ntohs(_addr_in.sin_port);
		}break;
		
		case Family::IPv6: {
			return ntohs(_addr_in6.sin6_port);
		}break;

		default:
			throw axError_Undefined(AX_LOC);
			return 0;
	}
}

axSockAddr::Family axSockAddr::family() const {
	switch (_addr.sa_family) {
		case AF_INET:	return Family::IPv4;
		case AF_INET6:	return Family::IPv6;
		default:		return Family::Unknown;
	}
}

void axSockAddr::setFamily(Family f) {
	_addr.sa_family = ax_enum_int(f);
}

void axSockAddr::set(const axIPv4& ip, axUInt16 port) {
	setFamily(Family::Unknown);
	_addr_in.sin_port = htons(port);
	_addr_in.sin_addr = ip._addr;
	setFamily(Family::IPv4);
}

void axSockAddr::set(const axIPv6& ip, axUInt16 port) {
	setFamily(Family::Unknown);
	_addr_in6.sin6_port = htons(port);
	_addr_in6.sin6_addr = ip._addr;
	setFamily(Family::IPv6);
}

void axSockAddr::resolve(axStrView hostnameAndPort, Family family) {
	if (!tryResolve(hostnameAndPort, family))
		throw axSocketError_ResolveHostname();
}


void axSockAddr::resolve(axStrView hostname, axUInt16 port, Family family) {
	if (!tryResolve(hostname, port, family))
		throw axSocketError_ResolveHostname();
}

bool axSockAddr::tryResolve(axStrView hostnameAndPort, Family family) {
	auto pair = hostnameAndPort.splitByChar(':');

	if (!pair.second) {
		throw axError_Undefined(AX_LOC);		
	}

	axUInt16 port = 0;
	if (!pair.second.tryParse(port))
		throw axError_Undefined(AX_LOC);

	return tryResolve(pair.first, port, family);
}

bool axSockAddr::tryResolve(axStrView hostname, axUInt16 port, Family family) {
	axSocket::platformInit();
	reset();

	axString_<512> hostnameStr(hostname);

	struct addrinfo hints = {};
	hints.ai_family = ax_enum_int(family);

	struct addrinfo* result = nullptr;

	if (getaddrinfo(hostnameStr.c_str(), nullptr, &hints, &result) != 0)
		return false;

	return _setByAddrInfo(*result, port);
}


bool axSockAddr::s_tryResolveMutliple(axIArray<axSockAddr>& outAddrs, axStrView hostname, axUInt16 port, Family family) {
	outAddrs.clear();
	axSocket::platformInit();

	axString_<512> hostnameStr(hostname);

	struct addrinfo hints = {};
	hints.ai_family = ax_enum_int(family);

	struct addrinfo* result = nullptr;

	if (getaddrinfo(hostnameStr.c_str(), nullptr, &hints, &result) != 0)
		return false;

	while (result) {
		auto& dst = outAddrs.emplaceBack();
		dst._setByAddrInfo(*result, port);
		result = result->ai_next;
	}
	return true;
}

bool axSockAddr::_setByAddrInfo(const addrinfo& v, axUInt16 port) {
	setFamily(Family::Unknown);
	
	switch (v.ai_family) {
		case AF_INET: {
			_addr_in = *reinterpret_cast<const sockaddr_in*>(v.ai_addr);
			_addr_in.sin_port = htons(port);
			setFamily(Family::IPv4);
		}break;

		case AF_INET6: {
			_addr_in6 = *reinterpret_cast<const sockaddr_in6*>(v.ai_addr);
			_addr_in6.sin6_port = htons(port);
			setFamily(Family::IPv6);
		}break;

		default:
			return false;
	}
	return true;
}
