#pragma once

#if AX_USE_METAL

//
//  axMTLRenderContext_Base.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "../../axRenderContext.h"

class axMTLError : public axError {};
class axMTLError_Undefined : public axMTLError {};
class axMTLRenderContext;
class axMTLRenderer;
class axMTLShader;
class axMTLShaderPass;
class axMTLMaterial;
class axMTLRenderVertexBuffer;
class axMTLRenderIndexBuffer;

class axMTLCommandDispatcher;
class axMTLRenderContext_Base;

class axMTLRenderContext_Base : public axRenderContext {
	AX_RTTI_CLASS(axMTLRenderContext_Base, axRenderContext)
public:
	using AttrId = axRenderVertexAttrId;
	using Util = axMTLUtil;

	axMTLRenderContext_Base(axMTLRenderer* renderer, CreateDesc& desc);

	axMTLRenderer* renderer() { return static_cast<axMTLRenderer*>(Base::renderer()); }

	MTKView* mtkView() { return _mtkView; }

friend class axMTLCommandDispatcher;
protected:
			void _onCreate(MTKView* mtkView);

	virtual void onDispatchCommands(axRenderRequest& req) override;
	
			void _dispatchCommand(axRenderCommand* cmd_);

			void _setDrawableSize(const axVec2f& size);

	virtual void onPreDestroy() override;

	virtual void onSetNativeViewRect(const axRect2f& rect) final;
	virtual void onSetNeedToRender() override;

	axNSObj<MTKView>				_mtkView;
	id<MTLCommandQueue>				_cmdQueue = nil;
	id<MTLCommandBuffer>			_cmdBuffer = nil;
	id<MTLRenderCommandEncoder> 	_cmdEncoder = nil;

	id<MTLCommandBuffer>			_computeCmdBuffer = nil;
	id<MTLComputeCommandEncoder>	_computeCmdEncoder = nil;

	id<MTLCommandBuffer>			_blitCmdBuffer = nil;
	id<MTLBlitCommandEncoder>		_blitCmdEncoder = nil;
};

#endif //AX_USE_METAL
