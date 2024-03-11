#pragma once

#include "GEO_GeoData.h"

namespace Anon {

class GEO_GeoData_RenderMeshHelper {
public:

	static void createMesh					(GEO_GeoData& geo, axRenderMesh& mesh);

	static void createPointIdsMesh			(GEO_GeoData& geo, axRenderMesh& mesh);
	static void createVertexIdsMesh			(GEO_GeoData& geo, axRenderMesh& mesh, double distanceToPoint);
	static void createEdgeIdsMesh			(GEO_GeoData& geo, axRenderMesh& mesh);
	static void createPrimitiveIdsMesh		(GEO_GeoData& geo, axRenderMesh& mesh);

	static void createPointNormalsMesh		(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength);
	static void createVertexNormalsMesh		(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength);
	static void createPrimitiveNormalsMesh	(GEO_GeoData& geo, axRenderMesh& mesh, double normalLength);

	static void createIdsMesh				(GEO_GeoData& geo, axRenderMesh& mesh, axRenderComponentFlags flags);
	static void createNormalsMesh			(GEO_GeoData& geo, axRenderMesh& mesh, axRenderComponentFlags flags, float normalLength);

	static void createGeoData				(GEO_GeoData& geo, axEditableMesh& srcMesh);
};

}