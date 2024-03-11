#pragma once
#include "../Core/AnonObject.h"

// +------------+                           +-------------+
// |            | outPlug            InPLug |             |
// |  outSocket |>-----> Connection >------>| InSocket    |
// |            |                           |             |
// |            |                           |             |
// +------------+                           +-------------+



namespace Anon {

class DiaNode;
class DiaInSocket;
class DiaOutSocket;
class DiaConnectionNode;
class DiaConnection;
class DiaInPlug;
class DiaOutPlug;

class DiaInPlug : public axDListNode< DiaInPlug > {
public:
	~DiaInPlug() { removeFromList(); }

	using Socket = DiaInSocket;

			DiaOutPlug& outPlug();
	const	DiaOutPlug& outPlug() const;

	void connect(Socket* sock, const AnonFieldPath& fieldPath);

			DiaNode* diaNode();
	const	DiaNode* diaNode() const { return ax_const_cast(this)->diaNode(); }

			DiaConnection& connection();
	const	DiaConnection& connection() const { return ax_const_cast(this)->connection(); }

	void	getString(axIString& outStr) const;

	AnonFieldValuePtr	getFieldValuePtr();

	Socket*	sock() { return _sock; }

	const AnonFieldPath&	fieldPath() { return _fieldPath; }
	const axFieldInfo*		fieldInfo() { return _fieldPath.fieldInfo; }

	void onFormat(axFormat& f) const;

private:
	Socket*			_sock = nullptr;
	AnonFieldPath	_fieldPath;
};

class DiaOutPlug : public axDListNode< DiaOutPlug > {
public:
	~DiaOutPlug() { removeFromList(); }

	using Socket = DiaOutSocket;

			DiaInPlug& inPlug();
	const	DiaInPlug& inPlug() const;

	void connect(Socket* sock, const AnonFieldPath& fieldPath);

			DiaNode* diaNode();
	const	DiaNode* diaNode() const { return ax_const_cast(this)->diaNode(); }

			DiaConnection& connection();
	const	DiaConnection& connection() const { return ax_const_cast(this)->connection(); }

	void	getString(axIString& outStr) const;

	AnonFieldValuePtr	getFieldValuePtr();

	Socket*	sock() { return _sock; }

	const AnonFieldPath&	fieldPath() { return _fieldPath; }
	const axFieldInfo*		fieldInfo() { return _fieldPath.fieldInfo; }

	void onFormat(axFormat& f) const;

private:
	AnonFieldPath	_fieldPath;
	Socket*			_sock = nullptr;
};


class DiaInSocket : public axNonCopyable {
public:
	using List = axDList<DiaInPlug, ax_no_delete>;

	~DiaInSocket() {
		disconnectAll();
	}

	void disconnectAll() {
		while (auto* p = _plugs.head()) {
			ax_delete(&p->connection());
		}
	}

	void disconnect(const AnonFieldPath& path);

	DiaConnectionNode&	connNode();
	DiaNode*			diaNode();

	List::Enumerator		plugs()			{ return _plugs.each(); }
	List::ConstEnumerator	plugs() const	{ return _plugs.each(); }

friend class DiaInPlug;
protected:
	List	_plugs;
};

class DiaOutSocket : public axNonCopyable {
public:
	using List = axDList<DiaOutPlug, ax_no_delete>;

	~DiaOutSocket() {
		disconnectAll();
	}

	void disconnectAll() {
		while (auto* p = _plugs.head()) {
			ax_delete(&p->connection());
		}
	}

	DiaConnectionNode&	connNode();
	DiaNode*			diaNode();

	List::Enumerator		plugs()			{ return _plugs.each(); }
	List::ConstEnumerator	plugs() const	{ return _plugs.each(); }

friend class DiaOutPlug;
protected:
	List	_plugs;
};

class DiaConnectionNode : public axNonCopyable {
public:
	DiaNode*		_diaNode = nullptr;
	DiaInSocket		inSock;
	DiaOutSocket	outSock;
};

class DiaConnection : public AnonObject {
public:
	bool transferValue(); 

	void onFormat(axFormat& f) const;

	DiaInPlug	inPlug;
	DiaOutPlug	outPlug;
};

inline DiaNode* DiaInPlug::diaNode()  { return _sock ? _sock->diaNode() : nullptr; }
inline DiaNode* DiaOutPlug::diaNode() { return _sock ? _sock->diaNode() : nullptr; }

inline DiaConnection& DiaInPlug::connection() { return *axPtrUtil::memberOwner(&DiaConnection::inPlug, this); }
inline DiaConnection& DiaOutPlug::connection() { return *axPtrUtil::memberOwner(&DiaConnection::outPlug, this); }

inline DiaConnectionNode& DiaInSocket::connNode() { return *axPtrUtil::memberOwner(&DiaConnectionNode::inSock, this); }
inline DiaConnectionNode& DiaOutSocket::connNode() { return *axPtrUtil::memberOwner(&DiaConnectionNode::outSock, this); }

inline DiaNode* DiaInSocket::diaNode() { return connNode()._diaNode; }
inline DiaNode* DiaOutSocket::diaNode() { return connNode()._diaNode; }

inline DiaOutPlug& DiaInPlug::outPlug() { return connection().outPlug; }
inline DiaInPlug&  DiaOutPlug::inPlug() { return connection().inPlug; }

inline const DiaOutPlug& DiaInPlug::outPlug() const { return connection().outPlug; }
inline const DiaInPlug&  DiaOutPlug::inPlug() const { return connection().inPlug; }


}