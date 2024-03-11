#include "axUIDiaConnection.h"
#include "axUIDiaSocket.h"
#include "axUIDiagram.h"

axUIDiaConnection::axUIDiaConnection(axUIDiaSocket* parent, axUIDiaSocket* target) 
	: Base(parent)
{
	if (parent->isInput()) {
		setInput(parent);
		setOutput(target);
	} else {
		setInput(target);
		setOutput(parent);
	}

	onUpdateConnectionPosition();
}

axUIDiaConnection::~axUIDiaConnection() {
	setInput(nullptr);
	setOutput(nullptr);
}

void axUIDiaConnection::onRender(axRenderRequest& req) {
	auto inPos  = _inputPos;
	auto outPos = _outputPos;

	if (_drawCurve) {
		req.ui.drawLineStrip(AX_LOC, _curvePoints, _color);
	} else {
		req.ui.drawLine(AX_LOC, inPos, outPos, _color);
	}

	{
		auto rect = axRect2f(inPos, axVec2f(0)).expand(4);
		req.ui.drawRect(AX_LOC, rect, axColor(1,1,0));
	}
}

void axUIDiaConnection::onMouseEvent(axUIMouseEvent& ev) {
	using Type		= axUIMouseEvent::Type;
	using Button	= axUIMouseEvent::Button;
	using Modifier	= axUIMouseEvent::Modifier;

	switch (ev.type) {
		case Type::DragAndDropDragging: {
			if (_isDraggingInput) {
				setInputPos(ev.worldPos - worldPos());
			} else {
				setOutputPos(ev.worldPos - worldPos());
			}
		}break;

		case Type::DragAndDropEnd: {
			if (!inputProp()) {
				AX_LOG_WARNING("inputProp is null");
			} else if (!outputProp()) {
				AX_LOG_WARNING("outputProp is null");
			} else if (!diagram()->onWillAcceptConnection(inputProp(), outputProp())) {
				// not accept the connection
			} else {
				break; // success connected
			}

			ax_delete(this);
		}break;

		case Type::Click: {
			auto* dia = diagram();
			switch (ev.button) {
				case Button::Left: {
					if (ax_enum(ev.modifier).hasFlags(Modifier::Ctrl)) {
						if (isSelected()) {
							dia->removeSelection(this, true);
						}else{
							dia->addSelection(this, true);
						}
					} else {
						dia->setSelection(this, true);
					}
				}break;
			}
		}break;
	}
}

bool axUIDiaConnection::onHitTest(axUIHitTest& ht) {
	if (_input  && _input->hitTestSelf(ht)) return true;
	if (_output && _output->hitTestSelf(ht)) return true;
	return Base::onHitTest(ht);
}

bool axUIDiaConnection::onHitTestSelf(axUIHitTest& ht) {
	auto rc = hitTestRect();
	if (!rc.containsPoint(ht.pos)) return false;

	if (_drawCurve) {
		for (axInt i = 1; i < _curvePoints.size(); i++) {
			axLine2f line(_curvePoints[i-1], _curvePoints[i]);
			auto dis = line.distanceToPoint(ht.pos);
			if (dis < _boundMargin) {
				setNeedToRender();
				return true;
			}
		}

	} else {
		axLine2f line(_inputPos, _outputPos);
		auto dis = line.distanceToPoint(ht.pos);
		if (dis < _boundMargin) {
			setNeedToRender();
			return true;
		}
	}
	
	return false;
}

void axUIDiaConnection::setInput(Socket* c) {
	if (_input) {
		_input->_inputConnections.remove(this);
		AX_ASSERT(_input == nullptr);
	}

	_input = c;

	if (_input) {
		_input->_inputConnections.append(this);
	}
}

void axUIDiaConnection::setOutput(Socket* c) {
	if (_output) {
		_output->_outputConnections.remove(this);
		AX_ASSERT(_output == nullptr);
	}

	_output = c;

	if (_output) {
		_output->_outputConnections.append(this);
	}
}

void axUIDiaConnection::setInputPos(const Vec2& v) {
	_inputPos = v;
	_positionChanged();
}

void axUIDiaConnection::setOutputPos(const Vec2& v) {
	_outputPos = v;
	_positionChanged();
}

void axUIDiaConnection::_positionChanged() {
	auto wpos = worldPos();

	auto i = wpos + _inputPos;
	auto o = wpos + _outputPos;

	BBox2 box;
	box.include(i);
	box.include(o);

	auto rc = box.toRect2() + Margin2(_boundMargin);
	setWorldRect(rc);

	_inputPos  = i - rc.pos;
	_outputPos = o - rc.pos;

	if (_drawCurve) {
		Vec2 tangent;
		tangent.x = ax_clamp(rc.w / 2, 100.0f, 200.0f);
		tangent.y = 0;
		axBezierCurveHelper<Vec2>::evaluateCurve(	_curvePoints, 
													_inputPos,
													_inputPos  - tangent,
													_outputPos + tangent,
													_outputPos);
	}
	setNeedToRender();
}

axUIDiaProp*	axUIDiaConnection::inputProp()		{ return _input  ? _input->prop()  : nullptr; }
axUIDiaProp*	axUIDiaConnection::outputProp()		{ return _output ? _output->prop() : nullptr; }

axStrView		axUIDiaConnection::inputPropName()	{ auto* p = inputProp();  return p ? p->propName() : axStrView(); }
axStrView		axUIDiaConnection::outputPropName()	{ auto* p = outputProp(); return p ? p->propName() : axStrView(); }

void axUIDiaConnection::onUpdateConnectionPosition() {
	auto wpos = worldPos();

	if (_input)		_inputPos  = _input->worldRect().center() - wpos;
	if (_output)	_outputPos = _output->worldRect().center() - wpos;

	_positionChanged();
	setNeedToRender();
}

axUIDiaSocket* axUIDiaConnection::getOppositeDraggingSocket() {
	return _isDraggingInput ? _output : _input;
}

axUIDiaProp* axUIDiaConnection::getOppositeDraggingProp() {
	auto* c = getOppositeDraggingSocket();
	return c ? c->prop() : nullptr;
}

void axUIDiaConnection::_onInputListRemove() {
	_input = nullptr;
}

void axUIDiaConnection::_onOutputListRemove() {
	_output = nullptr;
}

