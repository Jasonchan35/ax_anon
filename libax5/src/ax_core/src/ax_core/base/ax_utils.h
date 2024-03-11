//
//  ax_utils.h
//  ax_core
//
//  Created by Jason on 2014-11-29.
//  Copyright (c) 2014 Awenix. All rights reserved.
//

#pragma once

#include "ax_macro.h"

template<class T, class... Args > AX_INLINE
void ax_call_constructor( T* p, Args&&... args ) noexcept {
	::new(p) T(ax_forward(args)...);
}

template<class T> AX_INLINE
void ax_call_destructor( T* p ) noexcept {
	p->~T();
}

inline void ax_force_crash() {
	std::cout << "ax_force_crash\n";
	*reinterpret_cast<int*>(1) = 0;
}

template<class T> void ax_free(T* p);
template<class T> void ax_delete(T* p);
template<class T> void ax_delete_set_null(T* &p);
template<class T> void ax_delete_array(T* p);

inline void _ax_assert_impl(const char* title, const char* expr, const char* funcName, const char* filename, int lineNumber, const char* msg) {
	const int bufLen = 8 * 1024;
	char buf[bufLen + 1];
	snprintf(buf, bufLen,
		"\n%s\n"
		"  Expr: %s\n"
		"  Func: %s\n"
		"Source: %s:%d\n"
		"%s\n", title, expr, funcName, filename, lineNumber, msg);
	buf[bufLen] = 0; //snprintf might not end with zero if exists bufLen limit
	ax_assert_platform_impl(buf, filename, lineNumber);
}

#define _AX_ASSERT_ONCE(...) do { static bool called = false; if (!called) { called = true; _ax_assert_impl(__VA_ARGS__); }  } while(false)

