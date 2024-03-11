#pragma once

#if AX_OBJC

/* No needed when enable Automatic Reference Count
 
class	axScopedNSAutoreleasePool : public axNonCopyable {
public:
	axScopedNSAutoreleasePool () { _p = [[NSAutoreleasePool alloc] init]; }
	~axScopedNSAutoreleasePool() { [ _p release ]; }
private:
	NSAutoreleasePool* _p;
};

template<class T>
class	axNSObj {
public:
	axNSObj(T p = nil) : _p(nil)	{ reset(p); }
	~axNSObj()						{ reset(); }
	
	void	reset	()				{ if(_p) { [_p release]; _p = nil; } }
	void	reset	(T p)			{ reset(); _p = p; if(p) { [p retain]; } }
	
			T obj	()				{ return  _p; }
	const	T obj	() const		{ return  _p; }
	
	operator		T ()			{ return  _p; }
	operator const	T () const		{ return  _p; }
	
private:
	T	_p;
};

template<class T> axNSObj<T> axNSObj_make(T p) { return axNSObj<T>(p); }

 */

template<class T>
class axNSObj : public axNonCopyable {
	using This = axNSObj;
public:
	AX_INLINE axNSObj(T* p = nil) : _p(nil)		{ ref(p); }
	AX_INLINE axNSObj(axNSObj && r) : _p(r._p)		{ r._p = nil; }
	
	AX_INLINE ~axNSObj()		{ unref(); }

	AX_INLINE explicit operator bool() const		{ return _p != nil; }

	AX_INLINE void	ref			(T* p);
	AX_INLINE void	unref		();

	AX_INLINE T*	detach		()					{ auto* p = _p; _p = nil; return p; }

	AX_INLINE T*	ptr			()					{ return _p; }
	
	AX_INLINE operator			T* ()				{ return  _p; }
	AX_INLINE operator const	T* () const			{ return  _p; }

	AX_INLINE void operator=(T* p) { ref(p); }

	AX_INLINE static This s_make(T* p)				{ return This(p); }
	
private:
	T* __unsafe_unretained _p;
};

template<class T> axNSObj<T> axNSObj_make(T* p) { return axNSObj<T>::s_make(p); }

template<class T> AX_INLINE
void axNSObj<T>::ref(T* p) {
   unref();
   if (p) {
	   _p = p;
	   CFRetain((CFTypeRef)_p);
   }
}

template<class T> AX_INLINE
void axNSObj<T>::unref() {
	if (_p) {
		CFRelease((CFTypeRef)_p);
		_p = nil;
	}
}

#endif //AX_OBJC
