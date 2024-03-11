#pragma once

#include "../rtti/axType_macro.h"
#include "../container/axSpan.h"
#include "ax_c_str.h"

//template<class T, class V> axStrViewT<T> ax_to_string(const V& v);
//template<class T, class V> bool axStrView_tryParse(axStrViewT<T> s, V & outValue);

template<class T>
class axStrMutViewT { //Copyable
	AX_RTTI_INTERNAL_STRUCT(axStrMutViewT<T>)
public:

	using MutT = typename axTypeTraits::removeConst<T>::Type;
	using MutView = axStrMutViewT<T>;
	using View    = axStrMutViewT<const T>;
	using Byte    = typename axTypeTraits::selectConst<axByte, T>::Type;

	static  MutView s_empty() { return MutView(nullptr, 0); }

	axStrMutViewT() = default;
	AX_INLINE constexpr explicit axStrMutViewT(T& ch) : _data(&ch), _size(1) {}
	AX_INLINE constexpr explicit axStrMutViewT(T* data, axInt size) : _data(data), _size(size) {}

	template<axInt N>
	AX_INLINE axStrMutViewT(T (&sz)[N]) : _data(reinterpret_cast<T*>(sz)), _size(N ? N-1 : 0) {}

	AX_INLINE View	 constView() const { return View(_data, _size); }
	AX_INLINE operator View() const { return constView(); }

	AX_INLINE	explicit operator bool() const				{ return _size > 0; }

	AX_INLINE			axInt	size() const				{ return _size; }
	AX_INLINE			axInt	sizeInBytes() const			{ return _size * ax_sizeof(T); }

	AX_INLINE	axMutSpan<Byte>	byteSpan()					{ return axMutSpan<Byte>(reinterpret_cast<      Byte*>(_data), sizeInBytes()); }
	AX_INLINE	   axSpan<Byte>	byteSpan() const			{ return    axSpan<Byte>(reinterpret_cast<const Byte*>(_data), sizeInBytes()); }

	AX_INLINE  axMutSpan<T>	span()						{ return axMutSpan<T>(_data, _size); }

	AX_INLINE			bool	inBound		(axInt  i) const	{ return i >= 0 && i < _size; }
	AX_INLINE			bool	isOverlapped(const This& rhs) const	{ return axArrayUtil::isOverlapped(_data, _size, rhs.data(), rhs.size()); }

	AX_INLINE			T &		operator[]	(axInt i)			{ return at(i); }
	AX_INLINE	const	T &		operator[]	(axInt i) const		{ return at(i); }

	AX_INLINE			T &		at			(axInt i)			{ _checkBound(i); return _data[i]; }
	AX_INLINE	const	T &		at			(axInt i) const		{ _checkBound(i); return _data[i]; }

	AX_INLINE 			T &		back		(axInt i = 0)		{ return at( _size-i-1 ); }
	AX_INLINE 	const	T &		back		(axInt i = 0) const	{ return at( _size-i-1 ); }

	AX_INLINE 			T &		unsafe_at	(axInt i)			{ _debug_checkBound(i); return _data[i]; }
	AX_INLINE 	const	T &		unsafe_at	(axInt i) const		{ _debug_checkBound(i); return _data[i]; }

	AX_INLINE 			T &		unsafe_back	(axInt i)			{ return unsafe_at( _size-i-1 ); }
	AX_INLINE 	const	T &		unsafe_back	(axInt i) const		{ return unsafe_at( _size-i-1 ); }

	AX_INLINE			T*		data()							{ return _data; }
	AX_INLINE	const	T*		data() const					{ return _data; }

//! cannot assume StrView ends with null, the size can be shorter than null-terminator
//	AX_INLINE	const   T*			c_str		() const		{ return _data ? _data : ax_empty_c_str<T>(); }

	//----------------------------------
	AX_INLINE			MutView	slice		(axInt offset, axInt size);
	AX_INLINE			MutView	sliceFrom	(axInt offset)						{ return slice(offset, _size - offset); }
	AX_INLINE			MutView	sliceBack	(axInt size)						{ return slice(_size - size, size); }
	AX_INLINE			MutView	sliceLess	(axInt n)							{ return slice(0, _size - n); }
	//----------------------------------
										axInt	find				(View str,  bool ignoreCase = false) const;
										axInt	findBack			(View str,  bool ignoreCase = false) const;

