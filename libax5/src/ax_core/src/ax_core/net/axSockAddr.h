#pragma once

#include "../string/axFormat.h"

class axSocket;

//--------------------
class axSocketError : public axError {};
class axSocketError_Undefined					: public axSocketError {};
class axSocketError_InvalidHandle				: public axSocketError {};
class axSocketError_Disconnected				: public axSocketError {};
class axSocketError_ResolveHostname				: public axSocketError {};

#define axSockAddrFamily_EnumList(E) \
	E(Unknown, = AF_UNSPEC) \
	E(IPv4, = AF_INET) \
	E(IPv6, = AF_INET6) \
//----

ax_ENUM_CLASS(axSockAddrFamily, axTypeTraits::typeBySize<sizeof(sockaddr::sa_family)>::UIntType)

class axIPv6 {
public:
	using Addr = decltype(sockaddr_in6::sin6_addr);
	static constexpr axInt kAddrBufSize = 16;

	axIPv6() { _data64[0] = 0; _data64[1] = 0; }
	axIPv6(	axUInt8  v0, axUInt8  v1, axUInt8  v2, axUInt8  v3,
			axUInt8  v4, axUInt8  v5, axUInt8  v6, axUInt8  v7,
			axUInt8  v8, axUInt8  v9, axUInt8 v10, axUInt8 v11,
			axUInt8 v12, axUInt8 v13, axUInt8 v14, axUInt8 v15);

			void	resolve   (axStrView hostname) { if (!tryResolve(hostname)) throw axSocketError_ResolveHostname(); }
			bool	tryResolve(axStrView hostname);

	bool operator==(const axIPv6& r) const { return _data64[0] == r._data64[0]
												&&  _data64[1] == r._data64[1]; }
	void onFormat(axFormat& f) const;
	
	union {
		Addr	 _addr;
		axUInt8  _data[kAddrBufSize];
		axUInt64 _data64[2];
	};
};

AX_STATIC_ASSERT(ax_sizeof(axIPv6) == axIPv6::kAddrBufSize);

class axIPv4 {
public:
	using Addr = decltype(sockaddr_in::sin_addr);
	static constexpr axInt kAddrBufSize = 4;

	axIPv4() { _data32[0] = 0; }
	axIPv4(axUInt8 v0, axUInt8 v1, axUInt8 v2, axUInt8 v3);

			void	resolve   (axStrView hostname) { if (!tryResolve(hostname)) throw axSocketError_ResolveHostname(); }
			bool	tryResolve(axStrView hostname);

	bool operator==(const axIPv4& r) const { return _data32[0] == r._data32[0]; }
	void onFormat(axFormat& f) const;

	union {
		Addr	 _addr;
		axUInt8  _data[kAddrBufSize];
		axUInt32 _data32[1];
	};
};

AX_STATIC_ASSERT(ax_sizeof(axIPv4) == axIPv4::kAddrBufSize);

class axSockAddr {
public:
	using Family = axSockAddrFamily;

#if AX_OS_WINDOWS
	using SockLen = int;
#else
	using SockLen = socklen_t;
#endif

	axSockAddr() { reset(); }
	axSockAddr(const axIPv4& ip, axUInt16 port)	{ set(ip, port); }
	axSockAddr(const axIPv6& ip, axUInt16 port)	{ set(ip, port); }

	axSockAddr(axStrView hostnameAndPort,         Family family = Family::IPv4)	{ resolve(hostnameAndPort, family); }
	axSockAddr(axStrView hostname, axUInt16 port, Family family = Family::IPv4)	{ resolve(hostname, port,  family); }

			void	set(const axIPv4& ip, axUInt16 port);
			void	set(const axIPv6& ip, axUInt16 port);

			void	resolve(axStrView hostnameAndPort,         Family family = Family::IPv4);
			void	resolve(axStrView hostname, axUInt16 port, Family family = Family::IPv4);

			bool	tryResolve(axStrView hostnameAndPort,         Family family = Family::IPv4);
			bool	tryResolve(axStrView hostname, axUInt16 port, Family family = Family::IPv4);

	static	bool	s_tryResolveMutliple(axIArray<axSockAddr>& outAddrs, axStrView hostname, axUInt16 port, Family family = Family::IPv4);

	void reset() { _addr.sa_family = 0; }

	axIPv4		ipv4	() const;
	axIPv6		ipv6	() const;
	axUInt16	port	() const;

	Family		family	() const;
	void		setFamily(Family f);

	bool		isValid	();
	
	void		onFormat(axFormat &f) const;
	
	      sockaddr* _sockaddr()       { return &_addr; }
	const sockaddr* _sockaddr() const { return &_addr; }

	SockLen   _addrlen() const {
		switch (family()) {
			case Family::IPv4: return sizeof(sockaddr_in);
			case Family::IPv6: return sizeof(sockaddr_in6);
			default: return 0;
		}
	}

	static const axInt kAddrBufSize = 32;
friend class axSocket;
protected:
	bool _setByAddrInfo(const addrinfo& v, axUInt16 port);

	union {
		sockaddr		_addr;
		sockaddr_in		_addr_in;
		sockaddr_in6	_addr_in6; // sizeof(sockaddr_in6) > sizeof(sockaddr), so we can't use sockaddr to store
		axUInt8			_data[kAddrBufSize];
	};
};

AX_STATIC_ASSERT(ax_sizeof(axSockAddr) == axSockAddr::kAddrBufSize);
/*
void axSockAddr::serialize_io(axSerializer &s) {
//	BSD/UNIX sa_family defined as "unsigned char" (8 bit ) but Windows/Linux is "u_short" (16 bit)
//	and AF_MAX(32 on Windows, 37 on BSD, 35 on Linux) not over 256 (8bit unsigned max), so just serialize 8bit here is fine

	auto f = family();
	s.io(f);

	switch (f) {
		case Family::IPV4: { auto a = ipv4(); s.io(a); }break;
		case Family::IPV6: { auto a = ipv6(); s.io(a); }break;
	}
}
*/
