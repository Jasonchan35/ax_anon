#pragma once

#include "GEO_GeoAttr.h"

namespace Anon {

class GEO_GeoData : public axTypedBase {
	ax_CLASS_BEGIN(GEO_GeoData, axTypedBase);
	static constexpr axTypeFlags flags() {
		return axTypeFlags::Copyable;
	}
	ax_CLASS_END()

public:
	using PrimType = GEO_PrimitiveType;

	void onJsonReader(axJsonReader& rd) {}
	void onJsonWriter(axJsonWriter& wr) const {}

	void addPrimitive(PrimType type, axIntSpan newPointIndices);

	void clear();

	void setToDefaultValue() { clear(); }

	axInt pointCount		() const { return pointAttrs.size(); }
	axInt edgeCount			() const { return edgeAttrs.size(); }
	axInt vertexCount		() const { return vertexAttrs.size(); }
	axInt primitiveCount	() const { return primitiveAttrs.size(); }

	   axSpan<GEO_Point>		readPoints() const				{ return pointAttrs.readAttr<GEO_Point>(AnonNames::point()); }
	axMutSpan<GEO_Point>		editPoints()					{ return pointAttrs.editAttr<GEO_Point>(AnonNames::point()); }
	axMutSpan<GEO_Point>		editPoints(axInt size)			{ return pointAttrs.editAttr<GEO_Point>(AnonNames::point(), size); }

	   axSpan<GEO_Edge>			readEdges() const				{ return edgeAttrs.readAttr<GEO_Edge>(AnonNames::edge()); }
	axMutSpan<GEO_Edge>			editEdges()						{ return edgeAttrs.editAttr<GEO_Edge>(AnonNames::edge()); }
	axMutSpan<GEO_Edge>			editEdges(axInt size)			{ return edgeAttrs.editAttr<GEO_Edge>(AnonNames::edge(), size); }

	   axSpan<Vec3d>			readPointNormals() const		{ return pointAttrs.readAttr<Vec3d>(AnonNames::normal()); }
	axMutSpan<Vec3d>			editPointNormals()				{ return pointAttrs.editAttr<Vec3d>(AnonNames::normal()); }

	   axSpan<GEO_Vertex>		readVertices() const			{ return vertexAttrs.readAttr<GEO_Vertex>(AnonNames::vertex()); }
	axMutSpan<GEO_Vertex>		editVertices()					{ return vertexAttrs.editAttr<GEO_Vertex>(AnonNames::vertex()); }
	axMutSpan<GEO_Vertex>		editVertices(axInt size)		{ return vertexAttrs.editAttr<GEO_Vertex>(AnonNames::vertex(), size); }

	   axSpan<Vec3d>			readVertexNormals() const		{ return vertexAttrs.readAttr<Vec3d>(AnonNames::normal()); }
	axMutSpan<Vec3d>			editVertexNormals()				{ return vertexAttrs.editAttr<Vec3d>(AnonNames::normal()); }

	   axSpan<GEO_Primitive>	readPrimitives() const			{ return primitiveAttrs.readAttr<GEO_Primitive>(AnonNames::primitive()); }
	axMutSpan<GEO_Primitive>	editPrimitives()				{ return primitiveAttrs.editAttr<GEO_Primitive>(AnonNames::primitive()); }
	axMutSpan<GEO_Primitive>	editPrimitives(axInt size)		{ return primitiveAttrs.editAttr<GEO_Primitive>(AnonNames::primitive(), size); }

	   axSpan<Vec3d>			readPrimitiveNormals() const	{ return primitiveAttrs.readAttr<Vec3d>(AnonNames::normal()); }
	axMutSpan<Vec3d>			editPrimitiveNormals()			{ return primitiveAttrs.editAttr<Vec3d>(AnonNames::normal()); }

	bool copyPointAttrToVertex			(axNameId name) { return pointAttrs.copyAttrToVertex		(name, *this); }
	bool copyPointAttrToPrimitve		(axNameId name) { return pointAttrs.copyAttrToPrimitive		(name, *this); }

