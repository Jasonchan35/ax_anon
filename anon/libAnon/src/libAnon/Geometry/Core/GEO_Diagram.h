#pragma once

#include "../../Diagram/Diagram.h"
#include "GEO_Node.h"

namespace Anon {

class GEO_System;

class GEO_Diagram : public Diagram {
	AX_RTTI_CLASS(GEO_Diagram, Diagram)
public:
	GEO_Diagram(DiaGroup* parent);
	GEO_Diagram(const axTag::ResetT&) : GEO_Diagram(nullptr) {}

	void onJsonReader(axJsonReader& rd);
	void onJsonWriter(axJsonWriter& wr) const override;

	virtual bool onReadJsonProp (axJsonReader& rd) override;
	virtual void onWriteJsonProp(axJsonWriter& wr) const override;
	virtual void onCompute() override;

	GEO_Node*	outputNode() { return _outputNode.ptr(); }

	void setOutputNode(GEO_Node* node);

	void operator=(const axTag::ResetT&) { clear(); }

	void clear() {
		_outputNode = nullptr;
		clearNodes();
	}

private:
	axLink<GEO_Node>	_outputNode;
};

} //namespace
