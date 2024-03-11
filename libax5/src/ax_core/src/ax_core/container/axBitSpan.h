#pragma once

#include "../string/axFormat.h"

using axBitArray_DefaultBlock = axUInt32;

template<class BLOCK>
class axBitAccessorT {
public:
	using Block = BLOCK;
	static const axInt kBitsPerBlock = ax_sizeof(Block) * 8;
	axBitAccessorT(Block* blockData, axInt index) : _blockData(blockData), _index(index) {}

	AX_INLINE					Block&	block()			{ return _blockData[_index / kBitsPerBlock]; }
	AX_INLINE			const	Block&	block() const	{ return _blockData[_index / kBitsPerBlock]; }

	AX_INLINE	void operator=(bool v)	{ set(v); }

	AX_INLINE	explicit operator bool () const { return get(); }

	AX_INLINE	bool	get		() const	{ return (block() & _blockBit()) != 0; }
	AX_INLINE	void	set		(bool b)	{ return b ? on() : off(); }

	AX_INLINE	void	on		()	{ block() |=  _blockBit(); }
	AX_INLINE	void	off		()	{ block() &= ~_blockBit(); }
	AX_INLINE	void	toggle	()	{ block() ^=  _blockBit(); }

	bool operator==(bool v) const { return get() == v; }
	bool operator!=(bool v) const { return get() != v; }

private:
	AX_INLINE	Block	_blockBit		() const { return static_cast<Block>(Block(1) << (_index % kBitsPerBlock)); }

	Block*	_blockData;
	axInt	_index;
};

template<class BLOCK>
class axBitMutSpanT {
public:
	AX_STATIC_ASSERT(axTypeTraits::isUnsignedInt<BLOCK>::value);

	using Block = BLOCK;
	using Byte  = typename axTypeTraits::selectConst<axByte, BLOCK>::Type;
	using Accessor  = axBitAccessorT<Block>;
	using CAccessor = axBitAccessorT<const Block>;

	static const axInt kBitsPerBlock = ax_sizeof(Block) * 8;

	AX_INLINE axBitMutSpanT(Block* blockData, axInt size) : _blockData(blockData), _size(size) {}

	AX_INLINE			Block*	blockData	()			{ return _blockData; }
	AX_INLINE	const	Block*	blockData	() const	{ return _blockData; }

	AX_INLINE			axInt	size		() const		{ return _size; }
	AX_INLINE			bool	inBound		(axInt i) const	{ return i >= 0 && i < _size; }

	AX_INLINE			axInt	sizeInBytes	() const	{ return blockCount() * ax_sizeof(Block); }
	AX_INLINE	axMutSpan<Byte>	byteSpan	()			{ return axMutSpan<Byte>(reinterpret_cast<Byte*>(_blockData), sizeInBytes()); }
	AX_INLINE	axByteSpan		byteSpan	() const	{ return      axByteSpan(reinterpret_cast<Byte*>(_blockData), sizeInBytes()); }

	AX_INLINE			axInt	blockCount	() const	{ return ax_roundup_div(_size, kBitsPerBlock); }
	AX_INLINE	axSpan<Block>	blockSpan	() const	{ return axSpan<Block>(_blockData, blockCount()); }

	AX_INLINE					Block	blockBit	(axInt i) const { return Block(1) << (i % kBitsPerBlock); }

	AX_INLINE					Block&	block		(axInt i)		{ _debug_checkBound(i); return _blockData[i / kBitsPerBlock]; }
	AX_INLINE			const	Block&	block		(axInt i) const	{ _debug_checkBound(i); return _blockData[i / kBitsPerBlock]; }

	AX_INLINE	Accessor	at(axInt i)						{ _debug_checkBound(i); return Accessor( _blockData, i); }
	AX_INLINE	CAccessor	at(axInt i)	const				{ _debug_checkBound(i); return CAccessor(_blockData, i); }

	AX_INLINE	Accessor	operator[](axInt i)				{ return at(i); }
	AX_INLINE	CAccessor	operator[](axInt i)	const		{ return at(i); }

	void	onFormat(axFormat& f) const {
		for (axInt i = 0; i < _size; i++ ) {
			if ( i > 0) {
				if (i % 64 == 0)
					f << "\n";
				else if (i % 8 == 0)
					f << "  ";
				else if (i % 4 == 0)
					f << '_';
			}

			f << (at(i) ? '1' : '0');
		}
	}

private:
    AX_INLINE void _checkBound			( axInt i ) const { if( ! inBound(i) ) throw axArrayError_OutOfRange(); }
	AX_INLINE void	_debug_checkBound	( axInt i ) const {
		#ifdef AX_GEN_CONFIG_Debug
			_checkBound(i);
		#endif
	}

	Block*	_blockData;
	axInt	_size;
};

template<class T> using axBitSpanT = axBitMutSpanT<const T>;

using axBitMutSpan	= axBitMutSpanT<axBitArray_DefaultBlock>;
using axBitSpan		= axBitSpanT<axBitArray_DefaultBlock>;
