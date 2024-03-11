#pragma once

#include "axRenderMesh.h"

// https://en.wikipedia.org/wiki/Doubly_connected_edge_list
// The doubly connected edge list (DCEL), also known as half-edge data structure

enum class axEditableMesh_PointId		: axInt { Invalid = -1 };
enum class axEditableMesh_EdgeId		: axInt { Invalid = -1 };
enum class axEditableMesh_FaceId		: axInt { Invalid = -1 };
enum class axEditableMesh_FaceVertexId	: axInt { Invalid = -1 };
enum class axEditableMesh_FaceEdgeId	: axInt { Invalid = -1 };

AX_ENUM_OPERATORS(axEditableMesh_PointId);
AX_ENUM_OPERATORS(axEditableMesh_EdgeId);
AX_ENUM_OPERATORS(axEditableMesh_FaceId);
AX_ENUM_OPERATORS(axEditableMesh_FaceVertexId);
AX_ENUM_OPERATORS(axEditableMesh_FaceEdgeId);

#define axEditableMesh_ElementType_EnumList(E) \
	E(Unknown,)    \
	E(Point,)      \
	E(Edge,)       \
	E(Face,)       \
	E(FaceVertex,) \
	E(FaceEdge,)   \
//---------
ax_ENUM_CLASS(axEditableMesh_ElementType, axUInt8)

class axEditableMeshEdit;

class axEditableMesh : public axNonCopyable {
public:
	using Mesh		= axEditableMesh;
	using PointId		= axEditableMesh_PointId;
	using EdgeId		= axEditableMesh_EdgeId;
	using FaceId		= axEditableMesh_FaceId;
	using FaceEdgeId	= axEditableMesh_FaceEdgeId;
	using ElementType	= axEditableMesh_ElementType;
	using Color			= axColor;
	using Vec2			= axVec2d;
	using Vec3			= axVec3d;
	using Vec4			= axVec4d;
	using Mat4			= axMat4d;


	struct Point;
	struct Edge;
	struct FaceEdge;

	axEditableMeshEdit edit();

	//-----------------------------------------------------------------
	struct Point : public axNonCopyable {
		Vec3		pos;
		Vec3		normal;
		PointId	id() const { return _id; }

	friend class axEditableMesh;
	protected:
		axInt		_faceCount = 0;
		PointId		_id = PointId::Invalid;
		EdgeId		_edgeHead = EdgeId::Invalid;
		EdgeId		_edgeTail = EdgeId::Invalid;
	};

	//-----------------------------------------------------------------
	struct Edge : public axNonCopyable {
		Vec3		normal;
		bool		softEdge : 1;

		Edge() : softEdge(false) {}
		EdgeId	id() const { return _id; }

		Point&	point0(Mesh& mesh);
		Point&	point1(Mesh& mesh);

		bool hasPoint	(PointId p0, PointId p1);
		void addFaceEdge(Mesh& mesh, FaceEdge& fe);
		Vec3 center		(Mesh& mesh);

		void addToPoint	(Mesh& mesh, Point& v0, Point& v1);
		void onFormat	(axFormat& f) const;

		EdgeId& next(PointId v); //!< next base on pointId

	friend class axEditableMesh;
	protected:
		EdgeId			_id		= EdgeId::Invalid;
		PointId			_point0	= PointId::Invalid;
		PointId			_point1	= PointId::Invalid;

		FaceEdgeId		_faceEdgeHead = FaceEdgeId::Invalid;  //faceEdge shared this edge
		FaceEdgeId		_faceEdgeTail = FaceEdgeId::Invalid;

	private:
		EdgeId			_next0		= EdgeId::Invalid;  //next edge for vertex 0
		EdgeId			_next1		= EdgeId::Invalid;  //next edge for vertex 1
	};

	//-----------------------------------------------------------------
	struct FaceEdge : public axNonCopyable {
		Point&		point	(Mesh& mesh);
		Vec3&		normal	(Mesh& mesh);
		Edge&		edge	(Mesh& mesh);
		FaceEdgeId	id		() const { return _id; }

