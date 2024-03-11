#include "GEO_GeoData.h"

namespace Anon {

void GEO_GeoData::clear() {
	pointAttrs.clear();
	edgeAttrs.clear();
	vertexAttrs.clear();
	primitiveAttrs.clear();
}

axSpan<Vec3d> GEO_GeoData::getOrCreateVertexNormals(bool softEdge) {
	if (auto s = readVertexNormals()) {
		return s;
	}

	if (copyPointAttrToVertex(AnonNames::normal())) {
		return readVertexNormals();
	}

	if (copyPrimitiveAttrToVertex(AnonNames::normal())) {
		return readVertexNormals();
	}

	getOrCreatePrimitiveNormals();
	copyPrimitiveAttrToVertex(AnonNames::normal());

	//if (softEdge) {
	//	if (!averageVertexAttrWithSamePoint(g_predefinedNames.normal)) {
	//		throw axError_Undefined(AX_LOC);
	//	}
	//}
	return readVertexNormals();
}

axSpan<Vec3d> GEO_GeoData::getOrCreatePrimitiveCenters() {
	auto outCenters = primitiveAttrs.editAttr<Vec3d>(AnonNames::center());

	auto pointSpan  = readPoints();
	auto vertexSpan = readVertices();

	axInt primIndex = 0;
	for (auto& prim : readPrimitives()) {
		Vec4d sum(0);
		auto faceVertexSpan = vertexSpan.slice(prim.vertexRange);
		for (auto& fv : faceVertexSpan) {
			sum += Vec4d(pointSpan[fv.point].pos, 1);
		}
		outCenters[primIndex] = sum.homogenize();
		primIndex++;
	}
	return outCenters;
}

axSpan<Vec3d> GEO_GeoData::getOrCreatePrimitiveNormals() {
	auto outNormals = primitiveAttrs.editAttr<Vec3d>(AnonNames::normal());

	auto pointSpan  = readPoints();
	auto vertexSpan = readVertices();

	auto dst = ax_foreach_begin(outNormals);
	for (auto& prim : readPrimitives()) {
		Vec4d sum(0);

		auto faceVertexSpan = vertexSpan.slice(prim.vertexRange);
		axInt n = faceVertexSpan.size();

		for (axInt i = 2; i < n; i++) {
			auto v0 = faceVertexSpan[0].point;
			auto v1 = faceVertexSpan[i].point;
			auto v2 = faceVertexSpan[i-1].point;

			auto p0 = pointSpan[v0].pos;
			auto p1 = pointSpan[v1].pos;
			auto p2 = pointSpan[v2].pos;

			auto nl = (p2 - p0).cross(p1 - p0).normal();
			sum += Vec4d(nl, 1);
		}

		dst = sum.homogenize();
		dst++;
	}
	return outNormals;
}

axInt GEO_GeoData::findEdge(axSpan<GEO_Point> pointSpan, axSpan<GEO_Edge> edgeSpan, axInt point0, axInt point1) const {
	auto& pt0 = pointSpan[point0];
	axInt edgeId = pt0.firstEdge;
	if (edgeId < 0) return -1;

	do {
		auto& e = edgeSpan[edgeId];
		if (e.hasPoints(point0, point1)) {
			return edgeId;
		}
		edgeId = e.nextEdge(point0);
	} while (edgeId != pt0.firstEdge);

	return -1;
}

axInt GEO_GeoData::prevPointEdgeId(axSpan<GEO_Edge> edgeSpan, axInt edgeId, axInt point) const {
	if (edgeId >= 0) {
		auto cur = edgeId;
		for (;;) {
			auto next = edgeSpan[cur].nextEdge(point);
			if (next == edgeId)
				return cur;
			cur = next;
		}
	}
	throw axError_Undefined(AX_LOC);
}

axInt GEO_GeoData::prevEdgeVertexId(axSpan<GEO_Vertex> vertexSpan, const GEO_Edge& edge, axInt vertexId) const {
	auto cur = edge.firstVertex;
	for (;;) {
		auto next = vertexSpan[cur].nextEdgeVertex;
		if (next == vertexId)
			return cur;
		cur = next;
	}
	throw axError_Undefined(AX_LOC);	
}

axInt GEO_GeoData::_addEdge(axMutSpan<GEO_Point>& pointSpan, axInt p0, axInt p1, axInt vertex) {
	auto edgeId = edgeCount();
	
	auto dstEdges = editEdges(edgeId + 1);
	auto& e = dstEdges.back();

	e.firstVertex = vertex;
	e.point0 = p0;
	e.point1 = p1;

	auto& pt0 = pointSpan[p0];
	auto& pt1 = pointSpan[p1];

	if (pt0.firstEdge < 0) {
		e.nextEdge0 = edgeId; // loop back to itself
		pt0.firstEdge = edgeId;
	} else {
		e.nextEdge0 = pt0.firstEdge;
		auto prevId = prevPointEdgeId(dstEdges, pt0.firstEdge, p0);
		dstEdges[prevId].setNextEdge(edgeId, p0);
	}

	if (pt1.firstEdge < 0) {
		e.nextEdge1 = edgeId; // loop back to itself
		pt1.firstEdge = edgeId;
	} else {
		e.nextEdge1 = pt1.firstEdge;
		auto prevId = prevPointEdgeId(dstEdges, pt1.firstEdge, p1);
		dstEdges[prevId].setNextEdge(edgeId, p1);
	}
	return edgeId;
}

void GEO_GeoData::addPrimitive(PrimType type, axIntSpan newPointIndices) {
	axIntRange vertexRange(vertexCount(), newPointIndices.size());
	auto primitiveId  = primitiveCount();

	auto pointSpan  = editPoints();
	auto edgeSpan	= editEdges();
	auto vertexSpan = editVertices(vertexRange.end());
	auto primSpan	= editPrimitives(primitiveId + 1);

	auto newVertices = vertexSpan.slice(vertexRange);

	{		
		auto newVertex = ax_foreach_begin(newVertices);
		auto n = newPointIndices.size();

		for (axInt i = 0 ; i < n; i++) {
			auto pi0 = newPointIndices[i];
			auto pi1 = newPointIndices[(i+1) % n];

			newVertex->primitive = primitiveId;
			newVertex->point = pi0;	

			auto vertexId = vertexRange.start + i;

			axInt edgeId = findEdge(pointSpan, edgeSpan, pi0, pi1);
			if (edgeId < 0) {
				edgeId		= _addEdge(pointSpan, pi0, pi1, vertexId);
				edgeSpan	= editEdges();
				auto& edge	= edgeSpan[edgeId];

				newVertex->nextEdgeVertex = vertexId;
				edge.firstVertex = vertexId;

			} else {
				auto& edge = edgeSpan[edgeId];
				auto prevVertexId = prevEdgeVertexId(vertexSpan, edge, edge.firstVertex);
				auto& prevVertex = vertexSpan[prevVertexId];

				newVertex->nextEdgeVertex = prevVertex.nextEdgeVertex;
				prevVertex.nextEdgeVertex = vertexId;
			}

			newVertex->edge = edgeId;
			newVertex++;
		}
	}

	auto& prim = primSpan.back();
	prim.type = type;
	prim.vertexRange = vertexRange;
}

void GEO_GeoData::combine(const GEO_GeoData& src) {
	axInt pointOffset		= pointCount();
	axInt vertexOffset		= vertexCount();
	axInt edgeOffset		= edgeCount();
	axInt primitiveOffset	= primitiveCount();

	{
		axInt totalCount = pointOffset + src.pointCount();
		auto dstSpan = editPoints(totalCount).sliceBack(src.pointCount());
		auto dst = ax_foreach_begin(dstSpan);

		for (auto& s : src.readPoints()) {
			dst->set(s, vertexOffset, edgeOffset);
			dst++;
		}

		src.pointAttrs.copyAttrsTo(pointAttrs, pointOffset);
	}

	{
		axInt totalCount = vertexCount() + src.vertexCount();
		auto dstSpan = editVertices(totalCount).sliceBack(src.vertexCount());
		auto dst = ax_foreach_begin(dstSpan);

		for (auto& s : src.readVertices()) {
			dst->set(s, pointOffset, primitiveOffset, edgeOffset);
			dst++;
		}

		src.vertexAttrs.copyAttrsTo(vertexAttrs, vertexOffset);
	}

	{
		axInt totalCount = edgeCount() + src.edgeCount();
		auto dstSpan = editEdges(totalCount).sliceBack(src.edgeCount());
		auto dst = ax_foreach_begin(dstSpan);

		for (auto& s : src.readEdges()) {
			dst->set(s, pointOffset, vertexOffset, edgeOffset);
			dst++;
		}

		src.edgeAttrs.copyAttrsTo(edgeAttrs, edgeOffset);
	}

	{
		axInt totalCount = primitiveCount() + src.primitiveCount();
		auto dstSpan = editPrimitives(totalCount).sliceBack(src.primitiveCount());
		auto dst = ax_foreach_begin(dstSpan);

		for (auto& s : src.readPrimitives()) {
			dst->set(s, vertexOffset);
			dst++;
		}

		src.primitiveAttrs.copyAttrsTo(primitiveAttrs, primitiveOffset);
	}
}

void GEO_GeoData::onGetObjectInfo(axIString& info) {
	pointAttrs.onGetObjectInfo(info);
	edgeAttrs.onGetObjectInfo(info);
	vertexAttrs.onGetObjectInfo(info);
	primitiveAttrs.onGetObjectInfo(info);
}

} //namespace