										axInt	findChar			(T     ch,  bool ignoreCase = false) const;
										axInt	findCharBack		(T     ch,  bool ignoreCase = false) const;

										axInt	findChars			(View list, bool ignoreCase = false) const;
										axInt	findCharsBack		(View list, bool ignoreCase = false) const;

					axPair<MutView, MutView>	splitByIndex		(axInt index);

					axPair<MutView, MutView>	split				(View str,  bool ignoreCase = false);
					axPair<MutView, MutView>	splitBack			(View str,  bool ignoreCase = false);

					axPair<MutView, MutView>	splitByChar			(T     ch,  bool ignoreCase = false);
					axPair<MutView, MutView>	splitByCharBack		(T     ch,  bool ignoreCase = false);

					axPair<MutView, MutView>	splitByChars		(View list, bool ignoreCase = false);
					axPair<MutView, MutView>	splitByCharsBack	(View list, bool ignoreCase = false);

										void	getTokens			(axIArray<MutView> &outList, T separator, bool ignoreCase = false);

	//----------------------------------
							MutView	trim					(T ch);
							MutView	trimBack				(T ch);
							MutView	trimFrontAndBack		(T ch) 	{ return trim(ch).trimBack(ch); }

							MutView	trimChars				(View charList);
							MutView	trimCharsBack			(View charList);
							MutView	trimCharsFrontAndBack	(View charList) {
								return trimChars(charList).trimCharsBack(charList);
							}
	
							MutView	extractFromPrefix(View prefix, bool ignoreCase = false) { return startsWith(prefix, ignoreCase) ? sliceFrom(prefix.size()) : s_empty(); }
							   View	extractFromPrefix(View prefix, bool ignoreCase = false) const { return ax_const_cast(this)->extractFromPrefix(prefix, ignoreCase); }
	//----------------------------------
	AX_INLINE			bool	operator==	(View r) const	{ return equals(r); }
	AX_INLINE			bool	operator!=	(View r) const	{ return !equals(r); }
	AX_INLINE			bool	operator<	(View r) const	{ return compare(r) <  0; }
	AX_INLINE			bool	operator<=	(View r) const	{ return compare(r) <= 0; }
	AX_INLINE			bool	operator>	(View r) const	{ return compare(r) >  0; }
	AX_INLINE			bool	operator>=	(View r) const	{ return compare(r) >= 0; }

	AX_INLINE			bool	equals		(View r, bool ignoreCase = false) const;
                            axInt	compare		(View r, bool ignoreCase = false) const;

	AX_INLINE			bool	startsWith	(View r, bool ignoreCase = false) const;
	AX_INLINE			bool	endsWith	(View r, bool ignoreCase = false) const;

                            bool    matchWildcard(View wildcard, bool ignoreCase) const;

	//------ parse primitive value ----------------------------
							bool	tryParse	(axInt8     & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axInt16    & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axInt32    & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axInt64    & outValue) const { return _tryParsePrimitive(outValue); }

							bool	tryParse	(axUInt8    & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axUInt16   & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axUInt32   & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axUInt64   & outValue) const { return _tryParsePrimitive(outValue); }

							bool	tryParse	(axFloat32  & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axFloat64  & outValue) const { return _tryParsePrimitive(outValue); }
							bool	tryParse	(axFloat128 & outValue) const { return _tryParsePrimitive(outValue); }

							bool	tryParse	(bool & outValue) const { return _tryParsePrimitive(outValue); }

							template<class V>
							bool	tryParse	(V & outValue) const { return axStrView_tryParse(*this, outValue); }
	//----------------------------------

    template<class R> using Enumerator_    = axArrayEnumerator<R>;
    template<class R> using RevEnumerator_ = axArrayRevEnumerator<R>;

	using Enumerator  = Enumerator_<T>;
	using CEnumerator = Enumerator_<const T>;
	using RevEnumerator  = RevEnumerator_<T>;
	using CRevEnumerator = RevEnumerator_<const T>;
	using Iterator			= T*;
	using CIterator			= const T*;

    Enumerator		each	()			{ return Enumerator		( _data, _data + _size ); }
    CEnumerator		each	() const	{ return CEnumerator	( _data, _data + _size ); }
    
    RevEnumerator	revEach	()			{ return RevEnumerator	( _data, _data + _size ); }
    CRevEnumerator	revEach	() const	{ return CRevEnumerator	( _data, _data + _size ); }
    
