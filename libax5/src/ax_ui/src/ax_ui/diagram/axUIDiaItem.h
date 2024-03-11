#pragma once

#include "../base/axUIView.h"
#include "../base/axUISelectionList.h"

class axUIDiaNode;
class axUIDiaGroup;
class axUIDiaProp;
class axUIDiaConnection;
class axUIDiaSocket;
class axUIDiagram;

class axUIDiaItem : public axUIView, public axUISelectionListNode<axUIDiaItem> {
	AX_RTTI_CLASS(axUIDiaItem, axUIView)
public:
	const float kSnap = 20.0f;

	axUIDiaItem(axUIView* parent);
	axUIDiagram* diagram() { return _diagram; }

private:
	axUIDiagram*	_diagram = nullptr;
};