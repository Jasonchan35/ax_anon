#pragma once

#if AX_USE_METAL

#include "../../shader/axShaderPass.h"
#include "axMTLRenderer.h"

class axMTLShaderPass : public axShaderPass {
	AX_RTTI_CLASS(axMTLShaderPass, axShaderPass)
public:
	using Util = axMTLUtil;

	axMTLShaderPass();
	~axMTLShaderPass();

	virtual void onInit	(axInt passIndex, const Info& info) override;
	virtual bool onValidate() final;
	
	void bind(axMTLCommandDispatcher& dispatcher, DrawCall& 	drawCall);
	void bind(axMTLCommandDispatcher& dispatcher, ComputeCall& 	computeCall);

	void _initParamsByReflection(const StageFlags& stageFlags, NSArray<MTLArgument *> * stageArguments);

	id<MTLLibrary>				_metallib = nil;
	id<MTLFunction>				_vsFunc = nil;
	id<MTLFunction>				_psFunc = nil;
	id<MTLFunction>				_csFunc = nil;
//	id<MTLRenderPipelineState>	_renderPipelineState = nil;
	id<MTLComputePipelineState>	_computePipelineState = nil;
	axVec3i _csWorkgroupSize {1};

	class VertexInputLayoutDesc {
	public:
		VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc, bool forReflection);
		MTLVertexDescriptor* desc = nil;
	};

//---- Pipeline State Object ----
	class PsoKey {
	public:
		VertexDesc vertexDesc = nullptr;
		PrimitiveType primitiveType = PrimitiveType::Unknown;
		MTLPixelFormat colorPixelFormat = MTLPixelFormatInvalid;
		MTLPixelFormat depthStencilPixelFormat = MTLPixelFormatInvalid;

		bool operator==(const PsoKey& r) const {
			return vertexDesc				== r.vertexDesc
				&& primitiveType			== r.primitiveType
				&& colorPixelFormat			== r.colorPixelFormat
				&& depthStencilPixelFormat 	== r.depthStencilPixelFormat;
		}
	};

	class Pso {
	public:
		id<MTLRenderPipelineState> mtlPso = nil;
	};

	void _createComputePipelineState();
	void _createPipelineState(Pso* outPso, const PsoKey& key, bool forReflection);
	axArrayMap<PsoKey, Pso, 4> _pipelineStateMap;

	id<MTLDepthStencilState> _mtlDepthStencilState = nil;

};

#endif //AX_USE_METAL