	Iterator		begin	()			{ return _data; }
    CIterator		begin	() const	{ return _data; }
	Iterator		end		()			{ return _data + _size; }
    CIterator		end		() const	{ return _data + _size; }
    //---------------
    
	axHash onGetHash() const;

	void onFormat(axFormat& f) const;

private:
	AX_INLINE void	_checkBound			( axInt i ) const { if(!inBound(i)) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		AX_UNUSED(i);
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

	template<class R> 
	bool _tryParsePrimitive(R& outValue) const;

	T*		_data = nullptr; 
	axInt	_size = 0;
};

//--------------------------
template<class T> AX_INLINE 
axStrMutViewT<T> axStrView_c_str(T* p)					{ return axStrMutViewT<T>(p, axCStr::length(p)); }

template<class T> AX_INLINE 
axStrMutViewT<T> axStrView_char(T& p)					{ return axStrMutViewT<T>(&p, 1); }

template<class T> AX_INLINE 
axStrMutViewT<T> axStrMutSpan_make(T* p, axInt size)	{ return axStrMutViewT<T>(p, size); }

template<class T> AX_INLINE 
axStrViewT<T> axStrView_c_str(const T* p)				{ return axStrViewT<T>(p, axCStr::length(p)); }

template<class T> AX_INLINE 
axStrViewT<T> axStrView_make(T* p, axInt size)			{ return axStrViewT<T>(p, size); }

template<class T, axInt N> AX_INLINE 
axStrViewT<T> axStrView_make(const T (&p)[N])			{ return axStrViewT<T>(p); }

template<class T> AX_INLINE 
void axStrViewArray_make(axIArray< axStrViewT<T> >& outArr, axSpan<const T*> inSpan){
	outArr.resize(inSpan.size());
	for (axInt i = 0; i < inSpan.size(); i++) {
		outArr[i] = axStrView_c_str(inSpan[i]);
	}
}

AX_INLINE
axStrView8 axStrView_make(const std::string& str) { return axStrView8(str.c_str(), axInt_from_size_t(str.size())); }

AX_INLINE
axStrView8 axStrView_make(axByteSpan view)	{ return axStrView8(reinterpret_cast<const char*>(view.data()), view.size()); }

AX_INLINE
axStrView8 axStrView8_make(axByteSpan v) {
	AX_STATIC_ASSERT(sizeof(axChar) == sizeof(axByte));
	return axStrView8(reinterpret_cast<const axChar8*>(v.data()), v.size()); 
}

#if AX_OS_MACOSX || AX_OS_IOS

AX_INLINE
axStrView8 axStrView_make(NSString* s) {
	return axStrView_c_str([s UTF8String]);
}

#endif

//--------------------------

template<class T> inline
std::ostream& operator << ( std::ostream & s, const axStrMutViewT<T> & v ) { return s.write(v.data(), v.size()); }

inline
void ax_console_write(axStrView s) {
	std::cout.write(s.data(), s.size());
}

template<class T, axInt N> inline
axHash axHash_get(const T (&p)[N]) { return axStrView_make(p).onGetHash(); }


template<class T> AX_INLINE
bool axStrMutViewT<T>::equals(View r, bool ignoreCase) const {
	if (_size != r.size()) return false;
	if (_data == r.data()) return true;
	if (ignoreCase) {
		return axCStr::caseEquals(_data, r.data(), _size);
	}else{
		return axCStr::equals(_data, r.data(), _size);
	}
}

template<class T> AX_INLINE
bool axStrMutViewT<T>::startsWith(View r, bool ignoreCase) const {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->slice(0, r.size()).equals(r, ignoreCase);
}

template<class T> AX_INLINE
bool axStrMutViewT<T>::endsWith(View r, bool ignoreCase) const {
	if (_size < r.size()) return false;
	return ax_const_cast(this)->sliceBack(r.size()).equals(r, ignoreCase);
}

template<class T> AX_INLINE
axStrMutViewT<T> axStrMutViewT<T>::slice(axInt offset, axInt size) {
	if (offset < 0 || size < 0 || offset + size > _size ) {
		throw axError_SliceOutOfRange(); 
	}
	return MutView(_data + offset, size); 
}

template<class T>
template<class R> AX_INLINE
bool axStrMutViewT<T>::_tryParsePrimitive(R& outValue) const {
	//make sure has zero terminated, because StrView may not have '\0' according to the size()
	axStringT<MutT, 128> tmp;
	tmp = *this;
	return axCStr::parse(tmp.c_str(), outValue);
}