	bool copyVertexAttrToPoint			(axNameId name) { return vertexAttrs.copyAttrToPoint		(name, *this); }
	bool copyVertexAttrToPrimitive		(axNameId name) { return vertexAttrs.copyAttrToPrimitive	(name, *this); }

	bool copyPrimitiveAttrToPoint		(axNameId name)	{ return primitiveAttrs.copyAttrToPoint		(name, *this); }
	bool copyPrimitiveAttrToVertex		(axNameId name)	{ return primitiveAttrs.copyAttrToVertex	(name, *this); }

	axSpan<Vec3d>	getOrCreatePrimitiveCenters();
	axSpan<Vec3d>	getOrCreatePrimitiveNormals();
	axSpan<Vec3d>	getOrCreateVertexNormals(bool softEdge);

	void combine(const GEO_GeoData& src);

	GEO_PointComponent		pointAttrs;
	GEO_VertexComponent		vertexAttrs;
	GEO_PrimitiveComponent	primitiveAttrs;
	GEO_EdgeComponent		edgeAttrs;

	axInt	findEdge(axSpan<GEO_Point> pointSpan, axSpan<GEO_Edge> edgeSpan, axInt point0, axInt point1) const;

	axInt	prevPointEdgeId(axSpan<GEO_Edge> edgeSpan, axInt edgeId, axInt point) const;
	axInt	prevEdgeVertexId(axSpan<GEO_Vertex> vertexSpan, const GEO_Edge& edge, axInt vertexId) const;

	void onGetObjectInfo(axIString& info);

private:
	axInt	_addEdge(axMutSpan<GEO_Point>& pointSpan, axInt point0, axInt point1, axInt vertex);
};

template<class T> inline
void GEO_AttrArray<T>::onCopyTo(GEO_Component& target, axIntRange range, axInt offset) const {
	auto dstSpan = target.editAttr<T>(name());
	auto srcSpan = _data.slice(range);
	dstSpan.copyValues(srcSpan, offset);
}

template<class T> inline
void GEO_AttrArray<T>::onCopyPointAttrToVertex(axNameId name, GEO_GeoData& geo) const {
	auto vertices = geo.readVertices();
	auto dstSpan = geo.vertexAttrs.editAttr<T>(name);
	axInt vi = 0;
	for (auto& dst : dstSpan) {
		dst = _data[vertices[vi].point];
		vi++;
	}
}

template<class T> inline
void GEO_AttrArray<T>::onCopyPointAttrToPrimitive(axNameId name, GEO_GeoData& geo) const {
	//TODO
}

template<class T> inline
void GEO_AttrArray<T>::onCopyVertexAttrToPoint(axNameId name, GEO_GeoData& geo) const {
	//TODO
}

template<class T> inline
void GEO_AttrArray<T>::onCopyVertexAttrToPrimitive(axNameId name, GEO_GeoData& geo) const {
	//TODO
}

template<class T> inline
void GEO_AttrArray<T>::onCopyPrimitiveAttrToPoint(axNameId name, GEO_GeoData& geo) const {
	//TODO
}

template<class T> inline
void GEO_AttrArray<T>::onCopyPrimitiveAttrToVertex(axNameId name, GEO_GeoData& geo) const {
	auto primitives = geo.readPrimitives();

	auto vertexAttrSpan = geo.vertexAttrs.editAttr<T>(name);
	axInt primIndex = 0;
	for (auto prim : primitives) {
		auto primVertexAttrSpan = vertexAttrSpan.slice(prim.vertexRange);
		auto& value = _data[primIndex];
		for (auto& dst : primVertexAttrSpan) {
			dst = value;
		}
		primIndex++;
	}
}

template<class FUNC> // bool (*FUNC)(axInt id, const GEO_Edge& e)
AX_INLINE void GEO_Point::foreachEdges(axSpan<GEO_Edge> edges, axInt pointId, FUNC func) const {
	auto ei = firstEdge;
	if (ei < 0) return;
	do {
		auto& edge = edges[ei];
		if (!func(ei, edge)) return;
		ei = edge.nextEdge(pointId);
	} while(ei != firstEdge);
}


} //namespace