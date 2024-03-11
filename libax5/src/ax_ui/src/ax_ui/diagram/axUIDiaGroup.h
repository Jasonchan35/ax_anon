#pragma once

#include "axUIDiaNode.h"

class axUIDiaGroup : public axUIDiaNodeBase {
	AX_RTTI_CLASS(axUIDiaGroup, axUIDiaNodeBase)
public:
	axUIDiaGroup(axUIDiaGroup* parent);

	axUIDiaNode*  newNode () { return ax_new axUIDiaNode (this); }
	axUIDiaGroup* newGroup() { return ax_new axUIDiaGroup(this); }

	void clearChildNodes();

	void setTitle(axStrView title);

	axUIView*	childNodeView() { return &_childNodeView; }

protected:
	axUIDiaGroup(axUIView* view); // only for RootView

	class ChildNodeView : public axUIView {
		AX_RTTI_CLASS(ChildNodeView, axUIView);
	public:
		ChildNodeView(axUIDiaGroup* parent);
		virtual void onMeasureLayout(axUIMeasureRequest& req) override;
		virtual void onUpdateLayout() override;

		axUIDiaGroup* _owner;
	};

	virtual void onRender(axRenderRequest& req) override;
	virtual void onUpdateLayout() override;
	virtual void onUpdateConnectionPosition() override;

	axUIVBoxLayoutHandler	_vboxLayoutHandler;
	axUILabel		_titleBar;
	ChildNodeView	_childNodeView;
};