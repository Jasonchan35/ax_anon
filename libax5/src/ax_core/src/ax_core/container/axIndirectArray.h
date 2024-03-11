#pragma once
#include "axArray.h"

// better replacement of axArray< axUPtr<T> >
template< class T, axInt N = 2 >
class axIndirectArray : public axNonCopyable {
public:
	~axIndirectArray() { clear(); }
	void clear() { resize(0); }

	T* operator[](axInt i) {
		return _arr[i];
	}

	void resize(axInt reqSize) {
		if (reqSize < 0) reqSize = 0;

		axInt n = _arr.size();
		for (axInt i = reqSize; i < n; i++) {
			ax_delete(_arr[i]);
		}
		_arr.resize(reqSize);
	}

	axInt size() const { return _arr.size(); }

	T* append		(axUPtr<T> && r)	{ _arr.append(r.ptr()); return r.detach(); }

	template<class... ARGS>
	T* emplaceBack	(ARGS&&... args)	{
		return _arr.emplaceBack(ax_new T(ax_forward(args)...));
	}

	axInt findAndRemove(T* p) {  return _arr.findAndRemove(p); }
	axInt findAndDelete(T* p) { 
		auto index = _arr.findAndRemove(p);
		if (index >= 0) {
			ax_delete(p);
		}
		return index;
	}

	using CView = axSpan< axPtr<T> >;
	using View	= axMutSpan< axPtr<T> >;

	CView	constView() const	{ return _arr.constView(); }
	View	view()				{ return _arr.view(); }

	operator CView() const	{ return constView(); }
	operator View()			{ return view(); }

	using Enumerator = axArrayEnumerator<T*>;
	using Iterator = typename Enumerator::Iterator;

	Iterator begin() { return _arr.begin(); }
	Iterator end()   { return _arr.end(); }


private:
	axArray<T*, N>	_arr;
};


