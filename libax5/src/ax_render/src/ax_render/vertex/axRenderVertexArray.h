#pragma once

#include <ax_render/vertex/axRenderVertexDesc.h>
#include <ax_render/buffer/axRenderMultiGpuBuffer.h>

class axRenderVertexArray : public axNonCopyable {
	using This = axRenderVertexArray;
public:
	using VertexDesc	= axRenderVertexDesc;
	using VertexType	= axRenderVertexType;
	using AttrId		= axRenderVertexAttrId;

	axRenderVertexArray() = default;
	axRenderVertexArray(This &&) = default;

	bool isEmpty() const { return _vertexData.size() == 0; }
	axInt size() const { return ax_div(_vertexData.size(), sizeofVertex()); }

	template<class T> using Enumerator = axArrayInterleaveEnumerator<T>;

										void	create		(VertexDesc desc, axInt n);
	template<class Vertex>	axMutSpan<Vertex>	create		(axInt n);

										void	resize		(axInt n);
										void	incSize		(axInt n)	{ resize(size() + n); }
										void	reserve		(axInt n);

	template<class Vertex>	axMutSpan<Vertex>	add			(axInt n) &;
							axIntRange			add			(axInt n);

	template<class Vertex>	axSpan<Vertex>		get			() const &;
	template<class Vertex>	axMutSpan<Vertex>	edit		() &;
										void	clear		();

								bool	hasPositions	() const		{ return _desc->hasPositions(); }
								bool	hasNormals		() const		{ return _desc->hasNormals(); }
								bool	hasTangents		() const		{ return _desc->hasTangents(); }
								bool	hasUvSet		(axInt i) const	{ return _desc->hasUvSet(i); }
								bool	hasColorSet		(axInt i) const	{ return _desc->hasColorSet(i); }
								bool	hasCustomSet	(axInt i) const	{ return _desc->hasCustomSet(i); }

	template<class T>	Enumerator<T>	editVertexAttr(AttrId attrId, axIntRange range) &;

		axIntRange	dataRange() const { return axIntRange(0, size()); }

		Enumerator<axFloat32x3>			editPositions	(axIntRange range) & { return editVertexAttr<axFloat32x3>(AttrId::POSITION0, range); }
		Enumerator<axFloat32x3>			editNormals		(axIntRange range) & { return editVertexAttr<axFloat32x3>(AttrId::NORMAL0,   range); }
		Enumerator<axFloat32x3>			editTangents	(axIntRange range) & { return editVertexAttr<axFloat32x3>(AttrId::TANGENT0,  range); }

		Enumerator<axFloat32x2>			editUvSet		(axInt i, axIntRange range) &	{	if (i < 0 || i >= axRenderVertexAttrId_kUvCount)
																								throw axError_Undefined(AX_LOC);
																							return editVertexAttr<axFloat32x2>(AttrId::TEXCOORD0 + static_cast<axEnum<AttrId>::IntType>(i), range); }

		Enumerator<axColorRGBAh>		editColorSet	(axInt i, axIntRange range) &	{ 	if (i < 0 || i >= axRenderVertexAttrId_kColorCount )
																								throw axError_Undefined(AX_LOC);
																							return editVertexAttr<axColorRGBAh>(AttrId::COLOR0 + static_cast<axEnum<AttrId>::IntType>(i), range); }

		Enumerator<axFloat32x3>			editPositions	() &		{ return editPositions(dataRange()); }
		Enumerator<axFloat32x3>			editNormals		() &		{ return editNormals(dataRange()); }
		Enumerator<axFloat32x3>			editTangents	() &		{ return editTangents(dataRange()); }
		Enumerator<axFloat32x2>			editUvSet		(axInt i) & { return editUvSet(i, dataRange()); }
		Enumerator<axColorRGBAh>		editColorSet	(axInt i) & { return editColorSet(i, dataRange()); }

	template<class T> Enumerator<const T>	vertexAttr	(AttrId attrId) const { return ax_const_cast(this)->_vertexAttr<T>(attrId, axIntRange(0, size())); }

		Enumerator<const axFloat32x3>		positions	() const& { return vertexAttr<axFloat32x3>(AttrId::POSITION0); }
		Enumerator<const axFloat32x3>		normals		() const& { return vertexAttr<axFloat32x3>(AttrId::NORMAL0); }
		Enumerator<const axFloat32x3>		tangents	() const& {	return vertexAttr<axFloat32x3>(AttrId::TANGENT0);	}

