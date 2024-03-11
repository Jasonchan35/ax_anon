#pragma once

class axUIRenderContextView : public axUIView {
	AX_RTTI_CLASS(axUIRenderContextView, axUIView);
public:
	using CreateDesc = axRenderContext::CreateDesc;

	axUIRenderContextView(axUIView* parent, bool bCreateRenderContext = true);

	virtual void onRenderContextRender(axRenderRequest& req);

	virtual void onSetPos(const axVec2f& v) override;
	virtual void onSetSize(const axVec2f& v) override;

	void createRenderContext();
	void createRenderContext(CreateDesc& desc);
	void destroyRenderContext();

protected:
	virtual void onSetNeedToRender() override;

private:

	class RenderEventHandler : public axUIRenderEventHandler {
	public:
		virtual void onRender(axRenderRequest& req) override { owner->onRenderContextRender(req); }
		axUIRenderContextView* owner = nullptr;
	};

	RenderEventHandler _renderEventHandler;
	axSPtr<axRenderContext>	_renderContext;
};