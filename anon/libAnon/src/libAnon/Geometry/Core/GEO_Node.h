#pragma once
#include "GEO_GeoData.h"
#include "../../Diagram/DiaNode.h"

namespace Anon {

class GEO_Diagram;
using GEO_Group = DiaGroup;

AX_CLASS()
class GEO_Node : public DiaNode {
	AX_CLASS_SPEC(GEO_Node, DiaNode)

protected:
	AX_PROP(DontSave, Output)	GEO_GeoData	_output;

	virtual void onGetObjectInfo(axIString& info) override;
public:
	const GEO_GeoData& output() const { return _output; }

	GEO_Node(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	
	virtual void onCompute() override;
	virtual void onSetComputeNeeded() override;

	GEO_Diagram*	diagram();

	void setAsOutputNode();
};

} //namespace