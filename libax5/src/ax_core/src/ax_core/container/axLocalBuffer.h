//
//  LocalBuffer.h
//  ax_core
//
//  Created by Jason Chan on Mar 16, 2015.
//  Copyright (c) 2015 Awenix. All rights reserved.
//

#pragma once

template< class T, axInt LOCAL_BUF_SIZE >
class axLocalBuffer {
public:
	static const	axInt	kLocalBufSize = LOCAL_BUF_SIZE;

	AX_INLINE			T*		localBufPtr	() 		 { return reinterpret_cast<	     T*>(_localBuf); }
	AX_INLINE	const 	T*		localBufPtr	() const { return reinterpret_cast<const T*>(_localBuf); }

	//axLocalBuffer() { _getMemCheck() = _memCheckSignature(); }
	//~axLocalBuffer() { AX_ASSERT(_getMemCheck() == _memCheckSignature()); }

private:
	//using MemCheckType = const void*;
	//MemCheckType& _getMemCheck() { return *reinterpret_cast<MemCheckType*>(_localBuf + sizeof(T) * kLocalBufSize); }
	//MemCheckType _memCheckSignature() const { return _localBuf + 0x1F; }
	//char	_localBuf[sizeof(T) * LOCAL_BUF_SIZE + sizeof(MemCheckType)];

	char	_localBuf[sizeof(T) * LOCAL_BUF_SIZE];
};

template< class T >
class axLocalBuffer<T,0> {
public:
	static const 			axInt	kLocalBufSize = 0;

	AX_INLINE 			T*		localBufPtr	() 		 { return nullptr; }
	AX_INLINE 	const	T*		localBufPtr	() const { return nullptr; }
};