#define AX_ASSERT_FORMAT(expr, fmt, ...) \
	do{ \
		if (!(expr)) { \
			axString_<8 * 1024> msg; \
			msg.format(fmt, ##__VA_ARGS__); \
			_AX_ASSERT_ONCE("---- ASSERTION -------", #expr, AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__, msg.c_str()); \
		} \
	}while(false) \
//----------

//for some basic class cannot include axString otherwise might consider throw exception
#define AX_ASSERT_MSG(expr, msg)	do{ if (!(expr)) { _AX_ASSERT_ONCE("---- ASSERTION -------", #expr, AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__, msg); } }while(false)
#define AX_ASSERT(    expr     )	do{ if (!(expr)) { _AX_ASSERT_ONCE("---- ASSERTION -------", #expr, AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__, "" ); } }while(false)
#define AX_FATAL_ASSERT(expr)		do{ if (!(expr)) { _AX_ASSERT_ONCE("---- FATAL ASSERT ----", #expr, AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__, ""); ax_force_crash() } }while(false)
#define AX_ASSERT_NOT_IMPLEMENTED()	do{                _AX_ASSERT_ONCE("---- ASSERTION -------", "",    AX_FUNC_FULLNAME_SZ, __FILE__, __LINE__, "AX_ASSERT_NOT_IMPLEMENTED"); } while(false)

inline bool _ax_validate_func(bool expr, const char* expr_sz, const char* filename, int lineNumber, const char* msg) {
	if (expr) return true;
	_AX_ASSERT_ONCE("---- VALIDATE -------", expr_sz, AX_FUNC_FULLNAME_SZ, filename, lineNumber, msg);
	return false;
}
#define ax_validate(expr) _ax_validate_func(expr, #expr, __FILE__, __LINE__, "")

template<class T, axInt N> constexpr axInt ax_c_array_size(const T (&p)[N]) { return N; }

template<class T> AX_INLINE void ax_swap		( T& a, T& b )	{ T tmp(ax_move(a)); a = ax_move(b); b = ax_move(tmp); }

//---------------------
template<class T> using axDeleteFunc = void (*)(T* p);
template<class T> void ax_delete(T* p);
template<class T> AX_INLINE void ax_no_delete(T* /* p */) {}

//---------------------
template<class T> AX_INLINE T* ax_const_cast( const T* v ) { return const_cast<T*>(v); }
template<class T> AX_INLINE T& ax_const_cast( const T& v ) { return const_cast<T&>(v); }

template<class DST, class SRC> AX_INLINE void ax_static_cast     (DST& dst, SRC& src) { dst = static_cast<DST>(src); }
template<class DST, class SRC> AX_INLINE void ax_reinterpret_cast(DST& dst, SRC& src) { dst = reinterpret_cast<DST>(src); }

template<class ITER>
struct axForEachIter {
	axForEachIter(const axSourceLoc& loc, ITER p, ITER end) : _loc(loc), _p(p), _end(end) {}
	~axForEachIter() {
		if (_p != _end) {
			_ax_assert_impl("axForEachIter"
							, "_p == _end"
							, _loc.func, _loc.file, _loc.line
							, "not iterate till the end");
		}
	}

	AX_INLINE auto&&	operator*	() & 		{ return  *_p; }
	AX_INLINE auto		operator->	() &		{ return  _p; }
	AX_INLINE void		operator++	()			{ ++_p; }
	AX_INLINE void		operator++	(int)		{ _p++; }
	AX_INLINE void		operator+=	(axInt n)	{ _p += n; }
	AX_INLINE bool		operator==	(const ITER & rhs)	{ return _p == rhs._p; }
	AX_INLINE bool		operator!=	(const ITER & rhs)	{ return _p != rhs._p; }

	AX_INLINE auto&&	operator[](axInt i) { return _p[i]; }

	template<class R>
	AX_INLINE void		operator=	(const R& r)	{ *_p = r; }

private:
	axSourceLoc _loc;
	ITER _p;
	ITER _end;
};

template<class SPAN>
auto ax_foreach_begin(SPAN span) { return axForEachIter<decltype(span.begin())>(AX_LOC, span.begin(), span.end()); }

#define AX_OFFSET_OF(T, member) \
	reinterpret_cast<intptr_t>(&(reinterpret_cast<T*>(nullptr)->member))

namespace axPtrUtil {

AX_INLINE axIntPtr  toInt (const void* p) { return reinterpret_cast<axIntPtr >(p); }
AX_INLINE axUIntPtr toUInt(const void* p) { return reinterpret_cast<axUIntPtr>(p); }

template<class T> AX_INLINE
axInt countInBytes(const T* start, const T* end) {
	return reinterpret_cast<char*>(end) - reinterpret_cast<char*>(start);
}

template<class T> AX_INLINE
T* addOffsetInByte(T* p, axInt numBytes) { 
	return reinterpret_cast<T*>(reinterpret_cast<char*>(p) + numBytes); 
}

template<class T> AX_INLINE 
const T* addOffsetInByte(const T* p, axInt numBytes) {
	return reinterpret_cast<const T*>(reinterpret_cast<const char*>(p) + numBytes);
}

template< class Obj, class Member > constexpr
intptr_t memberOffset(Member Obj::*ptrToMember) {
	auto* c = reinterpret_cast<Obj*>(0);
	auto* m = &(c->*ptrToMember);
	return reinterpret_cast<intptr_t>(m);
}

template< class Obj, class Member > AX_INLINE
Obj* _memberOwner( Member Obj::*ptrToMember, Member* member) {
	if (!member) return nullptr;
	auto o = reinterpret_cast<char*>(member) - memberOffset(ptrToMember);
	return reinterpret_cast<Obj*>(o);
}

template< class Obj, class Member > AX_INLINE
Obj* memberOwner( Member Obj::*ptrToMember, Member* member) {
	return _memberOwner(ptrToMember, member);
}

template< class Obj, class Member > AX_INLINE
const Obj* memberOwner( Member Obj::*ptrToMember, const Member* member) {
	return _memberOwner(ptrToMember, ax_const_cast(member));
}

template<class DST, class SRC>
union ForceCastUnion {
	DST dst;
	SRC src;
};

// this cast can over come function pointer to void*
template<class DST, class SRC> AX_INLINE
DST forceCast(SRC src) {
	AX_STATIC_ASSERT(sizeof(DST) == sizeof(SRC));
	ForceCastUnion<DST, SRC> u;
	u.src = src;
	return u.dst;
}

} //namespace axPtrUtil

class axDeferHelper {
public:
	using Func = axFunc<void()>;
	Func func;

	explicit axDeferHelper(Func func_) : func(func_) {}
	~axDeferHelper() { if (func) func(); }
};

#define ax_defer(FUNC) axDeferHelper AX_UNIQUE_NAME(ax_defer)(FUNC);
//---------------------------------

struct axEnterOnce : public axNonCopyable {
	struct Scoped : public axNonCopyable {
		Scoped(Scoped && r) noexcept {
			_p = r._p;
			r._p = nullptr;
		}

		Scoped(axByte& entered) {
			if (!entered) {
				_p = &entered;
				entered = 1;
			}
		}
		~Scoped() {
			if (_p) *_p = 0;
		}

		explicit operator bool() const { return _p != nullptr; }
		axByte* _p = nullptr;
	};

	AX_NODISCARD Scoped enter() { return Scoped(_entered); }
	axByte _entered{0};
};

//---------------------------------
template<class T, axInt SIZE_IN_BYTES = sizeof(T)>
class axPrivateObject {
public:
	axPrivateObject() = default;
	~axPrivateObject() { AX_ASSERT(_ptr == nullptr); /* please call deleteObject() beforehand*/ }

	template<class... Args>
	T* newObject(Args&&... args) {
		deleteObject();
		AX_STATIC_ASSERT(sizeof(T) <= SIZE_IN_BYTES);
		_ptr = new(_buffer) T(ax_forward(args)...);
		return _ptr;
	}

	void deleteObject() {
		if (_ptr) {
			_ptr->~T();
			_ptr = nullptr;
		}
	}

	AX_INLINE 	    T* ptr()				{ return  _ptr; }
	AX_INLINE const T* ptr() const			{ return  _ptr; }

	AX_INLINE 		T* operator->()			{ return  _ptr; }
	AX_INLINE const T* operator->() const	{ return  _ptr; }

	AX_INLINE 		T& operator*()			{ return *_ptr; }
	AX_INLINE const T& operator*() const	{ return *_ptr; }

	explicit operator bool() const { return _ptr != nullptr; }

private:
	T*	 _ptr = nullptr;
	char _buffer[SIZE_IN_BYTES];
};

template<class T>
class axBlackBox {
public:
private:
	T	_value;
};

struct axDummyInstance {
  template <typename Type> operator Type() const;
};

template<axInt... ints>
using axIntSequence = std::integer_sequence<axInt, ints...>;

template<axInt N>
using axIntSequence_make = std::make_integer_sequence<axInt, N>;

template<class T>
class axComPtr : public axNonCopyable {
public:
	axComPtr() = default;
	axComPtr(const axComPtr& r) { ref(r.ptr()); }
	axComPtr( axComPtr && r ) noexcept {
		_p = r._p;
		r._p = nullptr;
	}

	~axComPtr() noexcept { unref(); }

	T* operator->() noexcept { return _p; }
	  operator T*() noexcept { return _p; }

	void ref(T* p) { 
		if (p == _p) return; 
		unref(); 
		_p = p; 
		if (_p) _p->AddRef();
	}
	void unref() noexcept {
		if (_p) {
			_p->Release();
			_p = nullptr;
		}
	}

	T** ptrForInit() noexcept { unref(); return &_p; }

		  T* ptr() noexcept			{ return _p; }
	const T* ptr() const noexcept	{ return _p; }

	T* detach() { T* o = _p; _p = nullptr; return o; }
private:
	T*	_p = nullptr;
};

//---- SturctFieldCount ----

template< class... >
using ax_void_t = void; // std::void_t require c++17

template<typename Obj, typename IntSequence = axIntSequence<>, typename = void >
struct ax_struct_field_count_impl : IntSequence {};

template<typename Obj, axInt... Indices>
struct ax_struct_field_count_impl
	<
		Obj,
		axIntSequence<Indices...>, 
		ax_void_t<
			decltype(
				Obj{
					(static_cast<void>(Indices), std::declval<axDummyInstance>())..., std::declval<axDummyInstance>()
				}
			)
		>
	>
	: ax_struct_field_count_impl< Obj, axIntSequence<Indices..., sizeof...(Indices)> > 
{
};

template<typename T> constexpr
axInt ax_struct_field_count() {
	AX_STATIC_ASSERT(axTypeTraits::isStandardLayout<T>::value);
	AX_STATIC_ASSERT(!axTypeTraits::isPolymorphic<T>::value);
	return ax_struct_field_count_impl<T>().size();
}
//-----------------------

inline void ax_sleep(axInt milliseconds) { ::std::this_thread::sleep_for(std::chrono::duration<axInt, std::milli>(milliseconds)); }

// Visual Studio - Natvis
struct axDebugNatvisHex {
	struct Low  { unsigned char c; };
	struct High { unsigned char c; };

	struct low  { unsigned char c; };
	struct high { unsigned char c; };
};

//---------------------
