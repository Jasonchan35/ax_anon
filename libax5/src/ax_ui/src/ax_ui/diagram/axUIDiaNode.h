#pragma once

#include "axUIDiaProp.h"
#include "../widget/axUILabel.h"

class axUIDiagram;
class axUIDiaGroup;

class axUIDiaNodeBase : public axUIDiaItem {
	AX_RTTI_CLASS(axUIDiaNodeBase, axUIDiaItem)
public:
	axUIDiaNodeBase(axUIView* parent) : Base(parent) {}

	virtual axUIDiaProp* findInputProp (axStrView name) { return nullptr; }
	virtual axUIDiaProp* findOutputProp(axStrView name) { return nullptr; }

	void updateConnectionPosition() { onUpdateConnectionPosition(); }
protected:
	virtual void onUpdateConnectionPosition() {}
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onDiaNodePositionChanged() {}

private:
	axVec2f		_dragStartWorldPos;
};

class axUIDiaNode : public axUIDiaNodeBase {
	AX_RTTI_CLASS(axUIDiaNode, axUIDiaNodeBase)
public:

	class TitleBar : public axUIHBoxView {
		AX_RTTI_CLASS(TitleBar, axUIHBoxView);
	public:
		TitleBar(axUIDiaNode* node);

		axUIDiaNode*	node() { return _node; }

		axUILabel		_nameLabel;
	private:
		axUIDiaNode*	_node = nullptr;
	};

	axUIDiaNode(axUIDiaGroup* parent);

		void	setName(axStrView name);
	axStrView	name() const;

	void clearAllProps();

	axUIDiaProp* newInputProp (axStrView name, axStrView displayName);
	axUIDiaProp* newOutputProp(axStrView name, axStrView displayName);

	virtual axUIDiaProp* findInputProp (axStrView name) override;
	virtual axUIDiaProp* findOutputProp(axStrView name) override;

	virtual void onRender(axRenderRequest& req) override;

	TitleBar& titleBar() & { return _titleBar; }

protected:
	virtual void onUpdateConnectionPosition() override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

	TitleBar	_titleBar;
	axString	_name;
	axUIHBoxView		_propGroup;
	axUIDiaInputPropGroup	_inputPropGroup;
	axUIDiaOutputPropGroup	_outputPropGroup;
	axUIVBoxLayoutHandler	_vboxLayoutHandler;
};
