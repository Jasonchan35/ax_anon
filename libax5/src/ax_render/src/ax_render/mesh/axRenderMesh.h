#pragma once

#include <ax_render/vertex/axRenderVertexArray.h>
#include <ax_render/vertex/axRenderIndexArray.h>

class axRenderMeshEdit;
class axRenderMesh;

class axRenderSubMesh : public axNonCopyable {
	using This = axRenderSubMesh;
public:
	using SubMesh			= axRenderSubMesh;
	using VertexDesc		= axRenderVertexDesc;
	using VertexType		= axRenderVertexType;
	using AttrId			= axRenderVertexAttrId;
	using PrimitiveType		= axRenderPrimitiveType;
	using IndexType			= axRenderIndexType;

	template<class T> using Enumerator = axArrayInterleaveEnumerator<T>;

	axRenderSubMesh() = default;
	axRenderSubMesh(This && ) = default;

	void create(PrimitiveType primitiveType, const VertexDesc& desc, IndexType indexType);

	bool			isEmpty() { return vertices.isEmpty() && indices.isEmpty(); }
	void			clear			();
	bool			canAddVertices	(axInt n) const;

	void			setVertexCount	(axInt n)	{ vertices.resize(n); }
	axInt			vertexCount		() const	{ return vertices.size(); }
	VertexDesc		vertexDesc		() const	{ return vertices.vertexDesc(); }

	PrimitiveType	primitiveType	() const	{ return _primitiveType; }
	IndexType		indexType		() const	{ return indices.indexType(); }

	void			generateNormal	();
	axInt			triCount		() const;

	void			combine			(const This& r);
	void			copy			(const This & r);

	bool			validateIndices	() const;

	void setIsInlineDraw(bool b) { vertices.setIsInlineDraw(b); indices.setIsInlineDraw(b); }

	axRenderVertexArray	vertices;
	axRenderIndexArray	indices;

friend class axRenderMesh;
protected:
	template<class INDEX_TYPE> bool _validateIndicesT() const;

	PrimitiveType	_primitiveType = PrimitiveType::Unknown;
	template<class Index> void _combineIndices(const This& r, axInt oldVertexCount);
};

class axRenderMesh : public axNonCopyable {
	using This = axRenderMesh;
public:
	using SubMesh		= axRenderSubMesh;
	using VertexDesc	= axRenderVertexDesc;
	using PrimType		= axRenderPrimitiveType;
	using IndexType		= axRenderIndexType;
	using VertexType	= axRenderVertexType;

	axRenderMesh() = default;
	axRenderMesh(This &&) = default;

	struct EditVertexProxy {
		SubMesh& subMesh;
		axIntRange range{0,0};

		axRenderIndexArray&	indices() & { return subMesh.indices; }

		bool	hasPositions	() const		{ return subMesh.vertices.hasPositions(); }
		bool	hasNormals		() const		{ return subMesh.vertices.hasNormals(); }
		bool	hasTangents		() const		{ return subMesh.vertices.hasTangents(); }
		bool	hasUvSet		(axInt i) const { return subMesh.vertices.hasUvSet(i); }
		bool	hasColorSet		(axInt i) const { return subMesh.vertices.hasColorSet(i); }
		bool	hasCustomSet	(axInt i) const { return subMesh.vertices.hasCustomSet(i); }

		template<class T>
		using E = axRenderVertexArray::Enumerator<T>;

		E<axFloat32x3>			positions	() &		{ return subMesh.vertices.editPositions	(range); }
		E<axFloat32x3>			normals		() &		{ return subMesh.vertices.editNormals	(range); }
		E<axFloat32x3>			tangents	() &		{ return subMesh.vertices.editTangents	(range); }
		E<axFloat32x2>			uvSet		(axInt i) &	{ return subMesh.vertices.editUvSet		(i, range); }
		E<axColorRGBAh>			colorSet	(axInt i) &	{ return subMesh.vertices.editColorSet	(i, range); }
	};

	template<class Vertex>
	struct EditVertexProxyT {
		SubMesh&			subMesh;
		axIntRange			range{0,0};
		axMutSpan<Vertex>	vertices;
	};

	void	clear();
	void	copy(const This& r);

		  SubMesh&	subMesh		(axInt index) &			{ return _subMeshes[index]; }
	const SubMesh&	subMesh		(axInt index) const &	{ return _subMeshes[index]; }

		  SubMesh&	addSubMesh	(PrimType primType, const VertexDesc& vertexDesc, IndexType indexType) &;
		  SubMesh&	getSubMeshCanAddVertices(PrimType primType, const VertexDesc& vertexDesc, IndexType indexType, axInt numVertices) &;

	template<class Vertex>
	EditVertexProxyT<Vertex> addVertices(PrimType primType, IndexType indexType, axInt n) {
		auto& sm = getSubMeshCanAddVertices(primType, Vertex::s_desc(), indexType, n);
		axIntRange range(sm.vertices.size(), n);
		auto v = sm.vertices.template add<Vertex>(n);
		return {sm, range, v};
	}

	template<class Vertex, class INDEX_TYPE>
	EditVertexProxyT<Vertex> addVertices(PrimType primType, axInt n) {
		return addVertices<Vertex>(primType, axRenderIndexTypeOf<INDEX_TYPE>(), n);
	}

	EditVertexProxy addVertices(PrimType primType, const VertexDesc& vertexDesc, IndexType indexType, axInt n) {
		auto& sm = getSubMeshCanAddVertices(primType, vertexDesc, indexType, n);
		auto range = sm.vertices.add(n);
		return {sm, range};
	}

	axMutSpan<SubMesh>	subMeshes() &		{ return _subMeshes; }
	   axSpan<SubMesh>	subMeshes() const &	{ return _subMeshes; }

	axInt			subMeshCount		() const			{ return _subMeshes.size(); }
	void			setSubMeshCount		(axInt newSize);
	void			reserveSubMeshCount	(axInt newSize);

	void			setAllocator	(axAllocator* allocator)	{ _subMeshes.setAllocator(allocator); }
	axAllocator*	allocator		() const					{ return _subMeshes.allocator(); }

	void		generateNormal	();

	axInt		sizeofIndex		() const;
	axInt		indexLimit		() const;

	axInt		vertexCount		() const;
	axInt		indexCount		() const;
	axInt		triCount		() const;

	bool		validateIndices	() const;

	axBBox3f	calcBoundingBox	() const;

	void transform(const axMat4f& mat);
	void combine(const This & r);
	void combine(const SubMesh & r);

	void setIsInlineDraw(bool b);
	bool isInlineDraw() const { return _isInlineDraw; }

	axRenderMeshEdit edit();

private:
	bool _isInlineDraw = false;
	axArray<SubMesh, 1>		_subMeshes;
};

//---------------------------------------

inline
bool axRenderSubMesh::canAddVertices(axInt n) const {
	return vertices.size() + n <= indices.indexLimit();
}
