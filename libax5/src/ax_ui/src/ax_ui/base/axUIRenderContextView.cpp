#include "axUIRenderContextView.h"
#include "../dock/axUIDockZone.h"

axUIRenderContextView::axUIRenderContextView(axUIView* parent, bool bCreateRenderContext) 
	: Base(parent)
{
	_uiRenderContextView = this;
	_renderEventHandler.owner = this;
	if (bCreateRenderContext)
		createRenderContext();
}

void axUIRenderContextView::onRenderContextRender(axRenderRequest& req) {
	auto rc = localRect();
	req.setViewport(rc);

	auto bgColor = axColor(0.3f, .3f, .3f);
	req.clearColorAndDepthBuffers(bgColor, 1);
	render(req);
}

void axUIRenderContextView::onSetNeedToRender() {
	if (!_renderContext) return;
	_renderContext->setNeedToRender();
}

void axUIRenderContextView::onSetPos(const axVec2f& v) {
	Base::onSetPos(v);
	if (_renderContext) {
		_renderContext->setNativeViewRect(rectOnWindow());
	}
}

void axUIRenderContextView::onSetSize(const axVec2f& v_) {
	axVec2f v = v_;

	const float maxSize = 4 * 1024;
	AX_ASSERT(v.x <= maxSize && v.y <= maxSize);
	ax_min_it(v.x, maxSize);
	ax_min_it(v.y, maxSize);

	Base::onSetSize(v);
	if (_renderContext) {
		_renderContext->setNativeViewRect(rectOnWindow());
	}
}

void axUIRenderContextView::createRenderContext(CreateDesc& desc) {
	if (_renderContext) return;

	auto* win = uiWindow();
	if (win) {
		desc.window = win->getNativeWindow();
		desc.eventHandler = &_renderEventHandler;

		_renderContext = axRenderer::s_instance()->createRenderContext(desc);
		_renderContext->setNativeViewRect(rectOnWindow());
	}
}

void axUIRenderContextView::createRenderContext() {
	CreateDesc desc;
	desc.debugName = debugName();
	createRenderContext(desc);
}

void axUIRenderContextView::destroyRenderContext() {
	_renderContext.unref();
}
