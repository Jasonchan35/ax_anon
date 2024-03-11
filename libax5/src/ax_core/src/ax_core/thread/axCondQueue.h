#pragma once

#include "axCondMutex.h"

template<class T, bool bBroadcast = false>
class axCondQueue : public axNonCopyable {
public:
	struct MData {
		axInt _maxSize = axNumLimit<axInt>::max();
		axSList<T> _list; // cannot use DList, because DListNode::removeFromList() may cause race condition
	};
	using ProtectedData = axCondMutexProtected<MData, bBroadcast>;
	using ScopedLock = typename ProtectedData::ScopedLock;

	void	clear() {
		auto md = _mdata.scopedLock();
		md->_list.clear();
	}

	void	insert	(T* p)	{
		auto md = _mdata.scopedLock();
		while (md->_list.size() >= md->_maxSize) {
			md.wait();
		}
		md->_list.insert(p); 
	}

	void	append	(T* p)	{
		auto md = _mdata.scopedLock();
		while (md->_list.size() >= md->_maxSize) {
			md.wait();
		}
		md->_list.append(p);
	}

	void	insert	(axUPtr<T> && p)	{ insert(p.ptr()); p.detach(); }
	void	append	(axUPtr<T> && p)	{ append(p.ptr()); p.detach(); }

	void	setMaxSize(axInt n) { _mdata.scopedLock()->_maxSize = n; }

	axUPtr<T>	popHead	()	{ return _mdata.scopedLock()->_list.popHead(); }
	axUPtr<T>	popTail	()	{ return _mdata.scopedLock()->_list.popTail(); }

	axUPtr<T>	waitHead() {
		auto md = _mdata.scopedLock();
		for(;;) {
			auto p = md->_list.popHead();
			if (p) return p;
			md.wait();
		}
	}

	axUPtr<T>	timedWaitHead(axInt milliseconds) {
		auto md = _mdata.scopedLock();
		for(;;) {
			auto p = md->_list.popHead();
			if (p) return p;
			if (!md.timedWait(milliseconds))
				return nullptr;
		}
	}

	axInt size() {
		auto md = _mdata.scopedLock();
		return md->_list.size();
	}

private:
	ProtectedData _mdata;
};