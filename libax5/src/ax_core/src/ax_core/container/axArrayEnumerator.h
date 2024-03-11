#pragma once

#include "axArrayUtil.h"
#include "../pointer/axPtr.h"
#include "axLocalBuffer.h"
#include "../math/axRange.h"
#include "axPair.h"

//--------------------
class axArrayError						: public axError {};
class axArrayError_OutOfRange 			: public axArrayError {};
//--------------------

#define axArray_kDefaultSize(T) (ax_sizeof(void*) * 2 / ax_sizeof(T))

template< class T >
class axArrayEnumerator {
public:
	class	Iterator {
	public:
		Iterator( T* p=nullptr ) : _p(p) {}
		operator const T*	()			{ return  _p; }
		T &		operator*	()			{ return *_p; }
		T*		operator->	()			{ return  _p; }
		void	operator++	()			{ ++_p; }
		void	operator++	(int)		{ _p++; }
		void	operator+=	(axInt n)	{ _p += n; }
		bool	operator==	(const Iterator & rhs)	{ return _p == rhs._p; }
		bool	operator!=	(const Iterator & rhs)	{ return _p != rhs._p; }
	private:
		T*	_p;
	};	

	Iterator	begin	()	const	{ return Iterator(_begin); }
	Iterator	end		()	const	{ return Iterator(_end);   }

	AX_INLINE Iterator	at		(axInt i)	{
		auto* p = _begin + i;
		#ifdef AX_GEN_CONFIG_Debug
			if (p < _begin || p+1 > _end) {
				throw axArrayError_OutOfRange();
			}
		#endif
		return Iterator(p);
	}

	explicit axArrayEnumerator(T* begin, T* end) : _begin( begin ), _end( end ) {}

private:
	T*		_begin;
	T*		_end;
};


template< class T >
class axArrayInterleaveEnumerator {
	using Enumerator = axArrayInterleaveEnumerator;
public:

	operator axArrayInterleaveEnumerator<const T>() { return axArrayInterleaveEnumerator<const T>(_begin, _end, _stride); }

	class	Iterator {
	public:
		Iterator( T* p, axInt stride ) : _p(p), _stride(stride) {}
		operator const T*	()			{ return  _p; }
		T&		operator*	()			{ return *_p; }
		T*		operator->	()			{ return  _p; }
		void	operator++	()			{ _p = axPtrUtil::addOffsetInByte(_p, _stride); }
		void	operator++	(int)		{ _p = axPtrUtil::addOffsetInByte(_p, _stride); }
		void	operator+=	(axInt n)	{ _p = axPtrUtil::addOffsetInByte(_p, _stride * n); }
		bool	operator==	(const Iterator & rhs)	{ return _p == rhs._p; }
		bool	operator!=	(const Iterator & rhs)	{ return _p != rhs._p; }
	protected:
		T*		_p;
		axInt	_stride; // in bytes
	};

	Enumerator slice(axIntRange range) {
		auto* s = axPtrUtil::addOffsetInByte(_begin, range.start * _stride);
		auto* e = axPtrUtil::addOffsetInByte(_begin, range.end() * _stride);
		if (s < _begin || e < _begin || s > _end || e > _end) {
			throw axArrayError_OutOfRange();
		}
		return Enumerator(s, e, _stride);
	}

	AX_INLINE Iterator	begin	()	const	{ return Iterator(_begin, _stride); }
	AX_INLINE Iterator	end		()	const	{ return Iterator(_end,   _stride); }
	AX_INLINE Iterator	operator[](axInt i)	{ return at(i); }

	AX_INLINE	axInt	size() const {
		return _stride ? axPtrUtil::countInBytes(_begin, _end) / _stride : 0;
	}

	AX_INLINE Iterator	at		(axInt i)	{
		auto* p = axPtrUtil::addOffsetInByte(_begin, i * _stride);
		#ifdef AX_GEN_CONFIG_Debug
			if (p < _begin || p+1 > _end) {
				throw axArrayError_OutOfRange();
			}
		#endif
		return Iterator(p, _stride);
	}

	void fillValues(const T& v) {
		for (auto& dst : *this) {
			dst = v;
		}
	}

	void fillRotateValues(axSpan<T> values) {
		if (values.size() == 0) return;

		axInt i = 0;
		for (auto& dst : *this) {
			dst = values.at(i);
			i = (i + 1) % values.size();
		}
	}

	explicit axArrayInterleaveEnumerator(T* begin, T* end, axInt stride) : _begin( begin ), _end( end ), _stride(stride) {}

private:
	T*		_begin;
	T*		_end;
	axInt	_stride; // in bytes
};


//! Reverse Enumerator
template< class T >
class	axArrayRevEnumerator {
public:
	class	Iterator {
	public:
		Iterator( T* p=nullptr ) : _p(p) {}
		operator const T*	()  { return  _p; }
		T &		operator*	()	{ return *_p; }
		void	operator++	()	{ --_p; }
		bool	operator==	( const Iterator & rhs )	{ return _p == rhs._p; }
		bool	operator!=	( const Iterator & rhs )	{ return _p != rhs._p; }
	private:
		T*	_p;
	};

	Iterator	begin	()	const	{ return Iterator(_begin); }
	Iterator	end		()	const	{ return Iterator(_end);   }

	explicit axArrayRevEnumerator(T* begin, T* end) {
		if( begin && end ) {
			_begin = end   - 1;
			_end   = begin - 1;
		}else{
			_begin = nullptr;
			_end   = nullptr;
		}
	}

private:
	T*		_begin;
	T*		_end;
};
