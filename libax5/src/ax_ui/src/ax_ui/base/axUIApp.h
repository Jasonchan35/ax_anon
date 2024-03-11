#pragma once

#include <ax_ui/base/axUIDefaultAssets.h>
#include <ax_ui/base/axUIRootView.h>

class axUIApp : public axNativeUIApp {
	AX_RTTI_CLASS(axUIApp, axNativeUIApp);
public:
	ax_DOWNCAST_GET_INSTANCE();

	class CreateDesc : public axNonCopyable
	{
	public:
		axRenderApi	renderApi = axRenderApi::Unknown;
	};

	axUIApp();
	virtual ~axUIApp();

	using Renderer		= axRenderer;
	using UIMouseEvent	= axUIMouseEvent;
	using UIKeyEvent	= axUIKeyEvent;

	virtual void onCreate() override;

	axUIRootView*	rootView	() { return _rootView.ptr(); }
	axRenderer*		renderer	() { return _renderer.ptr(); }

	axUIDefaultAssets*	defaultAssets() { return _defaultAssets.ptr(); }
	axFontManager*		fontManager()	{ return _fontManager.ptr(); }

	void setQuitWhenLastWindowClosed(bool b)	{ _quitWhenLastWindowClosed = b; }
	bool quitWhenLastWindowClosed() const		{ return _quitWhenLastWindowClosed; }

	void	onDebugRender(axRenderRequest& req);

	class Debug {
	public:
		Debug();
		bool showUIRect : 1;
	};

	Debug debug;

	void setRenderApi(axRenderApi v, bool multithread);

	void setNeedToLayoutRootView();
	void setNeedToDestroyUIView(axUIView* v);

	void _onWindowCreate(axUIWindow* w);
	void _onWindowDestroy(axUIWindow* w);

	bool handleKeyboardShortcut(UIKeyEvent& ev);
protected:
	virtual bool onKeyboardShortcut(UIKeyEvent& ev) { return false; }

	virtual void onHandleCustomAppEvent() override;
	virtual void onUpdateLayout() override;
	virtual void onSetNeedToRender() override;

	struct CustomEvents {
		CustomEvents() : layoutRootView(false) {}
		bool layoutRootView : 1;
		axLinkArray<axUIView, 4> destroyUIViewList;
	};

	CustomEvents	_customEvents;

	axRenderer_CreateDesc		_rendererCreateDesc;
	axSPtr<axRenderer>			_renderer;
	axUPtr<axFontManager>		_fontManager;
	axUPtr<axUIDefaultAssets>	_defaultAssets;
	axUPtr<axUIRootView>		_rootView;
	axLinkArray<axUIWindow, 4>	_windowList;
	bool _quitWhenLastWindowClosed = true;
};
