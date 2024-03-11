#pragma once

#include <ax_render/vertex/axRenderVertexDesc.h>
#include <ax_render/buffer/axRenderMultiGpuBuffer.h>

template<class Index> constexpr axRenderIndexType axRenderIndexTypeOf();

template<> constexpr axRenderIndexType axRenderIndexTypeOf<void>() { return axRenderIndexType::Unknown; }
template<> constexpr axRenderIndexType axRenderIndexTypeOf<axUInt16>() { return axRenderIndexType::UInt16; }
template<> constexpr axRenderIndexType axRenderIndexTypeOf<axUInt32>() { return axRenderIndexType::UInt32; }

inline
axInt axRenderIndexTypeSizeInBytes(axRenderIndexType t) {
	switch (t) {
		case axRenderIndexType::Unknown:	return 0;
		case axRenderIndexType::UInt16:	return ax_sizeof(axUInt16);
		case axRenderIndexType::UInt32:	return ax_sizeof(axUInt32);
		default: AX_ASSERT(false);	return 0;
	}
}

inline
axInt axRenderIndexTypeLimit(axRenderIndexType t) {
	switch (t) {
		case axRenderIndexType::Unknown:	return axNumLimit<axInt>::max();
		case axRenderIndexType::UInt16:		return axNumLimit<axUInt16>::max();
		case axRenderIndexType::UInt32:		return axNumLimit<axUInt32>::max();
		default: AX_ASSERT(false);	return 0;
	}
}

class axRenderIndexArray : public axNonCopyable {
	using This = axRenderIndexArray;
public:
	using PrimitiveType = axRenderPrimitiveType;
	using IndexType		= axRenderIndexType;

	axRenderIndexArray() = default;
	axRenderIndexArray(This &&) = default;

	void clear();

	IndexType	indexType		() const { return _indexType; }

	void		resize			(axInt n);
	void		incSize			(axInt n) { resize(size() + n); }
	void		reserve			(axInt n);

	bool		isEmpty			() const { return _indexData.size() == 0; }
	axInt		size			() const { return ax_div(_indexData.size(), sizeofIndex()); }

	axInt		sizeofIndex		() const { return axRenderIndexTypeSizeInBytes(_indexType); }
	axInt		indexLimit		() const { return axRenderIndexTypeLimit(_indexType); }

	template<class Index>	void	set		(axSpan<Index> src);
	template<class Index>	void	append	(axSpan<Index> src);

										void	create	(IndexType indexType, axInt n);
	template<class Index>	axMutSpan<Index>	create	(axInt n) &;

	template<class Index>	axMutSpan<Index>	add		(axInt n) &;

	template<class Index>	axSpan<Index>		get		() const;
	template<class Index>	axMutSpan<Index>	edit	(axIntRange range) & ;
	template<class Index>	axMutSpan<Index>	edit	() & { return edit<Index>(axIntRange(0, size())); }

										void setGpuBufferDirty(axInt start, axInt n);

	axByteSpan	byteSpan		() const { return _indexData.toByteSpan(); }

	void copy(const This& r);

	axRenderGpuBuffer*	uploadToGpu(axRenderRequest& req);

	void setIsInlineDraw(bool b) { _isInlineDraw = b; }
	bool isInlineDraw() const { return _isInlineDraw; }

private:
	IndexType				_indexType = IndexType::Unknown;
	axByteArray				_indexData;
	axSPtr<axRenderMultiGpuBuffer>	_multiGpuBuffer;
	axIntRange	_gpuBufferDirtyInBytes {0,0};
	bool _isInlineDraw = false;
};

//------------

template<class Index>
void axRenderIndexArray::set(axSpan<Index> src) {
	if (ax_sizeof(Index) != sizeofIndex()) throw axError_Undefined(AX_LOC);
	resize(src.size());
	_indexData = src.reinterpretByteSpan();
	setGpuBufferDirty(0, src.size());
}

template<class Index> inline
void axRenderIndexArray::append(axSpan<Index> view) {
	if (ax_sizeof(Index) != sizeofIndex()) throw axError_Undefined(AX_LOC);
	setGpuBufferDirty(size(), view.size());
	_indexData.appendRange(view.toByteSpan());
}

template<class Index>
axSpan<Index> axRenderIndexArray::get() const {
	if (ax_sizeof(Index) != sizeofIndex()) throw axError_Undefined(AX_LOC);
	return axSpan<Index>::s_fromByteSpan(_indexData.toByteSpan());
}

template<class Index>
axMutSpan<Index> axRenderIndexArray::edit(axIntRange range) & {
	if (ax_sizeof(Index) != sizeofIndex()) throw axError_Undefined(AX_LOC);
	setGpuBufferDirty(range.start, range.size);
	return axMutSpan<Index>::s_fromByteSpan(_indexData.toByteSpan());
}

template<class Index> inline
axMutSpan<Index> axRenderIndexArray::create(axInt n) & {
	create(axRenderIndexTypeOf<Index>(), n);
	return edit<Index>();
}

template<class Index> inline
axMutSpan<Index> axRenderIndexArray::add(axInt n) & {
	if (ax_sizeof(Index) != sizeofIndex()) throw axError_Undefined(AX_LOC);
	resize(size() + n);
	return edit<Index>().sliceBack(n);
}

inline void axRenderIndexArray::setGpuBufferDirty(axInt start, axInt n) { _gpuBufferDirtyInBytes |= axIntRange(start, n) * sizeofIndex(); }
