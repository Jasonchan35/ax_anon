#include "axUIDiaNode.h"
#include "axUIDiagram.h"

void axUIDiaNodeBase::onMouseEvent(axUIMouseEvent& ev) {
	using Button = axUIMouseEvent::Button;
	using Type   = axUIMouseEvent::Type;
//	using Modifier = axUIMouseEvent::Modifier;

	switch (ev.type) {
		case Type::DragBegin: {
			_dragStartWorldPos = worldPos();
		}break;

		case Type::Dragging: {
			if (ev.pressedButtons == Button::Left) {
				auto newPos = _dragStartWorldPos + ev.dragDeltaPos;

				{ // snap
					auto rootPos = diagram()->root()->worldPos();
					newPos -= rootPos;
					newPos = ax_round(newPos / kSnap) * kSnap;
					newPos += rootPos;
				}

				setWorldPos(newPos);
				onUpdateConnectionPosition();

				if (_parent) {
					_parent->setNeedToMeasure();
				}

			} else if (ev.pressedButtons == Button::Right) {
				forwardEventToParent(ev);
			}
		}break;

		case Type::DragEnd: {
			onDiaNodePositionChanged();
		}break;
	}
}

axUIDiaNode::axUIDiaNode(axUIDiaGroup* parent) 
	: Base(parent->childNodeView())
	, _titleBar(this)
	, _propGroup(this)
	, _inputPropGroup(&_propGroup)
	, _outputPropGroup(&_propGroup)
{
	setBgColor(.2f, .2f, .4f);
	setLayoutHandler(&_vboxLayoutHandler);

	layout.setFitContentW();
	layout.setFitContentH();

	setHitTestMargin(12);

#if 0
	const float layoutSpacing = 2.0f;
	_titleBar.setHeight(kSnap - layoutSpacing);
	_titleBar.layout.setFixedH();
#else
	_titleBar.layout.setFitContentH();
#endif

	_titleBar.layout.setMinW(100);

	_inputPropGroup.setWidth(160);
	_outputPropGroup.setWidth(160);
}

void axUIDiaNode::setName(axStrView name) {
	_name = name;
	_titleBar._nameLabel.setText(name);
}

axStrView axUIDiaNode::name() const {
	return _name;
}

void axUIDiaNode::onRender(axRenderRequest& req) {
	Base::onRender(req);
	if (isSelected()) {
		req.ui.drawRectBorder(AX_LOC, localRect(), Margin2(-2), axColor::kYellow());
	} else {
		req.ui.drawRectBorder(AX_LOC, localRect(), Margin2(-1), axColor::kBlack());
	}
}

void axUIDiaNode::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.isLeftDown()) {
		setAsLastSibling();
	} else if (ev.isLeftClick()) {
		auto* dia = diagram();
		if (ev.isCtrlDown()) {
			if (isSelected()) {
				dia->removeSelection(this, true);
			}else{
				dia->addSelection(this, true);
			}
		} else {
			dia->setSelection(this, true);
		}
	}
	Base::onMouseEvent(ev);
}

axUIDiaNode::TitleBar::TitleBar(axUIDiaNode* node)
	: Base(node)
	, _nameLabel(this)
	, _node(node)
{
	_nameLabel.layout.setExpandW();
}

void axUIDiaNode::clearAllProps() {
	_inputPropGroup.clearChildren();
	_outputPropGroup.clearChildren();
}

axUIDiaProp* axUIDiaNode::newInputProp(axStrView name, axStrView displayName) {
	return ax_new axUIDiaProp(&_inputPropGroup, this, name, displayName, true);
}

axUIDiaProp* axUIDiaNode::newOutputProp(axStrView name, axStrView displayName) {
	return ax_new axUIDiaProp(&_outputPropGroup, this, name, displayName, false);
}

axUIDiaProp* axUIDiaNode::findInputProp(axStrView name) {
	return _inputPropGroup.findProp(name);
}

axUIDiaProp* axUIDiaNode::findOutputProp(axStrView name) {
	return _outputPropGroup.findProp(name);
}

void axUIDiaNode::onUpdateConnectionPosition() {
	Base::onUpdateConnectionPosition();

	_inputPropGroup.onUpdateConnectionPosition();
	_outputPropGroup.onUpdateConnectionPosition();
}

