#include "GEO_Subdiv.h"

namespace Anon {

GEO_Subdiv::GEO_Subdiv(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}

void GEO_Subdiv_updatePointPos(GEO_GeoData& dst, const GEO_GeoData& src) {
	AX_ZoneScoped;

	auto srcPoints		= src.readPoints();
	auto srcEdges		= src.readEdges();
	auto srcVertices	= src.readVertices();
	auto srcPrimitives	= src.readPrimitives();

	auto newPrimPointStart = src.pointCount();
	auto newEdgePointStart = src.pointCount() + src.primitiveCount();

	auto dstPointCount	= src.pointCount() + src.primitiveCount() + src.edgeCount();
	auto dstPoints		= dst.editPoints(dstPointCount);

	// update point at primitive center
	{
		axInt primIndex = 0;
		for (auto& prim : srcPrimitives) {
			Vec4d sum(0);
			auto faceVertexSpan = srcVertices.slice(prim.vertexRange);
			for (auto& fv : faceVertexSpan) {
				sum += Vec4d(srcPoints[fv.point].pos, 1);
			}
			dstPoints[newPrimPointStart + primIndex].pos = sum.homogenize();
			primIndex++;
		}
	}

	// update point at edge center
	{
		axInt edgeIndex = 0;
		for (auto& edge : srcEdges) {
			auto& pt0 = srcPoints[edge.point0];
			auto& pt1 = srcPoints[edge.point1];

			Vec4d pos(pt0.pos + pt1.pos, 2);
			edge.foreachVertex(srcVertices, [&](auto vi, auto& v) -> bool {
				pos += Vec4d(dstPoints[newPrimPointStart + v.primitive].pos, 1);
				return true;
			});

			dstPoints[newEdgePointStart + edgeIndex].pos = pos.homogenize();
			edgeIndex++;
		}
	}

	// update points
	{
		for (axInt pointId = 0; pointId < srcPoints.size(); pointId++) {
			auto& pt = srcPoints[pointId];

			double n = 0;
			Vec4d sumEdge(0);
			Vec4d sumFace(0);

			pt.foreachEdges(srcEdges, pointId, [&](axInt ei, const GEO_Edge& edge) -> bool {
				sumEdge += Vec4d(srcPoints[edge.point0].pos, 1);
				sumEdge += Vec4d(srcPoints[edge.point1].pos, 1);
				n++;

				edge.foreachVertex(srcVertices, [&](axInt vi, const GEO_Vertex& v) -> bool {
					if (v.point == pointId) {
						sumFace += Vec4d(dstPoints[newPrimPointStart + v.primitive].pos, 1);
						return false;
					}
					vi = v.nextEdgeVertex;
					return true;
				});
				return true;
			});

			dstPoints[pointId].pos	= pt.pos * ((n-3)/n)
									+ sumFace.homogenize() / n 
									+ sumEdge.homogenize() * 2 / n;
		}
	}
}

void GEO_Subdiv_create(GEO_GeoData& dst, const GEO_GeoData& src, axInt level) {
	AX_ZoneScoped;
	#if TRACY_ENABLE
	auto text = axTempString::s_format("Lv={?}", level);
	ZoneText(text.c_str(), text.size());
	#endif

	auto srcPoints		= src.readPoints();
	auto srcEdges		= src.readEdges();
	auto srcVertices	= src.readVertices();
	auto srcPrimitives	= src.readPrimitives();

	if (srcPrimitives.size() <= 0) return;

	auto dstPointCount = srcPoints.size() + srcPrimitives.size() + srcEdges.size();
	dst.pointAttrs.resize(dstPointCount);

	auto newPrimPointStart = srcPoints.size();
	auto newEdgePointStart = srcPoints.size() + srcPrimitives.size();

	axInt primIndex = 0;
	for (auto& srcPrim : srcPrimitives) {

		auto faceVertices = srcVertices.slice(srcPrim.vertexRange);
		auto fvCount = faceVertices.size();

		axInt	pt[4];
		for (axInt fv = 0; fv < fvCount; fv++) {
			auto& p0 = faceVertices[fv].point;
			auto& p1 = faceVertices[(fv + 1) % fvCount].point; // next point
			auto& p2 = faceVertices[(fvCount + fv - 1) % fvCount].point; // prev point

			auto e1 = src.findEdge(srcPoints, srcEdges, p0, p1);
			auto e2 = src.findEdge(srcPoints, srcEdges, p0, p2);

			pt[0] = p0;
			pt[1] = newEdgePointStart + e1; // next edge
			pt[2] = newPrimPointStart + primIndex; // point at primitive center
			pt[3] = newEdgePointStart + e2; // prev edge
			dst.addPrimitive(GEO_PrimitiveType::Polygon, pt);
		}
		primIndex++;
	}

	GEO_Subdiv_updatePointPos(dst, src);
}

void GEO_Subdiv::onCompute() {
	Base::onCompute();
	if (bypass()) return;
	if (_level <= 0) return;

	AX_ZoneScoped;

	_level = ax_clamp<axInt>(_level, 1, 6);

	axArray<GEO_GeoData> tmp;

	tmp.resize(_level + 1);
	tmp[0] = input();

	for (axInt i = 1; i <= _level; i++) {
		GEO_Subdiv_create(tmp[i], tmp[i-1], i);
	}

	_output = tmp.back();
}

}