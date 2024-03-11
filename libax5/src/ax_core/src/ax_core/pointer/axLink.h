#pragma once

#include "../container/axDList.h"
#include "../rtti/axType.h"

class axLinkable;
class axLinkableDummy;

//! Intrusive Ref by LinkedList, unsafe in Multi-thread !!!, but fast and simple in Single Thread
template<class T>
class axLink : public axPtrBase<T>, public axDListNode< axLink<T> > {
	using This = axLink;
	using Base = axPtrBase<T>;
	using Base::_p;
public:
	explicit axLink(T* p = nullptr)		{ ref(p); }
	explicit axLink(axLink &  r)		{ ref(r.ptr()); }
	explicit axLink(axLink && r)		{ if (_p == r._p) return; ref(r.ptr()); r.unref(); }
	~axLink() { unref(); }

	AX_INLINE operator       T*() && = delete;
	AX_INLINE operator       T*() &			{ return  _p; }
	AX_INLINE operator const T*() const &		{ return  _p; }
//	AX_INLINE operator axLink<const T>() const	{ return axLink<const T>(_p); }

	AX_INLINE T*		ref(T* p) &;
	AX_INLINE void		unref();

	AX_INLINE void		operator=(T* p)				{ ref(p); }
	AX_INLINE void		operator=(axLink & r)		{ ref(r.ptr()); }
	AX_INLINE void		operator=(axLink && r)		{ ref(r.ptr()); r.unref(); }

	void setOwner(axTypedBase* owner) { _owner = owner; }
	axTypedBase*	owner() { return _owner; }

	template<class OWNER>
	OWNER*	owner() {
		if (!_owner) return nullptr;
		return ax_type_cast<OWNER>(_owner);
	}

protected:
	axTypedBase*	_owner = nullptr;
	friend class axLinkable;
	using Dummy = axLinkableDummy;
	axLink<Dummy>* _asDummy() { return reinterpret_cast< axLink<Dummy>* >(this); }
};

class axLinkable {
	using Dummy = axLinkableDummy;
public:
	using LinkableList = axDList< axLink<Dummy> >;

	~axLinkable() { Linkable_removeAll(); }
	void Linkable_onAdd		(axLink<Dummy>* p) { _links.insert(p); }
	void Linkable_onRemove	(axLink<Dummy>* p) { _links.remove(p); }
	void Linkable_removeAll	();

	axInt Linkable_count() const { return _links.size(); }

	LinkableList::Enumerator	Linkable_each() { return _links.each(); }
	
private:
	LinkableList	_links;
};

template<class T> AX_INLINE bool operator==(const T* a,    const axLink<T>& b) { return b == a; }
template<class T> AX_INLINE bool operator==(const axNull&, const axLink<T>& b) { return b == nullptr; }

template<class T> AX_INLINE
axLink<T> axLink_make(T* p) { return axLink<T>(p); }

class axLinkableDummy : public axLinkable {
};

template<class T, axInt N = 0>
class axLinkArray : public axArray<axLink<T>, N> {
	using Base = axArray<axLink<T>, N>;
public:
	void toPtrArray(axIArray<T*>& outBuf) {
		outBuf.resize(Base::size());

		auto* dst = outBuf.data();
		for (auto& e : *this) {
			*dst = e.ptr();			
		}
	}
};

template<class T> inline void ax_delete(axLink<T>& p) { ax_delete(p.ptr()); p.unref(); }

//----------------------------
template<class T>
inline T* axLink<T>::ref(T * p) & {
	if (_p == p) return p;
	unref();
	_p = p;
	if (p)
		p->Linkable_onAdd(_asDummy());
	return p;
}

template<class T>
inline void axLink<T>::unref() {
	if (_p) {
		_p->Linkable_onRemove(_asDummy()); 
		_p = nullptr;
	}
}

inline void axLinkable::Linkable_removeAll() {
	while (auto p = _links.popHeadPtr()) {
		p->_p = nullptr;
	}
}
