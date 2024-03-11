#include "GEO_GeoData_RenderMeshHelper.h"

namespace Anon {

void GEO_GeoData_RenderMeshHelper::createMesh(GEO_GeoData& geo, axRenderMesh& mesh) {
	using Vertex = axRenderVertex_PosUvColorNormal;
	using Index  = axUInt16;

	auto color = axColorRGBAh_make(Color(1,1,1));
	auto primType	= axRenderPrimitiveType::Triangles;

	auto vertexSpan = geo.readVertices();

	auto vertexNormals = geo.getOrCreateVertexNormals(false);
	auto srcPos = geo.readPoints();

	mesh.clear();

	for (auto& prim : geo.readPrimitives()) {
		auto srcSpan = vertexSpan.slice(prim.vertexRange);
		auto srcNormals = vertexNormals.slice(prim.vertexRange);

		axInt tri = prim.vertexRange.size - 2;
		if (tri <= 0) continue;

		auto edit = mesh.addVertices<Vertex, Index>(primType, prim.vertexRange.size);
		if (edit.vertices.size() <= 0) continue;

		{
			auto dst = ax_foreach_begin(edit.vertices);
			auto src = srcSpan.begin();

			for (axInt i = 0; i < srcSpan.size(); i++) {
				dst->pos.setByCast(srcPos[src[i].point].pos);
				dst->normal.setByCast(srcNormals[i]);
				dst->color[0] = color;
				dst++;
			}
		}

		{
			// TODO: proper polygon triangulation
			auto editIndices = edit.subMesh.indices.add<Index>(tri * 3);
			auto dst = ax_foreach_begin(editIndices);

			Index base = static_cast<Index>(edit.range.start);
			Index j = base;
			for (axInt i = 2; i < srcSpan.size(); i++, j++) {
				dst[0] = base;
				dst[1] = j + 1;
				dst[2] = j + 2;
				dst += 3;
			}
		}
	}

//	AX_ASSERT(mesh.validateIndices());
}

void GEO_GeoData_RenderMeshHelper::createPointIdsMesh(GEO_GeoData& geo, axRenderMesh& mesh) {
	using Vertex = axRenderVertex_PosUv2Color;

	mesh.clear();
	auto editMesh = mesh.edit();

	axTempString text;
	axInt i = 0;
	for (auto& pt : geo.readPoints()) {
		text.format("{?}", i);
		editMesh.addTextBillboard(Vertex::s_desc(), text, axVec3f::s_cast(pt.pos), nullptr);
		i++;
	}
}

void GEO_GeoData_RenderMeshHelper::createVertexIdsMesh(GEO_GeoData& geo, axRenderMesh& mesh, double distanceToPoint) {
	using Vertex = axRenderVertex_PosUv2Color;

	mesh.clear();
	auto editMesh = mesh.edit();

	auto pointSpan   = geo.readPoints();
	auto verticeSpan = geo.readVertices();

	auto primCenterSpan = geo.getOrCreatePrimitiveCenters();

	axTempString text;
	axInt primIndex = 0;
	for (auto& prim : geo.readPrimitives()) {
		auto faceVertices = verticeSpan.slice(prim.vertexRange);

		axInt fvIndex = 0;
		for (auto& fv : faceVertices) {
			text.format("{?}:{?}", primIndex, fvIndex);

			auto pos = pointSpan[fv.point].pos;
			auto d = (pos - primCenterSpan[primIndex]).normal();
			pos -= d * distanceToPoint;

			editMesh.addTextBillboard(Vertex::s_desc(), text, axVec3f::s_cast(pos), nullptr);
			fvIndex++;
		}
		primIndex++;
	}
}

void GEO_GeoData_RenderMeshHelper::createEdgeIdsMesh(GEO_GeoData& geo, axRenderMesh& mesh) {
	using Vertex = axRenderVertex_PosUv2Color;

	mesh.clear();
	auto editMesh		= mesh.edit();
	auto srcPoints		= geo.readPoints();
	auto srcEdges		= geo.readEdges();

	axTempString text;
	axInt edgeId = 0;
	for (auto& edge : srcEdges) {
		text.format("{?}", edgeId);
		auto pos0 = srcPoints[edge.point0].pos;
		auto pos1 = srcPoints[edge.point1].pos;

		auto pos = (pos0 + pos1) / 2;
		editMesh.addTextBillboard(Vertex::s_desc(), text, axVec3f::s_cast(pos), nullptr);
		edgeId++;
	}
}

void GEO_GeoData_RenderMeshHelper::createPrimitiveIdsMesh(GEO_GeoData& geo, axRenderMesh& mesh) {
	using Vertex = axRenderVertex_PosUv2Color;

	mesh.clear();
	auto editMesh = mesh.edit();

	auto primCenterSpan = geo.getOrCreatePrimitiveCenters();

	axTempString text;
	axInt primIndex = 0;
	for (auto& pos : primCenterSpan) {
		text.format("{?}", primIndex);
		editMesh.addTextBillboard(Vertex::s_desc(), text, axVec3f::s_cast(pos), nullptr);
		primIndex++;
	}
}

void GEO_GeoData_RenderMeshHelper::createPointNormalsMesh(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength) {
	using Vertex = axRenderVertex_PosColor;

	mesh.clear();
	auto pointNormals = geo.readPointNormals();
	if (pointNormals.size() <= 0) return;

	auto outVertexCount = geo.pointCount() * 2;
	auto& sm = mesh.getSubMeshCanAddVertices(axRenderMesh::PrimType::Lines, Vertex::s_desc(), axRenderMesh::IndexType::Unknown, outVertexCount);

	sm.vertices.resize(outVertexCount);

	auto outVertices = sm.vertices.editPositions();

	auto dst = ax_foreach_begin(outVertices);

	axInt i = 0;
	for (auto& pt : geo.readPoints()) {
		auto pos = pt.pos;
		dst->setByCast(pos);
		dst++;
		dst->setByCast(pos + pointNormals[i] * normalLength);
		dst++;
		i++;
	}

	sm.vertices.editColorSet(0).fillValues(axColorRGBAh_make(Color::kGreen()));
}

void GEO_GeoData_RenderMeshHelper::createVertexNormalsMesh(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength) {
	using Vertex = axRenderVertex_PosColor;

	mesh.clear();
	auto vertexNormals = geo.readVertexNormals();
	if (vertexNormals.size() <= 0) return;

	auto outVertexCount = geo.vertexCount() * 2;
	auto& sm = mesh.getSubMeshCanAddVertices(axRenderMesh::PrimType::Lines, Vertex::s_desc(), axRenderMesh::IndexType::Unknown, outVertexCount);

	sm.vertices.resize(outVertexCount);

	auto outVertices = sm.vertices.editPositions();

	auto dst = ax_foreach_begin(outVertices);
	auto pts = geo.readPoints();

	axInt i = 0;
	for (auto& v : geo.readVertices()) {
		auto pos = pts[v.point].pos;
		dst->setByCast(pos);
		dst++;
		dst->setByCast(pos + vertexNormals[i] * normalLength);
		dst++;
		i++;
	}

	sm.vertices.editColorSet(0).fillValues(axColorRGBAh_make(Color::kGreen()));
}

void GEO_GeoData_RenderMeshHelper::createPrimitiveNormalsMesh(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength) {
	using Vertex = axRenderVertex_PosColor;

	mesh.clear();
	auto primNormals = geo.getOrCreatePrimitiveNormals();
	auto primCenters = geo.getOrCreatePrimitiveCenters();

	auto outVertexCount = geo.primitiveCount() * 2;
	auto& sm = mesh.getSubMeshCanAddVertices(axRenderMesh::PrimType::Lines, Vertex::s_desc(), axRenderMesh::IndexType::Unknown, outVertexCount);

	sm.vertices.resize(outVertexCount);

	auto outVertices = sm.vertices.editPositions();

	auto dst = ax_foreach_begin(outVertices);

	axInt i = 0;
	for (auto& pos : primCenters) {
		dst->setByCast(pos);
		dst++;

		dst->setByCast(pos + primNormals[i] * normalLength);
		dst++;
		i++;
	}

	i++;
	sm.vertices.editColorSet(0).fillValues(axColorRGBAh_make(Color::kGreen()));
}

void GEO_GeoData_RenderMeshHelper::createIdsMesh(GEO_GeoData& geo, axRenderMesh& mesh, axRenderComponentFlags flags) {
	using F = axRenderComponentFlags;
	switch (flags) {
		case F::Point:		return createPointIdsMesh		(geo, mesh);
		case F::Edge:		return createEdgeIdsMesh		(geo, mesh);
		case F::Vertex:		return createVertexIdsMesh		(geo, mesh, 0);
		case F::Primitive:	return createPrimitiveIdsMesh	(geo, mesh);
	}
}

void GEO_GeoData_RenderMeshHelper::createNormalsMesh(GEO_GeoData& geo, axRenderMesh& mesh, axRenderComponentFlags flags, float normalLength) {
	using F = axRenderComponentFlags;
	switch (flags) {
		case F::Point:		return createPointNormalsMesh		(geo, mesh, normalLength);
//		case F::Edge:		return createEdgeNormalsMesh		(geo, mesh, normalLength);
		case F::Vertex:		return createVertexNormalsMesh		(geo, mesh, normalLength);
		case F::Primitive:	return createPrimitiveNormalsMesh	(geo, mesh, normalLength);
	}
}

void GEO_GeoData_RenderMeshHelper::createGeoData(GEO_GeoData& geo, axEditableMesh& srcMesh) {
	geo.clear();

	auto srcPoints	= srcMesh.points();
	auto dstPoints = geo.editPoints(srcPoints.size());

	{
		auto dst = ax_foreach_begin(dstPoints);
		for (auto& src : srcPoints) {
			dst->pos = src.pos;
			dst++;
		}
	}

	{
		axArray<axEditableMesh::Point*, 64> facePoints;
		axArray<axInt, 64> pointIds;
		auto srcFaces	= srcMesh.faces();
		for (auto& src : srcFaces) {
			src.getPoints(srcMesh, facePoints);

			pointIds.resize(facePoints.size());
			auto dst = ax_foreach_begin(pointIds.span());
			for (auto& fp : facePoints) {
				dst = ax_enum_int(fp->id());
				dst++;
			}

			geo.addPrimitive(GEO_GeoData::PrimType::Polygon, pointIds);
		}
	}
}

}