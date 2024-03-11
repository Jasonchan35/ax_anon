#pragma once

#include "axUIDiaSocket.h"
#include "../layout/axUIBoxLayout.h"
#include "../widget/axUILabel.h"

class axUIDiaNode;
class axUIDiaProp;

class axUIDiaPropGroup : public axUIVBoxView {
	AX_RTTI_CLASS(axUIDiaPropGroup, axUIVBoxView);
public:
	axUIDiaPropGroup(axUIView* parent);

	axUIDiaProp* findProp(axStrView name);

	void onUpdateConnectionPosition();
};

class axUIDiaInputPropGroup : public axUIDiaPropGroup {
	AX_RTTI_CLASS(axUIDiaInputPropGroup, axUIDiaPropGroup);
public:
	axUIDiaInputPropGroup(axUIView* parent) : Base(parent) {}
};

class axUIDiaOutputPropGroup : public axUIDiaPropGroup {
	AX_RTTI_CLASS(axUIDiaOutputPropGroup, axUIDiaPropGroup);
public:
	axUIDiaOutputPropGroup(axUIView* parent) : Base(parent) {}
};

class axUIDiaProp : public axUIView {
	AX_RTTI_CLASS(axUIDiaProp, axUIView)

	using Node       = axUIDiaNode;
	using Prop       = axUIDiaProp;
	using GroupNode  = axUIDiaGroup;
	using Connection = axUIDiaConnection;
	using Socket	 = axUIDiaSocket;

public:
	axUIDiaProp(axUIView* parent, Node* node, axStrView name, axStrView displayName, bool isInput);

	virtual void onUpdateLayout() override;

	virtual void onFormat(axFormat& f) const override;

	bool isInput() const;
	void onUpdateConnectionPosition();

	void		setPropName(axStrView propName);
	axStrView	propName() const { return _propName; }

	void		setDisplayName(axStrView s);
	axStrView	displayName() const { return _displayName; }

	Node*		node() { return _node; }
	axStrView	nodeName() const;

	Socket&		socket() { return _socket; }

private:
	axString		_propName;
	axString		_displayName;
	axUILabel		_nameBox;
	Socket			_socket;
	Node*			_node = nullptr;
};

