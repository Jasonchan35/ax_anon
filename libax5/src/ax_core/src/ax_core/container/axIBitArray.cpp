#include "axIBitArray.h"

template<class BLOCK>
void	axIBitArrayT<BLOCK>::_reserveBlocks(axInt newBlockCount) {
	auto oldBlockCount = blockCount();

	auto buf = onMalloc(newBlockCount);
	if (buf.data() != _blockData) {
		if (_size) {
			ax_memcpy(buf.data(), _blockData, oldBlockCount * ax_sizeof(Block));
		}
		if (_blockData) {
			onFree(_blockData);
		}
		_blockData = buf.data();
	}

	_capacity = buf.size() * kBitsPerBlock;
}

template<class BLOCK>
void axIBitArrayT<BLOCK>::_resize(axInt newSize, bool newValue) {
	auto oldBlockCount = blockCount();
	auto newBlockCount = ax_roundup_div(newSize, kBitsPerBlock);
	reserveBlocks(newBlockCount);

	auto curBlock = oldBlockCount;
	if (newValue && oldBlockCount > 0) {
		auto startBit = _size % kBitsPerBlock;
		if (startBit) {
			Block mask = 0;
			for (axInt i = startBit; i < kBitsPerBlock; i++) {
				mask |= Block(1) << i;
			}
			_blockData[curBlock-1] |= ~mask;
		}
	}

	{
		Block mask = newValue ? Block(~0) : Block(0);
		axArrayUtil::constructor(_blockData + curBlock, newBlockCount - curBlock, mask);
	}
	_size = newSize;
}

#define	AX_MACRO_OP(T)	\
	template class axIBitArrayT<T>; \
//-------------
	AX_TYPE_LIST_UINT(AX_MACRO_OP);
#undef AX_MACRO_OP
