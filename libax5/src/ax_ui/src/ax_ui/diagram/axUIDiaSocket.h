#pragma once

#include "axUIDiaConnection.h"

class axUIDiaSocket : public axUIView {
	AX_RTTI_CLASS(axUIDiaSocket, axUIView)
public:
	using Node = axUIDiaNode;
	using Prop = axUIDiaProp;
	using Diagram = axUIDiagram;
	using Connection = axUIDiaConnection;

	axUIDiaSocket(axUIView* parent, Prop* prop,  bool isInput);
	virtual ~axUIDiaSocket();

	virtual void onRender(axRenderRequest& req) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;
	virtual void onUpdateLayout() override;

	bool isInput() const { return _isInput; }
	Prop*		prop() { return _prop; }
	Node*		node();
	Diagram*	diagram();

	void onUpdateConnectionPosition();

	using InputConnList  = axDList<Connection, ax_no_delete, Connection::InputListNode>;
	using OutputConnList = axDList<Connection, ax_no_delete, Connection::OutputListNode>;

	bool hasConnection() const;

friend class axUIDiaConnection;
private:
	axSPtr<axTexture2D>	_texture;
	InputConnList		_inputConnections;
	OutputConnList		_outputConnections;
	Prop* _prop = nullptr;
	bool _isInput : 1;
};