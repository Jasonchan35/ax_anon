#include "axRenderMesh.h"

void axRenderSubMesh::create(PrimitiveType primitiveType, const VertexDesc& desc, IndexType indexType) {
	_primitiveType = primitiveType;
	vertices.create(desc, 0);
	indices.create(indexType, 0);
}

void axRenderSubMesh::clear() {
	vertices.clear();
	indices.clear();
}

void axRenderSubMesh::generateNormal() {
	if (indexType() != IndexType::UInt16) {
		AX_ASSERT(false); // only support indexed at the moment
		return;
	}

	auto pos = vertices.editPositions();
	auto outNormal = vertices.editNormals();

	axArray<axInt, 2048> normalSharedCount;

	axInt vertexCount = vertices.size();
	normalSharedCount.resize(vertexCount);

	for (axInt i = 0; i < vertexCount; i++) {
		outNormal[i]->set(0,0,0);
	}

	auto triCount = indices.size() / 3;
	using Index = axUInt16;
	auto triIndices = indices.get<Index>();

	auto* idx = triIndices.begin();
	for (axInt tri = 0; tri < triCount; tri++) {
		auto vi0 = *idx; idx++;
		auto vi1 = *idx; idx++;
		auto vi2 = *idx; idx++;

		auto p0 = axVec3f(*pos[vi0]);
		auto p1 = axVec3f(*pos[vi1]);
		auto p2 = axVec3f(*pos[vi2]);

		auto a = (p0 - p1).normal();
		auto b = (p0 - p2).normal();

		auto nl = a.cross(b);

		normalSharedCount[vi0]++;
		normalSharedCount[vi1]++;
		normalSharedCount[vi2]++;

		*outNormal[vi0] += axFloat32x3(nl);
		*outNormal[vi1] += axFloat32x3(nl);
		*outNormal[vi2] += axFloat32x3(nl);
	}

	AX_ASSERT(idx == triIndices.end());

	for (axInt i = 0; i < vertexCount; i++) {
		auto& nl = *outNormal[i];
		nl = (axVec3f(nl) / (float)normalSharedCount[i]).normal();
	}
}

axInt axRenderSubMesh::triCount() const {
	switch (indexType()) {
		case IndexType::Unknown:	return vertices.size() / 3;
		case IndexType::UInt16:		return indices.size() / 3;
		case IndexType::UInt32:		return indices.size() / 3;
		default: AX_ASSERT(false);	return 0;
	}
}

void axRenderSubMesh::combine(const This& r) {
	if (primitiveType() != r.primitiveType()
		|| vertexDesc() != r.vertexDesc()
		|| indexType()  != r.indexType())
	{
		throw axError_Undefined(AX_LOC);
	}

	auto oldVertexCount = vertices.size();
	vertices.append(r.vertices);
	
	switch (indexType()) {
		case IndexType::Unknown:  break;
		case IndexType::UInt16: _combineIndices<axUInt16>(r, oldVertexCount); break;
		case IndexType::UInt32: _combineIndices<axUInt32>(r, oldVertexCount); break;
		default: AX_ASSERT(false); break;
	}
}

template<class Index>
void axRenderSubMesh::_combineIndices(const This& r, axInt oldVertexCount) {
	auto offset = static_cast<Index>(oldVertexCount);

	auto srcView = r.indices.get<Index>();
	auto dstView = indices.add<Index>(srcView.size());
	auto dst = dstView.begin();

	for (auto& src : srcView) {
		*dst = src + offset;
		dst++;
	}
	AX_ASSERT(dst == dstView.end());
}

void axRenderSubMesh::copy(const This & r) {
	create(r.primitiveType(), r.vertexDesc(), r.indexType());
	vertices.copy(r.vertices);
	indices.copy(r.indices);
}

template<class INDEX_TYPE>
bool axRenderSubMesh::_validateIndicesT() const {
	auto span = axSpan<axUInt16>::s_fromByteSpan(indices.byteSpan());
	auto vc = vertices.size();

	for (auto e : span) {
		if (e >= vc)
			return false;
	}
	return true;
}

