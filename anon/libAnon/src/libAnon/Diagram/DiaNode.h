#pragma once

#include "DiaConnection.h"
#include "../Core/AnonObject.h"

namespace Anon {

class IDiaSystem;
class DiaGroup;
class Diagram;

class DiaNode : public AnonObject, public axDListNode<DiaNode> {
	AX_RTTI_CLASS(DiaNode, AnonObject);
public:
	using ListNode = axDListNode<DiaNode>;

	DiaNode(IDiaSystem* system, DiaGroup* parent);
	virtual ~DiaNode();

	static DiaNode* s_newNodeByTypeName(DiaGroup* parent, axStrView typeName);
	static DiaNode* s_newNodeFromJson(DiaGroup* parent, axJsonReader& rd);

	virtual void onJsonWriter(axJsonWriter& wr) const;

	void setDiaPos(const Vec2f& v)	{ _diaPos = v; }
	const Vec2f& diaPos() const		{ return _diaPos; }

	IDiaSystem*	system() { return _system; }

	DiaConnectionNode	connNode;
	Diagram*	diagram() { return _diagram; }

	virtual void onCompute() override;
	virtual void onSetComputeNeeded() override;

friend class DiaGroup;
protected:
	Diagram*	_diagram = nullptr;
private:
	Vec2f		_diaPos {0,0};
	IDiaSystem*	_system = nullptr;
};

} //namespace