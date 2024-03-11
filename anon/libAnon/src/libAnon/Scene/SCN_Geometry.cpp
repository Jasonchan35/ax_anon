#include "SCN_Geometry.h"
#include "SCN_System.h"
#include "../Geometry/Core/GEO_GeoData_RenderMeshHelper.h"

namespace Anon {

SCN_Geometry::SCN_Geometry(SCN_Object* object)
	: Base(object)
	, _diagram(nullptr)
{
	_diagram.setOwner(this);
}

void SCN_Geometry::onRender(axRenderRequest& req) {
	compute();

	using ComponentFlags = axRenderComponentFlags;

	auto* scn = sceneObject();
	if (!scn) return;

	auto& localToWorld = scn->localToWorldMatrix();

	auto mv = req.world.modelview.push();
	mv *= localToWorld;

	auto* outputNode = _diagram.outputNode();
	if (!outputNode) return;

	GEO_GeoData geo = outputNode->output();
	if (!_renderMesh) {
		GEO_GeoData_RenderMeshHelper::createMesh(geo, *_renderMesh);
	}

	{
		if (req.debugWireframe) {
			auto mtl = req.renderer()->createMaterial();
			mtl->setShader(req.builtInAssets()->shaders.Wireframe);
			mtl->setParam(AnonNames::matMvp(),		req.world.mvp());
			mtl->setParam(AnonNames::color(),		axColor(.8f, .8f, .8f));
			req.addDrawCall(AX_LOC, mtl, *_renderMesh);
		} else {
			auto mtl = req.world.createMaterial_BlinnPhong();
			mtl->set_matObjectToWorld(localToWorld);
			mtl->set_ambient({0.02f, 0.03f, 0.04f});
//			mtl->set_ambient({0,0,0});
			mtl->set_diffuse({0.6f, 0.8f, 0.95f});
			mtl->set_specular({1,1,1});
			mtl->set_shininess(64.0f);
			req.addDrawCall(AX_LOC, mtl, *_renderMesh);
		}
	}

// ids
	if (req.drawComponentIds != ComponentFlags::None) {
		if (!_renderIds) {
			GEO_GeoData_RenderMeshHelper::createIdsMesh(geo, *_renderIds, req.drawComponentIds);
		}
		auto mtl = req.world.createMaterial_TextBillboardTexture0();
		auto wireframe = req.debugWireframe.push(false);
		req.addDrawCall(AX_LOC, mtl, *_renderIds);
	}

	if (req.drawComponentNormals != ComponentFlags::None) {
		if (!_renderNormals || _renderNormalLength != req.drawComponentNormalLength) {
			_renderNormalLength = req.drawComponentNormalLength;
			GEO_GeoData_RenderMeshHelper::createNormalsMesh(geo, *_renderNormals, req.drawComponentNormals, req.drawComponentNormalLength);
		}
		req.world.drawMesh(AX_LOC, *_renderNormals);
	}


	auto* renderer = req.renderer();
	auto& gpuInfo = renderer->adapterInfo();

	static auto testName = axNameId::s_make("Geometry");
	auto objName = sceneObject()->name();

	if (objName == testName &&  gpuInfo.computeShader) {
		//==== test render triangles from compute shader ===
		using Vertex = axRenderVertex_PosColor;

		axInt32x2 gridSize(4,4);
		axInt vertexCount = gridSize.x * gridSize.y * axInt(6);

		axRenderStorageBuffer_CreateDesc  desc;
		desc.bufferSize = vertexCount * ax_sizeof(Vertex);
		desc.byteStride = 4; //sizeof(Vertex);

		auto bufferOut	= req.renderer()->createStorageBuffer(desc);

		auto mtl = req.renderer()->createMaterial();
		mtl->setShader(req.builtInAssets()->computeShaders.ComputeTriangles);

		mtl->setParam(axNameId::s_make("cellSize"),		15.0f);
		mtl->setParam(axNameId::s_make("gridSize"),		gridSize);
		mtl->setParam(axNameId::s_make("BufferOut"),	bufferOut);

		req.addComputeCall(AX_LOC, mtl, axVec3i(gridSize.x, gridSize.y, 1));

		auto renderMtl = req.renderer()->createMaterial();
//		renderMtl->setShader(req.builtInAssets()->shaders.UnlitTexture0);
		renderMtl->setShader(req.builtInAssets()->shaders.Wireframe);

		renderMtl->setParam(AnonNames::matMvp(), req.world.mvp());
		renderMtl->setParam(axNameId::s_make("color"), Color(0,1,.5f));

		axRenderDrawCallInfo info;
		info.primitiveType	= axRenderPrimitiveType::Triangles;
		info.vertexDesc		= Vertex::s_desc();
		info.vertexBuffer	= bufferOut->gpuBuffer();
		info.vertexCount	= vertexCount;

		req.addDrawCall(AX_LOC, renderMtl, info);

		axRenderMesh mesh;
		mesh.edit().createCube(Vertex::s_desc(), {0,0,0}, {1.5f, 2, 1.5f}, {1,0,1});
		req.addDrawCall(AX_LOC, renderMtl, mesh);
	}
}

void SCN_Geometry::onCompute() {
	Base::onCompute();
	_diagram.compute();
}

void SCN_Geometry::onSetComputeNeeded() {
	Base::onSetComputeNeeded();
	_renderMesh.clear();
	_renderIds.clear();
	_renderNormals.clear();
}

} //namespace
