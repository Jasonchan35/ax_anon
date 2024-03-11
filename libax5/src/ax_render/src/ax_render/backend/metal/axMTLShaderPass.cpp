#if AX_USE_METAL

#include "axMTLShader.h"
#include "axMTLShaderPass.h"
#include "axMTLRenderContext.h"
#include "axMTLRenderer.h"
#include "axMTLCommonBase.h"
#include "axMTLCommandDispatcher.h"

axMTLShaderPass::axMTLShaderPass() {
}

axMTLShaderPass::~axMTLShaderPass() {
	ax_Metal_TODO
}

void axMTLShaderPass::onInit(axInt passIndex, const Info& info) {
	auto shaderFilename = shader()->filename();
	auto mtlDevice = Util::mtlDevice();

	{	// load metal library
		axTempString tmp;
	#if AX_OS_OSX
		auto profile = StageProfile::MSL_Lib_Mac;
	#elif AX_OS_IOS
		auto profile = StagePRofile::MSL_Lib_iOS;
	#else
		#error "This platform doesn't support metal"
	#endif
		NSError* error = nullptr;

		tmp.format("{?}/MTL_pass{?}.{?}.metallib", shaderFilename, passIndex, profile);
		_metallib = [mtlDevice newLibraryWithFile:ax_toNSString(tmp) error:&error];
		if (error) {
			AX_LOG("error: {?}", error);
			throw axError_Undefined(AX_LOC);
		}

		if (info.vsFunc) {
			_vsFunc = [_metallib newFunctionWithName:ax_toNSString(info.vsFunc)];
			if (!_vsFunc) throw axError_Undefined(AX_LOC);
			
		}

		if (info.psFunc) {
			_psFunc = [_metallib newFunctionWithName:ax_toNSString(info.psFunc)];
			if (!_psFunc) throw axError_Undefined(AX_LOC);
		}

		if (info.csFunc) {
			_csFunc = [_metallib newFunctionWithName:ax_toNSString(info.csFunc)];
			if (!_csFunc) throw axError_Undefined(AX_LOC);
		}
	}

	for (MTLVertexAttribute* va in _vsFunc.vertexAttributes) {
		if (!va.active) continue;
		auto attrId   = axShaderVertexAttrIdHelper::fromVarName(axStrView_make(va.name));
		auto dataType = Util::getAxDataType(va.attributeType);
		auto bindPoint = ax_safe_static_cast<axInt>(va.attributeIndex);
		_addVertexAttr(attrId, dataType, bindPoint);
	}

	{
		MTLDepthStencilDescriptor *desc = [MTLDepthStencilDescriptor new];
		auto& src = _staticRenderState.depthTest;

		desc.depthCompareFunction = Util::getMTLDepthTestOp(src.op);
		desc.depthWriteEnabled = src.writeMask;
		_mtlDepthStencilState = [mtlDevice newDepthStencilStateWithDescriptor:desc];

	}

	if (isCompute()) {
		_createComputePipelineState();

	} else {
		PsoKey key;
		key.vertexDesc = nullptr;
		key.primitiveType = axRenderPrimitiveType::Triangles;
		key.colorPixelFormat = MTLPixelFormatRGBA8Unorm;

		_createPipelineState(nullptr, key, true);
	}
}

bool axMTLShaderPass::onValidate() {
	return true;
}

axMTLShaderPass::VertexInputLayoutDesc::VertexInputLayoutDesc(const VertexAttrs& requiredVertexAttrs, VertexDesc inputVertexDesc, bool forReflection) {
	auto strideBytes = 0;
	if (!forReflection) {
		if (!inputVertexDesc) throw axError_Undefined(AX_LOC);
		strideBytes = inputVertexDesc->strideBytes;
	}

	desc = [MTLVertexDescriptor vertexDescriptor];

	for (auto& reqAttr : requiredVertexAttrs) {
		auto attrId = reqAttr.attrId;
		auto bindPoint = Util::castNSUInt(reqAttr.bindPoint);
		MTLVertexAttributeDescriptor* dst = desc.attributes[bindPoint];
		dst.bufferIndex = ax_enum_int(Util::BufferIndexUsage::VertexAttr);

		if (forReflection) { // create dummy for reflection base on shader vertex attr
			dst.format = Util::getMTLVertexFormat(reqAttr.dataType);
			dst.offset = Util::castNSUInt(strideBytes);
			strideBytes += axRenderDataTypeUtil::sizeInBytes(reqAttr.dataType);

		} else {
			auto& attr = inputVertexDesc->attr(attrId);
			axShaderPass::validateVertexAttr(reqAttr, attr);

			dst.format = Util::getMTLVertexFormat(attr.dataType);
			dst.offset = Util::castNSUInt(attr.offset);
		}
	}

	{ // vertex data
		if (strideBytes % 4) {
			throw axError_Undefined(AX_LOC);
		}
		MTLVertexBufferLayoutDescriptor* layout = desc.layouts[ax_enum_int(Util::BufferIndexUsage::VertexAttr)];
		layout.stride = Util::castNSUInt(strideBytes);
		layout.stepFunction = MTLVertexStepFunctionPerVertex;
		layout.stepRate = 1;
	}

}

