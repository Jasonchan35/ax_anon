#include "GEO_Diagram.h"
#include "../GEO_System.h"
#include "../../Core/AnonContext.h"

namespace Anon {

GEO_Diagram::GEO_Diagram(DiaGroup* parent)
	: Base(GEO_System::s_instance(), parent)
{
}

void GEO_Diagram::onJsonReader(axJsonReader& rd) {
	rd.beginObject();
	while (!rd.endObject()) {
		readJsonProps(rd);
	}
}

void GEO_Diagram::onJsonWriter(axJsonWriter& wr) const {
	writeJsonProps(wr);
}

bool GEO_Diagram::onReadJsonProp(axJsonReader& rd) {
	bool _ret = Base::onReadJsonProp(rd);
	if (rd.isMember("outputNode")) {
		auto* node = findNode(axNameId::s_make(rd.valueStrView()));
		rd.next();

		_outputNode = ax_type_cast<GEO_Node>(node);
		_ret = true;
	}
	return _ret;
}

void GEO_Diagram::onWriteJsonProp(axJsonWriter& wr) const {
	Base::onWriteJsonProp(wr);
	if (_outputNode) {
		wr.writeMember("outputNode", _outputNode->name());
	}
}

void GEO_Diagram::onCompute() {
	AX_ZoneScoped;

	Base::onCompute();
	if (_outputNode) {
		_outputNode->compute();
	}
}

void GEO_Diagram::setOutputNode(GEO_Node* node) {
	if (node) {
		if (node->diagram() != this) {
			AX_ASSERT(false);
			return;
		}
	}

	if (_outputNode == node)
		return;

	_outputNode = node;

	setComputeNeeded();
}

} //namespace