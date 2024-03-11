#pragma once

#include "axUIDiaItem.h"

class axUIDiaInputListEvent;
class axUIDiaOutputListEvent;

class axUIDiaConnection 
	: public axUIDiaItem
	, public axDListNode<axUIDiaConnection, axUIDiaInputListEvent>
	, public axDListNode<axUIDiaConnection, axUIDiaOutputListEvent> 
{
	AX_RTTI_CLASS(axUIDiaConnection, axUIDiaItem)
	using Socket = axUIDiaSocket;
	using Prop = axUIDiaProp;
public:
	using InputListNode  = axDListNode<axUIDiaConnection, axUIDiaInputListEvent>;
	using OutputListNode = axDListNode<axUIDiaConnection, axUIDiaOutputListEvent>;

	axUIDiaConnection(axUIDiaSocket* parent, axUIDiaSocket* target);
	virtual ~axUIDiaConnection();

	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

	virtual bool onHitTest(axUIHitTest& ht) override;
	virtual bool onHitTestSelf(axUIHitTest& ht) override;

	void setInput (Socket* c);
	void setOutput(Socket* c);

	Socket*	input()				{ return _input;  };
	Socket*	output()			{ return _output; }

	Prop*		inputProp();
	Prop*		outputProp();

	axStrView	inputPropName();
	axStrView	outputPropName();

	void setInputPos (const Vec2& v);
	void setOutputPos(const Vec2& v);

	void onUpdateConnectionPosition();

	void startDragging(bool isInput) { _isDraggingInput = isInput; }
	bool isDraggingInput() const { return _isDraggingInput; } 

	Socket*	getOppositeDraggingSocket();
	Prop*	getOppositeDraggingProp();

	void _onInputListRemove();
	void _onOutputListRemove();

	void setColor(const axColor& c) { _color = c; }

private:
	void _positionChanged();

	Socket*	_input  = nullptr;
	Socket*	_output = nullptr;

	const float _boundMargin = 8;

	Vec2	_inputPos {0,0};
	Vec2	_outputPos{0,0};
	bool	_isDraggingInput = false;

	axColor	_color = axColor::kYellow();
	bool	_drawCurve = true;

	axArray<axVec2f> _curvePoints;
};

class axUIDiaInputListEvent : public axDListEventBase {
public:
	static void onRemove(axUIDiaConnection* p) { p->_onInputListRemove(); }
};

class axUIDiaOutputListEvent : public axDListEventBase {
public:
	static void onRemove(axUIDiaConnection* p) { p->_onOutputListRemove(); }
};