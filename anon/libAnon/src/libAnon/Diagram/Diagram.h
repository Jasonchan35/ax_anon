#pragma once

#include "DiaGroup.h"

namespace Anon {

class Diagram : public DiaGroup {
	AX_RTTI_CLASS(Diagram, DiaGroup);
public:
	void setOwner(AnonObject* v) { _owner = v; }
	AnonObject* owner() { return _owner; }

	virtual void onSetComputeNeeded() override;

	Diagram(IDiaSystem* system, DiaGroup* parent);

	virtual bool onReadJsonProp(axJsonReader& rd) override;
	virtual void onWriteJsonProp(axJsonWriter& wr) const override;

	virtual DiaConnection* onAcceptConnection(axStrView srcNode, axStrView srcField, axStrView dstNode, axStrView dstField);

private:
	axLink<AnonObject>	_owner;
};

} //namespace
