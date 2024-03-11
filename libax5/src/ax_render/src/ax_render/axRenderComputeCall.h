#pragma once

#include "shader/axMaterial.h"
#include "buffer/axRenderMultiGpuBuffer.h"

class axRenderComputeCall : public axNonCopyable {
public:
	using MultiGpuBuffer		= axRenderMultiGpuBuffer;
	using Material			= axMaterial;

	axSPtr<Material>		material;
	axInt8					renderPass = 0;

	axVec3i		threadGroupCount {1,1,1};

//	axSPtr<MultiGpuBuffer>	indexBuffer;

	axSourceLoc	debugLoc;
};

