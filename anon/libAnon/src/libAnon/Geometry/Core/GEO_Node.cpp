#include "GEO_Node.h"
#include "GEO_Diagram.h"

namespace Anon {

GEO_Diagram* GEO_Node::diagram() {
	return ax_type_cast<GEO_Diagram>(Base::diagram());
}

void GEO_Node::setAsOutputNode() {
	if (auto* dia = diagram()) {
		dia->setOutputNode(this);
	}	
}

void GEO_Node::onSetComputeNeeded() {
	Base::onSetComputeNeeded();
	if (auto* dia = diagram()) {
		if (dia->outputNode() == this) {
			dia->setComputeNeeded();
		}
	}
}

void GEO_Node::onGetObjectInfo(axIString& info) {
	compute();
	_output.onGetObjectInfo(info);
}

void GEO_Node::onCompute() {
	Base::onCompute();
}

} //namespace