#include "axNetEngine.h"
#include <ax_core/other/axEnum.h>


axNetEngine::~axNetEngine() {
	closeAll();
}

void axNetEngine::listen(const axSockAddr& addr) {
	auto s = axUPtr_new(Socket(this));
	s->_status = Socket::Status::Listening;
	s->_sock.bindAndListen(addr);
	_listenSocks.append(s.ptr());
	s.detach();
}

void axNetEngine::listen(axUInt16 port) {
	listen("0.0.0.0", port);
}

void axNetEngine::listen(axStrView hostname, axUInt16 port) {
	listen(axSockAddr(hostname, port));
}

void axNetEngine::stopListen() {
	for (auto& s : _listenSocks) {
		s.close();
	}
}

axNetSocket* axNetEngine::connect(const axSockAddr& addr) {
	auto s = axUPtr_new(Socket(this));
	s->_status = Socket::Status::Connecting;
	try {
		s->_sock.nonBlockingConnect(addr);
		_connections.append(s.ptr());
		return s.detach();
	} catch(...) {
		_onErrorConnect(s.ptr());
	}
	return nullptr;
}

axNetSocket* axNetEngine::connect(axStrView hostname, axUInt16 port) {
	return connect(axSockAddr(hostname, port));
}

void axNetEngine::_addToPollFds(axDList<Socket>& list) {
	Socket* s = list.head();
	Socket* next = nullptr;
	for (;s ; s = next) {
		next = s->listNode_next;

		if (!s->_sock.isValid()) {
			s->removeFromList();
			ax_delete(s);
			continue;
		}

		_pollSocks.emplaceBack(s);
		auto* pf = &_pollfds.emplaceBack();
		pf->fd = s->_sock.handle();
		pf->events = ax_enum_int(s->_pollFlags());
		pf->revents = 0;
	}
}

void axNetEngine::update(axInt timeoutMilliseconds) {
	{
		_pollSocks.clear();
		_pollfds.clear();

		axInt n = _connections.size() + _listenSocks.size();
		_pollSocks.reserve(n);
		_pollfds.reserve(n);
	}
	
	_addToPollFds(_listenSocks);
	_addToPollFds(_connections);

	if (_pollfds.size() <= 0)
		return;

	unsigned int len = 0;
	int t = _truncate_to_int(timeoutMilliseconds);
	ax_safe_assign(len, _pollfds.size());

	//------ Poll -------
	int ret = ax_poll(_pollfds.data(), len, t);
	if (ret < 0) {
		AX_ASSERT(false);
		return;
	}

	if (ret == 0) return;

	{
		AX_ASSERT(_pollfds.size() == _pollSocks.size());

		auto* pf = _pollfds.data();
		for (auto& s : _pollSocks) {
			if (pf->revents) {
				try {
					_updateSocket(s, axPollFlags(pf->revents));

				} catch(axError& error) {
					_onError(s, error);

				} catch(ax_exception& e) {
					AX_LOG("axNetEngine _update_connection exception: {?}", e);
					auto err = axError_StdException(AX_LOC, e);
					_onError(s, err);
				}
			}
			pf++;
		}
	}
}

void axNetEngine::closeAll() {
	for (auto& c : _connections) {
		c.close();
	}
}

void axNetEngine::sendBytesToAll(axByteSpan data) {
	for (auto& c : _connections) {
		if (!c.sendBytes(data)) {
			// c.close();
		}
	}
}

void axNetEngine::sendPacketToAll(Packet& packet) {
	if (!_connections.size()) return;
	axNetPacket::Buffer buf;
	packet.writeBuffer(buf);

	AX_DUMP_VAR(buf.size());
	sendBytesToAll(buf);
}

axUPtr<axNetEngine::Bucket> axNetEngine::getBucket() {
	if (auto p = _freeBuckets.popHead()) {
		p->reset();
		return p;
	}
	return axUPtr_new(Bucket());
}

