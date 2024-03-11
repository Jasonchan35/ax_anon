#pragma once

#include "axBitSpan.h"
#include "axIArray.h"

template<class BLOCK>
class axIBitArrayT : public axNonCopyable {
	using This = axIBitArrayT;
public:
	using Block		= BLOCK;
	using MutSpan	= axBitMutSpanT<BLOCK>;
	using Span		= axBitMutSpanT<const BLOCK>;

	using Accessor  = axBitAccessorT<Block>;
	using CAccessor = axBitAccessorT<const Block>;

	static const axInt kBitsPerBlock = ax_sizeof(BLOCK) * 8;

	virtual ~axIBitArrayT() {
		AX_ASSERT_MSG(_blockData == nullptr, "axIBitArray data is not freed");
	}

	AX_INLINE	MutSpan	mutSpan	()			{ return MutSpan(_blockData, _size); }
	AX_INLINE	MutSpan	span	()			{ return MutSpan(_blockData, _size); }
	AX_INLINE	Span	span	() const	{ return    Span(_blockData, _size); }

	AX_INLINE	axByteSpan	byteSpan() const	{ return span().byteSpan(); }

	AX_INLINE	void	clear		()			{ resize(0); }
	AX_INLINE	void	clearAndFree()			{ _clearAndFree(); }

	AX_INLINE	axInt	size	() const		{ return _size; }
	AX_INLINE	bool	inBound	(axInt i) const	{ return i >= 0 && i < _size; }

	AX_INLINE	void	resize	(axInt newSize, bool newValue = false) { if (newSize <= _size) _size = newSize; else _resize(newSize, newValue); }

	AX_INLINE	axInt	capacity		() const			{ return _capacity; }
	AX_INLINE	axInt	blockCapacity	() const			{ return ax_roundup_div(_capacity, kBitsPerBlock); }

	AX_INLINE 	void	reserve		( axInt newSize )		{ reserveBlocks(ax_roundup_div(newSize, kBitsPerBlock)); }
	AX_INLINE 	void	reserveMore	( axInt n )				{ if (n < 0) { AX_ASSERT(false); return; } reserve(_size + n); }
	AX_INLINE 	void	reserveLess	( axInt n )				{ if (n < 0) { AX_ASSERT(false); return; } reserve(_size - n); }
	AX_INLINE	void	reserveBlocks(axInt newBlockCount)	{ if (newBlockCount > blockCapacity()) { _reserveBlocks(newBlockCount); } }

	AX_INLINE	axInt	blockCount	() const				{ return ax_roundup_div(_size, kBitsPerBlock); }

	AX_INLINE	Accessor	at(axInt i)						{ _debug_checkBound(i); return Accessor( _blockData, i); }
	AX_INLINE	CAccessor	at(axInt i)	const				{ _debug_checkBound(i); return CAccessor(_blockData, i); }

	AX_INLINE	Accessor	operator[](axInt i)				{ return at(i); }
	AX_INLINE	CAccessor	operator[](axInt i)	const		{ return at(i); }

	AX_INLINE	void	onFormat(axFormat& f) const { f << span(); }

	virtual bool isDataMovable() const = 0;

protected:
    AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

	void	_resize(axInt newSize, bool newValue);

	axIBitArrayT() = default;

	virtual	axMutSpan<Block>	onMalloc(axInt reqBlockCount) = 0;
	virtual	void				onFree	(Block* p) = 0;

	void _reserveBlocks(axInt newBlockCount);

	void _clearAndFree() {
		clear();
		if (_blockData) {
			onFree(_blockData);
			_blockData = nullptr;
			_capacity = 0;
		}
	}

	Block*	_blockData = nullptr;
	axInt	_size = 0;
	axInt	_capacity = 0;
	axAllocator* _allocator = nullptr;
};

using axIBitArray = axIBitArrayT<axBitArray_DefaultBlock>;