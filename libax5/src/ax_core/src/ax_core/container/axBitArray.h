#pragma once

#include "axIBitArray.h"

template<class BLOCK, axInt N>
class axBitArrayLocalBuffer {
public:
	static const axInt kBitsPerBlock = ax_sizeof(BLOCK) * 8;
	static const axInt kBlockCount   = ax_roundup_div(N, kBitsPerBlock);

	AX_INLINE			BLOCK*	localBufPtr	() 		 { return reinterpret_cast<	     BLOCK*>(_localBuf); }
	AX_INLINE	const 	BLOCK*	localBufPtr	() const { return reinterpret_cast<const BLOCK*>(_localBuf); }

protected:
	BLOCK _localBuf[kBlockCount];
};

template<class BLOCK>
class axBitArrayLocalBuffer<BLOCK, 0> {
public:
	static const axInt kBitsPerBlock = axBitSpan::kBitsPerBlock;
	static const axInt kBitCount     = 0;
	static const axInt kBlockCount   = 0;

	AX_INLINE			BLOCK*	localBufPtr	() 		 { return nullptr; }
	AX_INLINE	const 	BLOCK*	localBufPtr	() const { return nullptr; }
};

const axInt axBitArray_kDefaultSize = 64;

template<class BLOCK, axInt N = axBitArray_kDefaultSize >
class axBitArrayT : public axIBitArrayT<BLOCK>, axBitArrayLocalBuffer<BLOCK, N> {
	using Base = axIBitArrayT<BLOCK>;
	using This = axBitArrayT;
	using LocalBuf = axBitArrayLocalBuffer<BLOCK, N>;
public:
	using Block = BLOCK;
	using Base::_blockData;
	static const axInt kLocalBufBlockCount = LocalBuf::kBlockCount;

	virtual	~axBitArrayT() { Base::clearAndFree(); }

	virtual bool isDataMovable() const override { return _blockData != LocalBuf::localBufPtr(); }

protected:
	
	const	Block*	getLocalBufferPtr() const { return LocalBuf::localBufPtr(); }

	virtual	axMutSpan<Block>	onMalloc(axInt reqSize) override {
		if (reqSize <= kLocalBufBlockCount) {
			return axMutSpan<Block>(LocalBuf::localBufPtr(), kLocalBufBlockCount);
		} else {
			auto oldCap = Base::blockCapacity();
			auto newCapacity = ax_max(reqSize, ax_next_pow2_half(oldCap)); // reqSize or align to next pow of 2.5
			auto* p = ax_malloc(Block, newCapacity, 0);
			return axMutSpan<Block>(p, newCapacity);
		}
	}

	virtual	void onFree(Block* p) override {
		//std::cout << this << " onFree   " << "\n";
		if (p != LocalBuf::localBufPtr()) {
			ax_free(p);
		}
	}
};

using axBitArray = axBitArrayT<axBitArray_DefaultBlock, axBitArray_kDefaultSize>;
template<axInt N> using axBitArray_ = axBitArrayT<axBitArray_DefaultBlock, N>;

