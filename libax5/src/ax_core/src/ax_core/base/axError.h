#pragma once

class axError : public ax_exception {
protected:
	// not allowed to use it directly, so make constructor protected
	axError() { _ctor(); }
	explicit axError(const axSourceLoc& loc_) : loc(loc_) { _ctor(); }

public:
	static void s_setEnableAssertion(bool b);

	void onFormat(axFormat& f) const;
	axSourceLoc loc;

private:
	static bool s_enableAssertion;
	void _ctor();
};

class axError_NullPointer			: public axError {};
class axError_Undefined				: public axError {
public:
	explicit axError_Undefined(const axSourceLoc& loc) : axError(loc) {}
};

class axError_StdException			: public axError {
public:
	axError_StdException(const axSourceLoc& loc, const ax_exception& e) 
	: stdEx(e){
	}
	ax_exception stdEx;
};

class axError_NotEnoughMemory 		: public axError {};
class axError_MallocInvalidSize		: public axError {};
class axError_GetObjectAllocator	: public axError {};

class axError_BufferOverlapped		: public axError {};
class axError_SliceOutOfRange 		: public axError {};
class axError_Enumerating 			: public axError {};
class axError_SafeAssign			: public axError {};
class axError_Duplicated			: public axError {};
