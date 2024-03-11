#include "axUIWindow.h"
#include "axUIApp.h"

class axUIWindow::NativeWindow : public axNativeUIWindow {
	AX_RTTI_CLASS(NativeWindow, axNativeUIWindow);
public:
	NativeWindow(axUIWindow* owner, CreateDesc& desc) 
		: Base(desc)
	{
		_owner = owner;
	}

	~NativeWindow() {
		auto* p = _owner;
		_owner = nullptr;
		if (p) {
			p->_nativeWindow = nullptr;
			ax_delete(p);
		}
	}

	virtual void onSetWorldPos(const axVec2f& v) override {
		auto re = _re_setPos.enter();
		if (!re) return;
		Base::onSetWorldPos(v);
		_owner->setWorldPos(v);
	}

	virtual void onSetSize(const axVec2f& v) override {
		auto re = _re_setSize.enter();
		if (!re) return;

		Base::onSetSize(v);
		_owner->onSetSize(v);
	}

	virtual void onNativeUIMouseEvent(axNativeUIMouseEvent& ev) override {
		axUIInputManager::s_instance()->handleNativeUIMouseEvent(_owner, ev);
	}

	virtual void onNativeUIKeyEvent(axNativeUIKeyEvent& ev) override {
		axUIInputManager::s_instance()->handleNativeUIKeyEvent(_owner, ev);
	}

	virtual void onCloseButton() override {
		if (!_owner->onWindowCloseButton()) return;
		ax_delete(_owner);
	}

	virtual void onActive(bool b) override {
		Base::onActive(b);
		_owner->_windowActive = b;
		_owner->_onWindowActive(b);
	}

	axEnterOnce	_re_setPos;
	axEnterOnce	_re_setSize;

	axUIWindow* _owner = nullptr;
};

axUIWindow::axUIWindow(axUIView* parent, CreateDesc& desc) 
	: Base(parent)
{
	_uiWindow = this;
	_windowActive = false;
	layout.setFixedW();
	layout.setFixedH();

	_nativeWindow = ax_new NativeWindow(this, desc);
	setWorldRect(_nativeWindow->worldRect());
	setPadding(_nativeWindow->padding());
	
	axUIApp::s_instance()->_onWindowCreate(this);
}

axUIWindow::~axUIWindow() {
	axUIApp::s_instance()->_onWindowDestroy(this);
	
	auto* p = _nativeWindow;
	_nativeWindow = nullptr;
	if (p) {
		p->_owner = nullptr;
		ax_delete(p);
	}
}

axUIScreen axUIWindow::screen() {
	if (!_nativeWindow) return axUIScreen();
	auto p = _nativeWindow->screen();
	return axUIScreen(p);
}

axNativeUIWindow* axUIWindow::getNativeWindow() {
	return _nativeWindow;
}

void axUIWindow::setWindowActive(bool b) {
	if (_nativeWindow) {
		_nativeWindow->setActive(b);
	}
}

void axUIWindow::onSetPos(const axVec2f& v) {
	if (_nativeWindow) {
		auto wp = v;
		if (_parent) wp += _parent->worldPos();
		_nativeWindow->setWorldPos(wp);
	}
	Base::onSetPos(v);
}

void axUIWindow::onSetSize(const axVec2f& v) {
	if (_nativeWindow) {
		_nativeWindow->setSize(v);
	}
	Base::onSetSize(v);
}

void axUIWindow::onSetVisible(bool b) {
	if (_nativeWindow) {
		_nativeWindow->setVisible(b);
	}
}

void axUIWindow::_onWindowActive(bool b) {
	_windowActive = b;
	onWindowActive(b);
}

void axUIWindow::setWindowTitle(axStrView title) {
	if (_nativeWindow) {
		_nativeWindow->setWindowTitle(title);
	}
}

void axUIWindow::closeWindow() {
	if (_nativeWindow) {
		_nativeWindow->closeWindow();
	}
}

void axUIWindow::_destroyWindow() {
	if (_nativeWindow) {
		_nativeWindow->destroyWindow();
	}
}
