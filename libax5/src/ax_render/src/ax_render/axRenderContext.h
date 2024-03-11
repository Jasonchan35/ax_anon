#pragma once

#include "vertex/axRenderVertexDesc.h"
#include "buffer/axRenderMultiGpuBuffer.h"
#include "axRenderRequest.h"

class axRenderContext_EventHandler {
public:
	virtual ~axRenderContext_EventHandler() = default;

	void render(axRenderContext* renderContext);

	virtual void onRender(axRenderRequest& req) {}
	virtual void onUpdateGpuResources(axRenderCommandDispatcher& dispatcher) {}
};

class axRenderContext_CreateDesc {
public:
	axNativeUIWindow* window = nullptr;
	axRenderContext_EventHandler* eventHandler = nullptr;
	axNativeUIWindow_CreateDesc winDesc;
	axStrView debugName;
};

class axRenderContext : public axObject, public axRenderCommonBase {
	AX_RTTI_CLASS(axRenderContext, axObject)
public:
	using CreateDesc = axRenderContext_CreateDesc;

	using NativeUIMouseEvent = axNativeUIMouseEvent;
	using NativeUIKeyEvent = axNativeUIKeyEvent;
	using EventHandler	= axRenderContext_EventHandler;
	using Rect2			= axRect2f;
	using Vec2			= axVec2f;
	
	virtual ~axRenderContext() {}

	axRenderer*			renderer() { return _renderer; };
	axNativeUIWindow*	window	() { return _window; }
	EventHandler*		eventHandler() { return _eventHandler; }

	void setNativeViewRect(const axRect2f& rect);
	const Rect2& nativeViewRect()	{ return _nativeViewRect; }

	Rect2	worldRect() const		{ return _nativeViewRect.offset(_window->contentWorldPos()); }
	Vec2	worldPos() const		{ return _nativeViewRect.pos + _window->contentWorldPos(); }

	void setNeedToRender();

				axInt	frameAheadCount() const	{ return _frameAheadCount; }

friend class axRenderer;
friend class axRenderContext_EventHandler;
protected:
	axRenderContext(axRenderer* renderer, CreateDesc& desc);

	virtual void onSetNeedToRender() = 0;
	virtual void onDispatchCommands(axRenderRequest& req) = 0;

	virtual void onSetNativeViewRect(const axRect2f& rect) {}
	virtual void onPreDestroy();

	axString			_debugName;
	axRenderer*			_renderer = nullptr;
	axNativeUIWindow*	_window = nullptr;
	EventHandler*		_eventHandler = nullptr;
	axRect2f			_nativeViewRect;
	axInt				_frameAheadCount = axRender_kMaxFrameAheadCount;
};