	friend class axEditableMesh;
	protected:
		FaceEdgeId		_id			= FaceEdgeId::Invalid;
		PointId			_point		= PointId::Invalid;
		EdgeId			_edge		= EdgeId::Invalid;
		FaceId			_face		= FaceId::Invalid;
		FaceEdgeId		_next		= FaceEdgeId::Invalid;
		FaceEdgeId		_adjacent	= FaceEdgeId::Invalid; //FaceEdge shared by the same edge
	};

	//-----------------------------------------------------------------
	struct Face : public axNonCopyable {
		Vec3		normal;
		Vec3		center;
		axInt		pointCount()	{ return _faceEdgeCount; }
		FaceId		id() const		{ return _id; }

		void				getPoints	(Mesh& mesh, axIArray<Point*>& result);
		axMutSpan<Vec3>		getNormals	(Mesh& mesh);
		axMutSpan<Color>	getColors	(Mesh& mesh, axInt colorSetId);
		axMutSpan<Vec2>		getUvs		(Mesh& mesh, axInt uvSetId);
		axMutSpan<Vec4>		getCustoms	(Mesh& mesh, axInt customSetId);
		axMutSpan<FaceEdge>	getFaceEdges(Mesh& mesh);

	friend class axEditableMesh;
	protected:
		FaceId			_id   = FaceId::Invalid;
		FaceEdgeId		_faceEdgeHead = FaceEdgeId::Invalid;
		axInt			_faceEdgeCount;
	};

	//-----------------------------------------------------------------
	template<class T>
	struct FaceVertexSetBase : public axNonCopyable {
		T				defaultValue;
		axString		name;
		axArray<T>		values;
	};
	struct UvSet : public FaceVertexSetBase<Vec2> {
	};
	struct ColorSet : public FaceVertexSetBase<Color> {
	};
	struct CustomSet : public FaceVertexSetBase<Vec4> {
	};

	//-----------------------------------------------------------------
	struct DisplayOptions {
		bool		normals			= false;
		bool		componentIds	= false;
		bool		doubleSided		= false;
		ElementType	elementType		= ElementType::Unknown;
	};

	//-----------------------------------------------------------------
	void	clear();

	Face&		addFace(axIntSpan pointIndices);
	Point&		addPoint(const axVec3d& pos);
	Point&		addPoint(double x, double y, double z) { return addPoint(axVec3d(x,y,z)); }

	EdgeId		findEdge(PointId a, PointId b);

			Point&		point		(PointId    i)			{ return _points[ax_enum_int(i)]; }
	const	Point&		point		(PointId    i) const	{ return _points[ax_enum_int(i)]; }

			Edge&		edge		(EdgeId     i)			{ return _edges[ax_enum_int(i)];    }
	const	Edge&		edge		(EdgeId     i) const	{ return _edges[ax_enum_int(i)];    }

			Face&		face		(FaceId     i)			{ return _faces[ax_enum_int(i)]; }
	const	Face&		face		(FaceId     i) const	{ return _faces[ax_enum_int(i)]; }

			FaceEdge&	faceEdge	(FaceEdgeId i)			{ return _faceEdges[ax_enum_int(i)]; }
	const	FaceEdge&	faceEdge	(FaceEdgeId i) const	{ return _faceEdges[ax_enum_int(i)]; }

			Vec3&		fvNormal	(FaceEdgeId i)			{ return _fvNormals[ax_enum_int(i)]; }
	const	Vec3&		fvNormal	(FaceEdgeId i) const	{ return _fvNormals[ax_enum_int(i)]; }

	axMutSpan<Point>	points()			{ return _points; }
	   axSpan<Point>	points() const		{ return _points; }

	axMutSpan<Edge>		edges()				{ return _edges; }
	   axSpan<Edge>		edges() const		{ return _edges; }

	axMutSpan<Face>		faces()				{ return _faces; }
	   axSpan<Face>		faces() const		{ return _faces; }

