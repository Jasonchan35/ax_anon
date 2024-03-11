#pragma once

#include "shader/axMaterial.h"
#include "buffer/axRenderMultiGpuBuffer.h"

class axRenderDrawCall : public axNonCopyable {
public:
	using GpuBuffer			= axRenderGpuBuffer;
	using VertexDesc		= axRenderVertexDesc;
	using PrimitiveType		= axRenderPrimitiveType;
	using IndexType			= axRenderIndexType;
	using Material			= axMaterial;

	axRect2f				scissorWorldRect;

	axSPtr<Material>		material;
	axInt8					renderPass = 0;

	PrimitiveType			primitiveType = PrimitiveType::Unknown;

	VertexDesc				vertexDesc = nullptr;
	axSPtr<GpuBuffer>		vertexBuffer;

	AX_INLINE bool isIndexEnabled() const { return indexType != IndexType::Unknown; }

	IndexType				indexType  = IndexType::Unknown;
	axSPtr<GpuBuffer>		indexBuffer;

	axSourceLoc	debugLoc;

	bool	debugWireframe		= false;

	axInt	indexByteOffset		= 0;
	axInt	vertexByteOffset	= 0;

	axInt	indexCount			= 0;
	axInt	vertexCount			= 0;
};
