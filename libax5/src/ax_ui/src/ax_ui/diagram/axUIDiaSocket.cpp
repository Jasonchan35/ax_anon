#include "axUIDiaSocket.h"
#include "axUIDiagram.h"

axUIDiaSocket::axUIDiaSocket(axUIView* parent, Prop* prop, bool isInput)
	: Base(parent)
	, _isInput(isInput)
{
	_prop = prop;
	setDraggable(true);

	_texture = axUIDefaultAssets::s_instance()->textures.diaSocketCircle;
	auto size = axVec2f::s_cast(_texture->size());
	setSize(size);
}

axUIDiaSocket::~axUIDiaSocket() {
	_inputConnections.clear();
	_outputConnections.clear();
}

void axUIDiaSocket::onRender(axRenderRequest& req) {
	auto color0 = Color(0,0,0);
	auto color1 = Color(1,1,1);
	auto color2 = Color(0,0,0,0);
	auto color3 = Color(0,0,0,0);

	if (isMouseHover()) {
		color0 = {1,1,1};
	} else if (hasConnection()) {
		color1 = {.9f, .9f, .4f};
	}

//	req.ui.drawRect(AX_LOC, localRect(), color);
	req.ui.drawColorMaskTexture(AX_LOC, _texture, contentRect(), Rect2(0,0,1,1), color0, color1, color2, color3);
}

void axUIDiaSocket::onMouseEvent(axUIMouseEvent& ev) {
	switch (ev.type) {
		case axUIMouseEvent::Type::Down: {
			if (ev.button == axUIMouseEvent::Button::Left) {
				auto* conn = diagram()->onNewConnectionView(this, nullptr);

				auto center = localRect().center();
				if (_isInput) {
					conn->setInput(this);
				} else {
					conn->setOutput(this);
				}

				conn->setInputPos(center);
				conn->setOutputPos(center);
				conn->startDragging(!_isInput);
				axUIInputManager::s_instance()->beginDragAndDrop(conn);
			}
		}break;

		case axUIMouseEvent::Type::DragAndDropQueryAcceptSource: {
			auto* conn = ax_type_cast<Connection>(ev.dragAndDropSource.ptr());
			if (!conn) return;

			bool connIsInput = conn->isDraggingInput();
			if (connIsInput != _isInput) return;

			auto* dia	  = diagram();
			auto* srcProp = prop();
			auto* dstProp = conn->getOppositeDraggingProp();

			if (!dia || !srcProp || !dstProp) {
				AX_ASSERT(false);
				return;
			}

			if (_isInput) {
				ax_swap(srcProp, dstProp);
			}

			if (!dia->onWillAcceptConnection(srcProp, dstProp))
				return;

			ev.result.dragAndDropAcceptSource = true;
		}break;

		case axUIMouseEvent::Type::DragAndDropEnd: {
			auto* conn = ax_type_cast<Connection>(ev.dragAndDropSource.ptr());
			if (!conn) return;

			bool connIsInput = conn->isDraggingInput();
			if (connIsInput != _isInput) { AX_ASSERT(false); return; }

			auto* dia	  = diagram();
			auto* srcProp = prop();
			auto* dstProp = conn->getOppositeDraggingProp();

			if (!dia || !srcProp || !dstProp) {
				AX_ASSERT(false);
				return;
			}

			if (_isInput) {
				ax_swap(srcProp, dstProp);
			}

			if (_isInput) {
				conn->setInput(this);
			} else {
				conn->setOutput(this);
			}

			if (!dia->onAcceptConnection(conn))
				return;

			ev.result.dragAndDropAcceptSource = true;

			conn->onUpdateConnectionPosition();
		}break;

		case axUIMouseEvent::Type::Move: {
			forwardEventToParent(ev);
		}break;
	}
	setNeedToRender();
}

void axUIDiaSocket::onUpdateLayout() {
//	Base::onUpdateLayout(); // don't use default layout
	onUpdateConnectionPosition();
}

axUIDiaSocket::Node* axUIDiaSocket::node() {
	return _prop ? _prop->node() : nullptr;
}

axUIDiagram* axUIDiaSocket::diagram() {
	auto* n = node();
	return n ? n->diagram() : nullptr;
}

void axUIDiaSocket::onUpdateConnectionPosition() {
	for (auto& e : _inputConnections) {
		e.onUpdateConnectionPosition();
	}
	for (auto& e : _outputConnections) {
		e.onUpdateConnectionPosition();
	}
}

bool axUIDiaSocket::hasConnection() const {
	return _inputConnections.size() || _outputConnections.size();
}
