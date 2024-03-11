#pragma once

#include "axUIView.h"

class axUIWindow : public axUIView {
	AX_RTTI_CLASS(axUIWindow, axUIView)
public:

	struct CreateDesc : public axNativeUIWindow_CreateDesc {};

	axUIWindow(axUIView* parent, CreateDesc& desc);
	~axUIWindow();

	axUIScreen	screen();

	axNativeUIWindow*	getNativeWindow();

			void setWindowActive(bool b);
	virtual void onWindowActive	(bool b) {}
	void setWindowTitle(axStrView title);

	void closeWindow();

	void _destroyWindow(); // please call axUIView::setNeedToDestroy();

protected:
	virtual void onSetPos		(const axVec2f& v) override;
	virtual void onSetSize		(const axVec2f& v) override;
	virtual void onSetVisible	(bool b) override;
	virtual bool onWindowCloseButton() { return true; }

			void _onWindowActive(bool b);

private:
	CreateDesc _createDesc;

	class NativeWindow;
	NativeWindow* _nativeWindow = nullptr;
	bool	_windowActive : 1;
};
