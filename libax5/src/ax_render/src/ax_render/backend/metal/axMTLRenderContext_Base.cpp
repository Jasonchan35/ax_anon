#if AX_USE_METAL

#include "axMTLRenderer.h"
#include "axMTLRenderContext_Base.h"
#include "axMTLCommandDispatcher.h"

axMTLRenderContext_Base::axMTLRenderContext_Base(axMTLRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
}

void axMTLRenderContext_Base::onPreDestroy() {
}

void axMTLRenderContext_Base::_onCreate(MTKView* mtkView) {
	_cmdQueue = [mtkView.device newCommandQueue];

	_mtkView.ref(mtkView);
//	[mtkView setDelegate:_mtkViewDelegate];

	// If enableSetNeedsDisplay and paused are YES,
	// the view behaves similarly to a UIView object, responding to calls to setNeedsDisplay.
	[mtkView setEnableSetNeedsDisplay:true];
	[mtkView setPaused:true];
//	[mtkView setAutoResizeDrawable:true];
}

void axMTLRenderContext_Base::onDispatchCommands(axRenderRequest& req) {
	dispatch_semaphore_wait(renderer()->_semaphore, DISPATCH_TIME_FOREVER);

	_cmdBuffer = nil;
	_cmdEncoder = nil;

	_cmdBuffer = [_cmdQueue commandBuffer];
	_cmdBuffer.label = @"axRenderContext_Command";

	_computeCmdBuffer = [_cmdQueue commandBuffer];
	_computeCmdBuffer.label = @"ax_ComputeCommand";
//	_computeCmdEncoder = [_computeCmdBuffer computeCommandEncoderWithDispatchType:MTLDispatchTypeSerial]; // require OSX 10.14
	_computeCmdEncoder = [_computeCmdBuffer computeCommandEncoder];

	_blitCmdBuffer = [_cmdQueue commandBuffer];
	_blitCmdBuffer.label = @"ax_BlitCommand";
	_blitCmdEncoder = [_blitCmdBuffer blitCommandEncoder];

	[_mtkView setClearColor:MTLClearColorMake(.1, .3, .6, 1)];
	[_mtkView setClearDepth:1.0];
	
	// Obtain a renderPassDescriptor generated from the view's drawable textures.
	MTLRenderPassDescriptor *renderPassDescriptor = _mtkView.ptr().currentRenderPassDescriptor;
	if (renderPassDescriptor) {
		_cmdEncoder = [_cmdBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
		_cmdEncoder.label = @"ax_RenderEncoder";
	}

//---------------
	axMTLCommandDispatcher dispatcher(this, &req);
	dispatcher.dispatch();

//------------
	__block dispatch_semaphore_t block_sema = renderer()->_semaphore;
	[_cmdBuffer addCompletedHandler:^(id<MTLCommandBuffer> cmdBuf) {
		dispatch_semaphore_signal(block_sema);
	}];
	
	auto* fence = ax_type_cast_debug<axMTLFence>(req.completedFence());
	fence->addToGpu(_cmdBuffer);

	[_cmdBuffer commit];
}

void axMTLRenderContext_Base::_setDrawableSize(const axVec2f& size) {
	auto s = size;
	if (size.x <= 0 || ax_is_nan(size.x) || ax_is_inf(size.x)
	 || size.y <= 0 || ax_is_nan(size.y) || ax_is_inf(size.y))
	{
		s.set(4,4);
	}
	[_mtkView setDrawableSize:s];
}

void axMTLRenderContext_Base::onSetNativeViewRect(const axRect2f& rect) {
	if (_mtkView) {
		auto h = (float)[_mtkView superview].frame.size.height;
		auto rc = rect;
		rc.y = h - rect.yMax();
		[_mtkView setFrame:rc];
		_setDrawableSize(rc.size);
	}
	Base::onSetNativeViewRect(rect);
}

void axMTLRenderContext_Base::onSetNeedToRender() {
	if (_mtkView) {
		auto size = axVec2f(_mtkView.ptr().bounds.size);
		_setDrawableSize(size);
		if (size.x > 0 && size.y > 0)
			[_mtkView setNeedsDisplay:true];
	}
}

#endif //AX_USE_METAL
