#include "axRenderCommandDispatcher.h"

axRenderCommandDispatcher::axRenderCommandDispatcher(axRenderContext* renderContext, axRenderRequest* renderReq_)
	: renderContextBase(renderContext)
	, renderer(renderContext->renderer())
	, _renderReqBase(renderReq_)
	, _onGpuFrameBeginCalled(false)
	, _onGpuFrameEndCalled(false)
{
	_renderSeqId = renderReq_->renderSeqId();
}

axRenderCommandDispatcher::~axRenderCommandDispatcher() {
	AX_ASSERT(_onGpuFrameBeginCalled);
	AX_ASSERT(_onGpuFrameEndCalled);
}

void axRenderCommandDispatcher::onGpuFrameBegin() {
	_onGpuFrameBeginCalled = true;
	renderContextBase->eventHandler()->onUpdateGpuResources(*this);
}

void axRenderCommandDispatcher::onGpuFrameEnd() {
	_onGpuFrameEndCalled = true;
}
