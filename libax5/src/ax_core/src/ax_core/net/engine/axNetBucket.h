#pragma once

#include <ax_core/container/axIndirectArray.h>
#include <ax_core/container/axDList.h>
#include <ax_core/net/axSocket.h>

class axNetEngine;

class axNetBucket : protected axDListNode<axNetBucket> {
	using This = axNetBucket;
	using ListNode = axDListNode<axNetBucket>;
public:
	static const axInt kMaxSize = 8 * 1000;

	void reset() {
		_offset = 0;
		buf.clearAndFree();
	}

	bool tryAppend(axByteSpan span) {
		if (span.size() > avaliableSpace())
			return false;
		buf.appendRange(span);
		return true;
	}

	axInt avaliableSpace() const { return kMaxSize - buf.size(); }
	axByteSpan span() const { return buf.sliceFrom(_offset); }

	void advance(axInt n) {
		AX_ASSERT(n <= dataSize());
		_offset += n;
	}

	axInt dataSize() const { return buf.size() - _offset; }

	void appendData(axByteSpan s) { buf.appendRange(s); }

	axByteArray_<kMaxSize>	buf;

friend class axDList<This>;
protected:
	axInt _offset = 0;
};

AX_STATIC_ASSERT_MSG(ax_sizeof(axNetBucket) < 8 * 1024, "try to fit memory page");