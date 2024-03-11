#pragma once

#include "axBitSpan.h"

template<class BLOCK, axInt N>
class axFixedBitArrayLocalBuffer {
public:
	static const axInt kBitsPerBlock = ax_sizeof(BLOCK) * 8;
	static const axInt kBlockCount   = ax_roundup_div(N, kBitsPerBlock);

	AX_INLINE			BLOCK*	blockData()			{ return reinterpret_cast<	    BLOCK*>(_blockData); }
	AX_INLINE	const 	BLOCK*	blockData() const	{ return reinterpret_cast<const BLOCK*>(_blockData); }

protected:
	BLOCK _blockData[kBlockCount];
};

template<class BLOCK>
class axFixedBitArrayLocalBuffer<BLOCK, 0> {
public:
	static const axInt kBitsPerBlock = ax_sizeof(BLOCK) * 8;
	static const axInt kBlockCount   = 0;

	AX_INLINE			BLOCK*	blockData	() 		 { return nullptr; }
	AX_INLINE	const 	BLOCK*	blockData	() const { return nullptr; }
};

template<class BLOCK, axInt N>
class axFixedBitArrayT : public axFixedBitArrayLocalBuffer<BLOCK, N> {
	using LocalBuf = axFixedBitArrayLocalBuffer<BLOCK, N>;
public:
	static const axInt kSize = N;

	using Block		= BLOCK;
	using MutSpan	= axBitMutSpanT<BLOCK>;
	using Span		= axBitSpanT<BLOCK>;

	using Accessor  = axBitAccessorT<Block>;
	using CAccessor = axBitAccessorT<const Block>;

	using LocalBuf::blockData;
	using LocalBuf::kBitsPerBlock;
	using LocalBuf::kBlockCount;

	axFixedBitArrayT() {
		auto* s = blockData();
		auto* e = s + kBlockCount;
		for (;s < e; s++)
			*s = 0;
	}

	AX_INLINE	MutSpan	mutSpan	()			{ return MutSpan(blockData(), kSize); }
	AX_INLINE	MutSpan	span	()			{ return MutSpan(blockData(), kSize); }
	AX_INLINE	Span	span	() const	{ return    Span(blockData(), kSize); }

	AX_INLINE	axByteSpan	byteSpan() const { return span().byteSpan(); }

	AX_INLINE	axInt	size		() const		{ return N; }
	AX_INLINE	bool	inBound		(axInt i) const	{ return i >= 0 && i < N; }

	AX_INLINE	Accessor	at(axInt i)						{ _debug_checkBound(i); return Accessor( blockData(), i); }
	AX_INLINE	CAccessor	at(axInt i)	const				{ _debug_checkBound(i); return CAccessor(blockData(), i); }

	AX_INLINE	Accessor	operator[](axInt i)				{ return at(i); }
	AX_INLINE	CAccessor	operator[](axInt i)	const		{ return at(i); }

	AX_INLINE	void	onFormat(axFormat& f) const { f << span(); }

private:
    AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}
};

template<axInt N> using axFixedBitArray = axFixedBitArrayT<axBitArray_DefaultBlock, N>;