void axNetEngine::_updateSocket(Socket* s, axPollFlags revents) {
	auto ev = ax_enum(revents);

	switch (s->_status) {
		case Socket::Status::Listening: {
			if (ev.hasFlags(axPollFlags::In)) {
				auto c = axUPtr_new(Socket(this));
				if (!s->_sock.accept(c->_sock))
					return;

				c->_status = Socket::Status::Connected;
				c->_sock.setNonBlocking(true);
				_connections.append(c.ptr());
				_onAccept(c.detach());
				return;
			}
		}break;

		case Socket::Status::Connecting: {
			if (ev.hasFlags(axPollFlags::Hup) || ev.hasFlags(axPollFlags::Error)) {
				_onErrorConnect(s);
				return;

			} else if (ev.hasFlags(axPollFlags::Out)) {
				s->_status = Socket::Status::Connected;
				_onConnect(s);
				return;
			}
		}break;

		case Socket::Status::Connected: {
			if (ev.hasFlags(axPollFlags::Hup) || ev.hasFlags(axPollFlags::Error)) {
				_onDisconnect(s);
				return;
			}

			if (ev.hasFlags(axPollFlags::In)) {
				if (s->_sock.availableBytes() == 0) {
					_onDisconnect(s);
					return;
				}
				_onRecv(s);
			}

			if (ev.hasFlags(axPollFlags::Out)) {
				_onSend(s);
			}
		}break;

		default: AX_ASSERT(false); break;
	}
}

void axNetEngine::onRecv(Socket* s) {
	PacketHeader hdr;
	axByteSpan buf;

	const axInt maxPacketPerFrame = 10000;

	for (axInt i = 0; i < maxPacketPerFrame; i++) {
		if (!s->_onRecvPacket(hdr, buf))
			break;
		onRecvPacket(s, hdr, buf);

		auto p = s->_recvBuckets.popHead();
		p->reset();
		_freeBuckets.append(ax_move(p));
	}
}

void axNetEngine::_onAccept(Socket* s) {
	try {
		onAccept(s);
	} catch(axError& err) {
		AX_LOG_ERROR("onAccept {?}", err);
		_destroy(s);
	}
}

void axNetEngine::_onConnect(Socket* s) {
	try {
		onConnect(s);
	} catch(axError& err) {
		AX_LOG_ERROR("onConnect {?}", err);
		_destroy(s);
	}
}

void axNetEngine::_onErrorConnect(Socket* s) {
	try {
		onErrorConnect(s);
	} catch(axError& err) {
		AX_LOG_ERROR("onErrorConnect {?}", err);
		_destroy(s);
		return;
	}
	_destroy(s);
}

void axNetEngine::_onDisconnect(Socket* s) {
	try {
		onDisconnect(s);
	} catch(axError& err) {
		AX_LOG_ERROR("onDisconnect {?}", err);
		_destroy(s);
		return;
	}
	_destroy(s);
}

void axNetEngine::_onRecv(Socket* s) {
	try {
		onRecv(s);
	} catch(axError& err) {
		AX_LOG_ERROR("onRecv {?}", err);
		_destroy(s);
	}
}

void axNetEngine::_onSend(Socket* s) {
	try {
		auto* p = s->_sendBuckets.head();
		if (!p) return;
	
		auto buf = p->span();
		if (buf.size() <= 0) return;

		auto sentLen = s->_sock.send(buf);
		if (sentLen < 0) {
			throw axError_Undefined(AX_LOC);
		}

		p->advance(sentLen);	
		if (p->dataSize() <= 0) {
			p->reset();
			s->_sendBuckets.remove(p);
			_freeBuckets.append(p);
		}	
	} catch(...) {
		_destroy(s);
	}
}

void axNetEngine::_onError(Socket* s, axError& error) {
	try {
		onError(s, error);
	} catch(...) {
		_destroy(s);
		return;
	}

	_destroy(s);
}

void axNetEngine::_destroy(Socket* s) {
	onDestroy(s);
	s->removeFromList();
	s->close();
	ax_delete(s);
}
