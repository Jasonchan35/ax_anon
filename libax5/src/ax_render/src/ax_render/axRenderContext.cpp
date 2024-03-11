#include "axRenderContext.h"
#include <ax_render/axRenderRequest.h>

axRenderContext::axRenderContext(axRenderer* renderer, CreateDesc& desc)
	: _debugName(desc.debugName)
	, _renderer(renderer)
	, _window(desc.window)
	, _eventHandler(desc.eventHandler)
{
}

void axRenderContext::onPreDestroy() {
	ax_log_func_name();
}

void axRenderContext::setNativeViewRect(const axRect2f& rect_) {
	auto rect = rect_;
	ax_max_it(rect.size, {4,4}); // never let the width/height too small

	if (rect == _nativeViewRect) return;
	onSetNativeViewRect(rect);
	_nativeViewRect = rect;
	setNeedToRender();
}

void axRenderContext_EventHandler::render(axRenderContext* renderContext) {
	auto* renderer = renderContext->renderer();
	auto req = renderer->newRequest(renderContext, renderContext->worldRect());
	try {
		AX_FrameMark;
		AX_ZoneScoped;
		onRender(*req);
//		req->swapBuffers();
	} catch (...) {
		renderer->freeRequest(ax_move(req));
		return;
	}
	renderer->submitRequest(ax_move(req));
}

void axRenderContext::setNeedToRender() {
	onSetNeedToRender();
}