bool axRenderSubMesh::validateIndices() const {
	switch (indices.indexType()) {
		case IndexType::UInt16: return _validateIndicesT<axUInt16>();
		case IndexType::UInt32: return _validateIndicesT<axUInt32>();
	}
	return true;
}

void axRenderMesh::clear() {

	for (auto& sm : _subMeshes) {
		sm.clear();
	}
}

void axRenderMesh::copy(const This& r) {
	clear();
//	create(r.vertexType(), r.primitiveType(), r.indexType());
	auto n = r.subMeshCount();
	setSubMeshCount(n);
	for (axInt i = 0; i < n; i++) {
		_subMeshes[i].copy(r._subMeshes[i]);
	}
}

void axRenderMesh::setSubMeshCount(axInt newSize) {
	auto old_size = _subMeshes.size();
	_subMeshes.resize(newSize);
	for (auto i = old_size; i < newSize; i++ ) {
		auto& sm = _subMeshes[i];
		sm.setIsInlineDraw(_isInlineDraw);
	}
}

void axRenderMesh::reserveSubMeshCount(axInt newSize) {
	_subMeshes.reserve(newSize);
}

axRenderSubMesh & axRenderMesh::getSubMeshCanAddVertices(PrimType primType, const VertexDesc& vertexDesc, IndexType indexType, axInt numVertices) & {
	for (auto& sm : _subMeshes) {
		if (sm.isEmpty()) {
			sm.create(primType, vertexDesc, indexType);
			return sm;
		}

		if (sm.vertexDesc()		!= vertexDesc) continue;
		if (sm.indexType()		!= indexType ) continue;
		if (sm.primitiveType()	!= primType  ) continue;
		if (sm.canAddVertices(numVertices))
			return sm;
	}

	return addSubMesh(primType, vertexDesc, indexType);
}

axRenderMesh::SubMesh & axRenderMesh::addSubMesh(PrimType primType, const VertexDesc& vertexDesc, IndexType indexType) & {
	setSubMeshCount(subMeshCount() + 1);
	auto& sm = _subMeshes.back();
	sm.create(primType, vertexDesc, indexType);
	return sm;
}

axInt axRenderMesh::vertexCount() const {
	axInt c = 0;
	for (auto& sm : _subMeshes) {
		//AX_ASSERT(_desc				== &sm.vertices.desc());
		//AX_ASSERT(_primitiveType	== sm.primitiveType());
		//AX_ASSERT(_indexType		== sm.indexType());
		c += sm.vertices.size();
	}
	return c;
}

axInt axRenderMesh::indexCount() const {
	axInt c = 0;
	for (auto& sm : _subMeshes) {
		c += sm.indices.size();
	}
	return c;
}

axInt axRenderMesh::triCount() const {
	axInt c = 0;
	for (auto& sm : _subMeshes) {
		c += sm.triCount();
	}
	return c;
}

bool axRenderMesh::validateIndices() const {
	for (auto& sm : _subMeshes) {
		if (!sm.validateIndices())
			return false;
	}
	return true;
}

axBBox3f axRenderMesh::calcBoundingBox() const {
	axBBox3f box;
	for (auto& sm : _subMeshes) {
		//AX_ASSERT(_desc				== &sm.vertices.desc());
		//AX_ASSERT(_primitiveType	== sm.primitiveType());
		//AX_ASSERT(_indexType		== sm.indexType());
		box.encapsulate(sm.vertices.calcBoundingBox());
	}
	return box;
}

void axRenderMesh::transform(const axMat4f& mat) {
	for (auto& s : _subMeshes) {
		s.vertices.transform(mat);
	}
}

void axRenderMesh::combine(const This & r) {
	for (auto& s : r.subMeshes()) {
		combine(s);
	}
}

void axRenderMesh::combine(const SubMesh & r) {
	auto& dst = getSubMeshCanAddVertices(r.primitiveType(), r.vertexDesc(), r.indexType(), r.vertices.size());
	dst.combine(r);
}

void axRenderMesh::setIsInlineDraw(bool b) {
	_isInlineDraw = b;
	for (auto& s : _subMeshes) {
		s.setIsInlineDraw(b);
	}
}
