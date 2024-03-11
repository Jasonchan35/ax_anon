#pragma once

#include "../base/axUIWindow.h"
#include "../base/axUIRenderContextView.h"
#include "../layout/axUIBoxLayout.h"
#include "../text/axUITextMesh.h"

class axUIContextMenu : public axUIWindow {
	AX_RTTI_CLASS(axUIContextMenu, axUIWindow)
public:
	struct CreateDesc : public Base::CreateDesc {
		CreateDesc();
	};

	axUIContextMenu(axUIView* parent);

	using Func = axFunc<void ()>;

	void addItem(axStrView text, axObject* obj, Func func);

	void showOnScreen();

protected:
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onSetNeedToRender() override;
	virtual void onWindowActive(bool b) override;
	virtual void onRenderChildren(axRenderRequest& req) override;

private:
	static CreateDesc& _createDesc() {
		static CreateDesc desc;
		return desc;
	}

	class RenderView : axUIRenderContextView {
		AX_RTTI_CLASS(RenderView, axUIRenderContextView)
	public:
		virtual void onRenderContextRender(axRenderRequest& req) override;
	};

	class Menu : public VBoxView {
		AX_RTTI_CLASS(Menu, VBoxView)
	public:
		Menu(axUIView* parent);
	};

	class MenuItem : public axUIView {
		AX_RTTI_CLASS(MenuItem, axUIView)
	public:
		using Func = axUIContextMenu::Func;

		MenuItem(axUIView* menu, axUIContextMenu* parent, axObject* obj, Func func);

		void setText(axStrViewU text);
		void setText(axStrView  text) { setText(axTempStringU::s_convert(text)); }

		virtual void onMouseEvent(axUIMouseEvent& ev) override;
		virtual void onRender(axRenderRequest& req) override;
		virtual void onMeasureLayout(axUIMeasureRequest& req) override;

	private:
		axUIContextMenu*	_owner = nullptr;
		axUITextMesh		_textMesh;
		axDelegate<void ()>	_delegate;
		bool _isDown = false;
	};

	axUIRenderContextView	_renderView;
	Menu _menu;
};

