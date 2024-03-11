#pragma once

#include "../base/axUIRenderContextView.h"
#include "axUIDockPanel.h"
#include "../layout/axUIBoxLayout.h"
#include "../widget/axUIButton.h"

class axUIDockZone : public axUIView {
	AX_RTTI_CLASS(axUIDockZone, axUIView)
public:
	using PanelList = axDList<axUIDockPanel, ax_no_delete>;

	axUIDockZone(axUIView* parent);

	virtual axUIDockZone* split(float size, bool isVertical);

	virtual void addPanel(axUIDockPanel* panel);

	axUIDockPanel* getActivePanel();

	bool splitted() const { return _splitted; }
	bool splitIsVertical() const { return _splitIsVertical; }

	float splittedSize() const { return _splittedSize; }
	void setSplittedSize(float f) { _splittedSize = f; }

	static const float kDividerSize;

	void onPanelTitleChanged(axUIDockPanel* panel);

	class Divider : public axUIRenderContextView {
		AX_RTTI_CLASS(Divider, axUIRenderContextView)
	public:
		static const float kDividerSize;

		Divider(axUIDockZone* parent);
		virtual void onMouseEvent(axUIMouseEvent& ev) override;
		virtual void onRenderContextRender(axRenderRequest& req) override;

		void setCursorResize();
		void setCursorArrow();

		float _startSplittedSize = 0;
		axUIDockZone* _zone = nullptr;
		bool _dragging = false;
	};


	class TabBar : public axUIHBoxView {
		AX_RTTI_CLASS(TabBar, axUIHBoxView)
	public:
		TabBar(axUIView* parent);
		void onPanelTitleChanged(axUIDockPanel* panel);
	};

	class TabButton : public axUIButton {
		AX_RTTI_CLASS(TabButton, axUIButton)
	public:
		axLink<axUIDockPanel> _panel;
		TabButton(axUIView* parent, axStrView labelText) : Base(parent, labelText) {}
	};

	class PanelArea : public axUIRenderContextView {
		AX_RTTI_CLASS(PanelArea, axUIRenderContextView)
	public:
		PanelArea(axUIDockZone* parent);

		virtual void onRenderContextRender(axRenderRequest& req) override;
		virtual void onRenderChildren(axRenderRequest& req) override;
		virtual void onMeasureLayout(axUIMeasureRequest& req) override;
		virtual void onUpdateLayout() override;

		void onPanelTitleChanged(axUIDockPanel* panel);

		axUIDockPanel* getActivePanel();

		void addPanel(axUIDockPanel* panel);

	private:
		axUIDockZone* getZone();
		axUIDockZone* getZoneParent();
		bool isZone1();

		PanelList				_panels;
		axLink<axUIDockPanel>	_activeTab;
		TabBar					_tabBar;
	};

protected:
	virtual void onSetNeedToRender() override;
	virtual void onUpdateLayout() override;
	virtual void onMeasureLayout(axUIMeasureRequest& req) override;

private:
	bool _splitIsVertical = false;
	bool _splitted = false;
	float _splittedSize = 0;

	axUPtr<axUIDockZone> _zone0;
	axUPtr<axUIDockZone> _zone1;
	axUPtr<Divider>		 _divider;
	axUPtr<PanelArea>	_panelArea;
};