		Enumerator<const axFloat32x2>		uvSet		(axInt i) const&	{	if (i < 0 || i >= axRenderVertexAttrId_kUvCount)
																					throw axError_Undefined(AX_LOC);
																				return vertexAttr<axFloat32x2>(AttrId::TEXCOORD0 + static_cast<axEnum<AttrId>::IntType>(i)); }

		Enumerator<const axColorRGBAh>		colorSet	(axInt i) const&	{	if (i < 0 || i >= axRenderVertexAttrId_kColorCount )
																					throw axError_Undefined(AX_LOC);
																				return vertexAttr<axColorRGBAh>(AttrId::COLOR0 + static_cast<axEnum<AttrId>::IntType>(i)); }

	void setGpuBufferDirty(axInt start, axInt n);

	axByteSpan	byteSpan	() const { return _vertexData.toByteSpan(); }

	VertexDesc	vertexDesc	() const { return _desc; }
	axInt		sizeofVertex() const { return _desc ? _desc->strideBytes : 0; }
	
	void copy	(const This& r);
	void append	(const This& r);

	axBBox3f calcBoundingBox() const;
	void transform(const axMat4f& mat);

	axRenderGpuBuffer*	uploadToGpu(axRenderRequest& req);

	void setIsInlineDraw(bool b) { _isInlineDraw = b; }
	bool isInlineDraw() const { return _isInlineDraw; }

private:
	template<class T>	Enumerator<T>	_vertexAttr(AttrId attrId, axIntRange range) & { return _vertexAttr<T>(_desc->attr(attrId), range); }
	template<class T>	Enumerator<T>	_vertexAttr(const VertexDesc::Attr& attr, axIntRange range) &;

	VertexDesc			_desc = nullptr;
	axByteArray			_vertexData;
	axSPtr<axRenderMultiGpuBuffer>	_multiGpuBuffer;
	axIntRange _gpuBufferDirtyInBytes{0,0};
	bool _isInlineDraw = false;
};

template<class Vertex> inline
axMutSpan<Vertex> axRenderVertexArray::create(axInt n) {
    create(Vertex::s_desc(), n);
    return edit<Vertex>();
}

template<class Vertex> inline
axSpan<Vertex> axRenderVertexArray::get() const & {
	if (_desc->vertexType != Vertex::kType) throw axError_Undefined(AX_LOC);
	return axSpan<Vertex>::s_fromByteSpan(_vertexData.toByteSpan());
}

template<class Vertex> inline
axMutSpan<Vertex> axRenderVertexArray::edit() & {
	if (_desc->vertexType != Vertex::kType) throw axError_Undefined(AX_LOC);
	setGpuBufferDirty(0, size());
	return axMutSpan<Vertex>::s_fromByteSpan(_vertexData.toByteSpan());
}

template<class Vertex> inline
axMutSpan<Vertex> axRenderVertexArray::add(axInt n) & {
	if (_desc->vertexType != Vertex::kType) throw axError_Undefined(AX_LOC);
	resize(size() + n);
	return edit<Vertex>().sliceBack(n);
}

inline
axIntRange axRenderVertexArray::add(axInt n) {
	axIntRange o{size(), n};
	resize(size() + n);
	return o;
}

template<class T> inline
axRenderVertexArray::Enumerator<T> axRenderVertexArray::_vertexAttr(const VertexDesc::Attr& attr, axIntRange range) & {
	auto dataType = axRenderDataTypeUtil::typeOf<T>();

	if (dataType != attr.dataType)
		throw axError_Undefined(AX_LOC);

	if (attr.sizeInBytes != ax_sizeof(T))
		throw axError_Undefined(AX_LOC);

	if (!attr.active)
		throw axError_Undefined(AX_LOC);

	if (range.end() > size())
		throw axError_Undefined(AX_LOC);

	auto stride = sizeofVertex();

	auto* s = _vertexData.data() + range.start * stride + attr.offset;
	auto* e = _vertexData.data() + range.end() * stride + attr.offset;
	return Enumerator<T>(reinterpret_cast<T*>(s), reinterpret_cast<T*>(e), stride);
}

template<class T> inline
axRenderVertexArray::Enumerator<T> axRenderVertexArray::editVertexAttr(AttrId attr, axIntRange range) & {
	setGpuBufferDirty(range.start, range.size);
	return _vertexAttr<T>(attr, range);
}
