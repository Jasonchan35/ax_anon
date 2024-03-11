#include <ax_ui/base/axUIApp.h>
#include <ax_ui/base/axUIWindow.h>
#include <ax_ui/base/axUIView.h>

axUIApp::axUIApp() {
#if AX_USE_Gtk
	gtk_init(nullptr, nullptr);
#endif
//	AX_LOG("axUIApp thread={?}", axThreadUtil::currentThreadId());
}

axUIApp::~axUIApp() {
}

void axUIApp::setRenderApi(axRenderApi v, bool multithread) {
	if (_renderer) {
		AX_ASSERT_MSG(false, "cannot change render api after create");
		return;
	}
	_rendererCreateDesc.api = v;
	_rendererCreateDesc.multithread = multithread;
}

void axUIApp::setNeedToLayoutRootView() {
	if (_customEvents.layoutRootView) return;
	_customEvents.layoutRootView = true;
	requestCustomAppEvent();
}

void axUIApp::setNeedToDestroyUIView(axUIView* v) {
	if (!v) return;
	_customEvents.destroyUIViewList.emplaceBack(v);
	requestCustomAppEvent();
}

void axUIApp::onCreate() {
	_renderer = axRenderer_create(_rendererCreateDesc);
	_rootView.ref(ax_new axUIRootView());
	_rootView->setDebugName("axUIApp.root");
	_rootView->setRect(s_getVirtualDesktopRect());

	_defaultAssets.ref(ax_new axUIDefaultAssets());
	_fontManager.ref(ax_new axFontManager());
}

void axUIApp::onDebugRender(axRenderRequest& req) {
	if (_fontManager) {
		_fontManager->onDebugRender(req);
	}
}

void axUIApp::onHandleCustomAppEvent() {
	if (_customEvents.layoutRootView) {
		_customEvents.layoutRootView = false;
		onUpdateLayout();
	}

	if (_customEvents.destroyUIViewList.size()) {
		for (auto& v : _customEvents.destroyUIViewList) {
			if (!v) continue;
			ax_delete(v);
		}
		_customEvents.destroyUIViewList.clear();
	}
}

void axUIApp::onSetNeedToRender() {
	if (!_rootView) return;
	for (auto& c : _rootView->children()) {
		if (auto* win = ax_type_cast<axUIWindow>(&c)) {
			win->setNeedToRender();
		}
	}
}

void axUIApp::onUpdateLayout() {
	AX_ZoneScoped;
	if (_rootView) {
		_rootView->updateLayout();
	}
}

void axUIApp::_onWindowCreate(axUIWindow* w) {
	_windowList.emplaceBack(w);
}

void axUIApp::_onWindowDestroy(axUIWindow* w) {
	auto ret = _windowList.findAndRemove(w);
	if (ret < 0) {
		AX_ASSERT(false);
	}
	
	if (_quitWhenLastWindowClosed && _windowList.size() == 0) {
		quit(0);
	}
}

bool axUIApp::handleKeyboardShortcut(UIKeyEvent& ev) {
	using Type = UIKeyEvent::Type;
	using KeyCode = UIKeyEvent::KeyCode;

	if (ev.type == Type::Down) {
		switch (ev.key) {
			case KeyCode::F1: {
				AX_LOG("===== dump UI ====================================");
				rootView()->dump();
			}break;
			case KeyCode::F2: {
				AX_TOGGLE_BOOL(debug.showUIRect);
				setNeedToRender();
			}break;
		}
	}

	return onKeyboardShortcut(ev);
}

axUIApp::Debug::Debug()
	: showUIRect(false)
{
}