	axMutSpan<FaceEdge>	faceEdges()			{ return _faceEdges; }
	   axSpan<FaceEdge>	faceEdges() const	{ return _faceEdges; }

	axMutSpan<Vec3>		fvNormals()			{ return _fvNormals; }
	   axSpan<Vec3>		fvNormals() const	{ return _fvNormals; }

	void updateFaceNormals();
	void updateFaceVertexNormals(double hardEdgeAngleDeg);

	axInt colorSetCount() const { return _fvColorSets.size(); }
	void addColorSet(const Color & defaultValue);

	axInt uvSetCount() const { return _fvUvSets.size(); }
	void addUvSet(const Vec2& defaultValue);

	axInt customSetCount() const { return _fvCustomSets.size(); }
	void addCustomSet(const Vec4& defaultValue);

	axEditableMesh();

private:
	axArray<Face>		_faces;
	axArray<Point>		_points;
	axArray<Edge>		_edges;
	axArray<FaceEdge>	_faceEdges;

	axArray<Vec3>		_fvNormals;
	axArray<ColorSet>	_fvColorSets;
	axArray<UvSet>		_fvUvSets;
	axArray<CustomSet>	_fvCustomSets;

	DisplayOptions		_displayOptions;

};

//===============
namespace axTypeTraits {
	template<> struct isTriviallyCopyable< axEditableMesh::Point    > { const static bool value = true; };
	template<> struct isTriviallyCopyable< axEditableMesh::Edge     > { const static bool value = true; };
	template<> struct isTriviallyCopyable< axEditableMesh::Face     > { const static bool value = true; };
	template<> struct isTriviallyCopyable< axEditableMesh::FaceEdge > { const static bool value = true; };
}

AX_INLINE
axMutSpan<axEditableMesh::Vec3> axEditableMesh::Face::getNormals(Mesh& mesh) {
	return mesh._fvNormals.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
axMutSpan<axEditableMesh::Color> axEditableMesh::Face::getColors(Mesh& mesh, axInt colorSetId) {
	return mesh._fvColorSets[colorSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
axMutSpan<axEditableMesh::Vec2> axEditableMesh::Face::getUvs(Mesh& mesh, axInt uvSetId) {
	return mesh._fvUvSets[uvSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
axMutSpan<axEditableMesh::Vec4> axEditableMesh::Face::getCustoms(Mesh& mesh, axInt customSetId) {
	return mesh._fvCustomSets[customSetId].values.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
axMutSpan<axEditableMesh::FaceEdge> axEditableMesh::Face::getFaceEdges(Mesh& mesh) {
	return mesh._faceEdges.slice(ax_enum_int(_faceEdgeHead), _faceEdgeCount);
}

AX_INLINE
axEditableMesh::Point& axEditableMesh::Edge::point0(Mesh& mesh) {
	return mesh.point(_point0);
}

AX_INLINE
axEditableMesh::Point& axEditableMesh::Edge::point1(Mesh& mesh) {
	return mesh.point(_point1);
}

AX_INLINE
bool axEditableMesh::Edge::hasPoint(PointId p0, PointId p1) {
	return (p0 == _point0 && p1 == _point1)
		|| (p1 == _point0 && p0 == _point1);
}

AX_INLINE
axEditableMesh::EdgeId& axEditableMesh::Edge::next(PointId p) {
	if (_point0 == p) return _next0;
	AX_ASSERT(_point1 == p);
	return _next1;
}

AX_INLINE
axEditableMesh::Point& axEditableMesh::FaceEdge::point(Mesh& mesh) {
	return mesh.point(_point);
}

AX_INLINE
axEditableMesh::Vec3& axEditableMesh::FaceEdge::normal(Mesh& mesh) {
	return mesh.fvNormal(_id);
}

AX_INLINE
axEditableMesh::Edge& axEditableMesh::FaceEdge::edge(Mesh& mesh) {
	return mesh.edge(_edge);
}