void axMTLShaderPass::_createPipelineState(Pso* outPso, const PsoKey& key, bool forReflection) {
	if (!forReflection) {
		checkAcceptVertexDesc(key.vertexDesc);
	}

	auto mtlDevice = Util::mtlDevice();
	VertexInputLayoutDesc vertexInputLayout(_vertexAttrs, key.vertexDesc, forReflection);

	MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
	desc.label = @"axMTLShaderPass";
	desc.vertexFunction   = _vsFunc;
	desc.vertexDescriptor = vertexInputLayout.desc;
	desc.fragmentFunction = _psFunc;
	{
		MTLRenderPipelineColorAttachmentDescriptor* rt = desc.colorAttachments[0];
		rt.pixelFormat = key.colorPixelFormat;
//		rt.writeMask =

		auto& blend = _staticRenderState.blend;
		if (blend.isEnable()) {
			rt.blendingEnabled = true;
			rt.rgbBlendOperation			= Util::getMTLBlendOp(blend.rgb.op);
			rt.alphaBlendOperation			= Util::getMTLBlendOp(blend.alpha.op);
			rt.sourceRGBBlendFactor			= Util::getMTLBlendFactor(blend.rgb.srcFactor);
			rt.destinationRGBBlendFactor 	= Util::getMTLBlendFactor(blend.rgb.dstFactor);
			rt.sourceAlphaBlendFactor		= Util::getMTLBlendFactor(blend.alpha.srcFactor);
			rt.destinationAlphaBlendFactor 	= Util::getMTLBlendFactor(blend.alpha.dstFactor);
		} else {
			rt.blendingEnabled = false;
		}
		
		desc.depthAttachmentPixelFormat = key.depthStencilPixelFormat;
		//desc.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
	}

	desc.inputPrimitiveTopology = Util::getMTLPrimitiveTopologyClass(key.primitiveType);

	MTLRenderPipelineReflection* reflection = nil;

	NSError* error = nil;
	id<MTLRenderPipelineState> mtlPso = nil;
	mtlPso = [mtlDevice	newRenderPipelineStateWithDescriptor: desc
						options: MTLPipelineOptionArgumentInfo | MTLPipelineOptionBufferTypeInfo
						reflection: forReflection ? &reflection : nullptr
						error: &error];
	if (!mtlPso) {
		AX_LOG("error: {?}", error);
		throw axError_Undefined(AX_LOC);
	}

	if (forReflection) {
		_initParamsByReflection(StageFlags::Vertex, reflection.vertexArguments);
		_initParamsByReflection(StageFlags::Pixel,  reflection.fragmentArguments);
	}

	if (outPso) {
		outPso->mtlPso = mtlPso;
	}
}

void axMTLShaderPass::_createComputePipelineState() {
	auto mtlDevice = Util::mtlDevice();

	MTLComputePipelineDescriptor* desc = [MTLComputePipelineDescriptor new];
	desc.label = @"axMTLShaderPass - CS";
	desc.computeFunction   = _csFunc;

	MTLComputePipelineReflection* reflection = nil;

	NSError* error = nil;
	_computePipelineState = [mtlDevice 	newComputePipelineStateWithDescriptor: desc
										options: MTLPipelineOptionArgumentInfo | MTLPipelineOptionBufferTypeInfo
										reflection: &reflection
										error: &error];
	if (!_computePipelineState) {
		AX_LOG("error: {?}", error);
		throw axError_Undefined(AX_LOC);
	}

	_initParamsByReflection(StageFlags::Compute, reflection.arguments);

	{
		auto csSpirvFilename = axTempString::s_format("{?}/SPIRV_pass{?}.{?}.bin.json", shader()->filename(), passIndex(), StageProfile::SPIRV_CS);
		axShaderStageInfo csInfo;
		csInfo.loadFile(csSpirvFilename);
		_csWorkgroupSize = csInfo.csWorkgroupSize;
	}
}

