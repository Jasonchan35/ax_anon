#pragma once

#include "axNetSocket.h"

class axNetEngine : public axNonCopyable {
public:
	using Socket = axNetSocket;
	using Bucket = axNetBucket;
	using Packet = axNetPacket;
	using PacketHeader = axNetPacketHeader;

	virtual ~axNetEngine();

			void listen		(axUInt16 port);
			void listen		(const axSockAddr& addr);
			void listen		(axStrView hostname, axUInt16 port);

			void stopListen	();

	axNetSocket* connect	(const axSockAddr& addr);
	axNetSocket* connect	(axStrView hostname, axUInt16 port);

			void update		(axInt timeoutMilliseconds = 0);
			void closeAll	();

			void sendStringToAll(axStrView  str) { sendBytesToAll(str.byteSpan()); }
			void sendBytesToAll	(axByteSpan data);

			void sendPacketToAll(Packet& packet);

			axUPtr<Bucket>	getBucket();

protected:
	virtual void onAccept		(Socket* s) {}
	virtual void onConnect		(Socket* s) {}
	virtual void onErrorConnect	(Socket* s) {}
	virtual void onDisconnect	(Socket* s) {}

	virtual void onRecvPacket	(Socket* s, PacketHeader& hdr, axByteSpan buf) {}
	virtual void onRecv			(Socket* s);
	virtual void onSend			(Socket* s) {}

	virtual void onError		(Socket* s, axError& error) {}
	virtual void onDestroy		(Socket* s) {}

private:
			void _onAccept		(Socket* s);
			void _onConnect		(Socket* s);
			void _onErrorConnect(Socket* s);
			void _onDisconnect	(Socket* s);
			void _onRecv		(Socket* s);
			void _onSend		(Socket* s);
			void _onError		(Socket* s, axError& error);

			void _destroy		(Socket* s);

	int 	_truncate_to_int(axInt v) noexcept {
		const auto max = axNumLimit<int>::max();
		return v < max ? static_cast<int>(v) : max;
	}

	void _addToPollFds(axDList<Socket>& list);
	void _updateSocket(Socket* s, axPollFlags revents);

	axDList<Bucket>			_freeBuckets;
	axDList<Socket>			_listenSocks;
	axDList<Socket>			_connections;

	axArray<Socket*>		_pollSocks;
	axArray<axPollFd>		_pollfds;
};
