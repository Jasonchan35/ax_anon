#pragma once
#include "axStringError.h"
#include "../base/axHash.h"

template<class T> void axFormatParam_handler(axFormat& f, const T& value);

class axFormatParam {
public:
	axFormatParam() = default;

	// String Literal
#define AX_MACRO_OP(T) \
	template<axInt N> \
	axFormatParam(const T (&sz)[N]) \
		: _data(sz) \
		, _dataLen(N ? N-1 : 0) \
		, _handler(&axFormatParam::wrapper_StrView<T>) \
	{}
//----
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP

	template<class T>
	axFormatParam(const T& o) 
		: _data(&o)
		, _handler(&axFormatParam::wrapper<T>)
	{}

	//---------------------
	void onProc(axFormat& f) const {
		if (_handler) {
			(this->*_handler)(f);
		}
	}

	const void* data		() const { return _data; }
private:

	using Handler = void (axFormatParam::*)( axFormat &f ) const;

	template<class T> AX_INLINE
	void wrapper(axFormat& f) const { 
		axFormatParam_handler(f, *reinterpret_cast<const T*>(_data));
	}

	template<class T> AX_INLINE
	void wrapper_StrView(axFormat& f) const {
		axFormatParam_handler(f, axStrViewT<T>(reinterpret_cast<const T*>(_data), _dataLen));
	}

	const void* _data = nullptr;
	axInt		_dataLen = 0;
	Handler		_handler = nullptr;
};

#if 0
#pragma mark ----------- Obj-C ------------
#endif

#if AX_OS_MACOSX || AX_OS_IOS

template<>
void axFormatParam_handler<NSError>(axFormat& f, NSError const & error);

template<>
void axFormatParam_handler<NSError*>(axFormat& f, NSError* __strong const & error);

template<>
void axFormatParam_handler<NSString*>(axFormat& f, NSString* __strong const & v);
#endif


#if 0
#pragma mark ----------- char ------------
#endif

#define AX_MACRO_OP(T) \
	template<> void axFormatParam_handler<T>(axFormat& f, const T& value); \
	\
	template<axInt N> inline \
	void axFormatParam_handler(axFormat& f, const T (&sz)[N]) { \
		axFormatParam_handler(f, axStrViewT<T>(sz)); \
	} \
//----------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP

#if 0
#pragma mark ----------- int / uint ------------
#endif

#define AX_MACRO_OP(T) \
	template<> void axFormatParam_handler<T>(axFormat& f, const T& value); \
//--------
	AX_TYPE_LIST_ALL_PRIMITIVES(AX_MACRO_OP);
	AX_TYPE_LIST_EXTRA_NATIVE_SINT(AX_MACRO_OP);
	AX_TYPE_LIST_EXTRA_NATIVE_UINT(AX_MACRO_OP);
#undef AX_MACRO_OP

#if 0
#pragma mark ----------- void * ------------
#endif

void axFormatParam_handler_void_ptr(axFormat& f, const void* value);
AX_INLINE void axFormatParam_handler(axFormat& f, void const* value) { return axFormatParam_handler_void_ptr(f, value); }
AX_INLINE void axFormatParam_handler(axFormat& f, void *const value) { return axFormatParam_handler_void_ptr(f, value); }

template<class T> inline
void axFormatParam_handler_pointer(axFormat& f, const T* value) {

#define AX_MACRO_OP(R) \
	static_assert(!axTypeTraits::isSame<T, R>::value, "char* / wchar_t* is not allowed, please use axStrView"); \
//------
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP

	if (!value) {
		axFormatParam_handler(f, axStrView("null"));
	} else {
		axFormatParam_handler(f, *value);
	}
}

template<class T> inline
void axFormatParam_handler(axFormat& f, const T* value) { axFormatParam_handler_pointer(f, value); }

template<class T> inline
void axFormatParam_handler(axFormat& f, T* value) { axFormatParam_handler_pointer(f, value); }

template<> void axFormatParam_handler(axFormat& f, const axHash& v);
template<> void axFormatParam_handler(axFormat& f, const ax_exception& e);

template<class T> inline
void axFormatParam_handler(axFormat& f, const T& value) {
	value.onFormat(f);
}