void axMTLShaderPass::_initParamsByReflection(const StageFlags& stageFlags, NSArray<MTLArgument *> * stageArguments) {
	for (MTLArgument* arg in stageArguments) {
		if (!arg.active) continue;

		auto bindPoint = ax_safe_static_cast<axInt>(arg.index);

		switch (arg.type) {
			case MTLArgumentTypeBuffer: {
				auto dataSize  = ax_safe_static_cast<axInt>(arg.bufferDataSize);
				auto name = axNameId::s_make(axStrView_make(arg.name));

				if (arg.bufferDataType != MTLDataTypeStruct) {
					throw axError_Undefined(AX_LOC); // only support struct at the moment
				}

				auto* members = arg.bufferStructType.members;
				if ([members count] <= 0) continue; // skip vertex buffer

				{
					// Storage Buffer
					auto m = [members objectAtIndex:0];
					if (axStrView_make(m.name) == "_data" && m.dataType == MTLDataTypeArray) {

						auto sboName = axStrView_make(arg.name);
						if (sboName.endsWith("_1")) { // don't know why spirv-cross generate "_1" for Raw sbo
							sboName = sboName.sliceLess(2);
						}

						_addStorageBufferParam(stageFlags, axNameId::s_make(sboName), bindPoint, false);
						continue;
					}
				}

				// Uniform Buffer
				auto& uniformBuffer = _addUniformBuffer(stageFlags, name, dataSize, bindPoint, 0);
				for (MTLStructMember* member in arg.bufferStructType.members) {
					auto memberName = axNameId::s_make(axStrView_make(member.name));
					_addUniformParam(stageFlags, memberName, Util::getAxDataType(member.dataType), uniformBuffer.index, ax_safe_static_cast<axInt>(member.offset));
				}
			}break;

			case MTLArgumentTypeTexture: {
				auto name = axNameId::s_make(axStrView_make(arg.name));
				_addTextureParam(stageFlags, name, Util::getAxDataType(arg.textureType), bindPoint);
			}break;

			case MTLArgumentTypeSampler: {
				auto name = axNameId::s_make(axStrView_make(arg.name));
				_addSamplerState(stageFlags, name, bindPoint);
			}break;

			default: throw axError_Undefined(AX_LOC);
		}
	}
}

void axMTLShaderPass::bind(axMTLCommandDispatcher& dispatcher, DrawCall& drawCall) {
	PsoKey key;
	key.vertexDesc = drawCall.vertexDesc;
	key.primitiveType = drawCall.primitiveType;

	auto* mtkView = dispatcher.renderContext->mtkView();
	key.colorPixelFormat = mtkView.colorPixelFormat;
	key.depthStencilPixelFormat = mtkView.depthStencilPixelFormat;

	auto* p = _pipelineStateMap.find(key);
	if (!p) {
		p = &_pipelineStateMap.add(key);
		_createPipelineState(p, key, false);
	}

	if (!p->mtlPso) throw axError_Undefined(AX_LOC);

	[dispatcher.cmdEncoder setRenderPipelineState:p->mtlPso];
	[dispatcher.cmdEncoder setDepthStencilState:_mtlDepthStencilState];
	[dispatcher.cmdEncoder setFrontFacingWinding: MTLWindingCounterClockwise];
	[dispatcher.cmdEncoder setCullMode: Util::getMTLCullMode(_staticRenderState.cull)];
	[dispatcher.cmdEncoder setTriangleFillMode: _staticRenderState.wireframe || drawCall.debugWireframe ? MTLTriangleFillModeLines : MTLTriangleFillModeFill];
}

void axMTLShaderPass::bind(axMTLCommandDispatcher& dispatcher, ComputeCall& computeCall) {
	[dispatcher.computeCmdEncoder setComputePipelineState:_computePipelineState];
}

#endif //AX_USE_METAL
