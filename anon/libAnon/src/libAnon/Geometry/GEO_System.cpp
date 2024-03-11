#include "GEO_System.h"

namespace Anon {

GEO_System* GEO_System_instance;

GEO_System* GEO_System::s_instance() { return GEO_System_instance; }

GEO_System::GEO_System() {
	AX_ASSERT(GEO_System_instance == nullptr);
	GEO_System_instance = this;
}

void GEO_System::create(DiaSystem* diaSystem) {
	_diaSystem = diaSystem;
}

DiaNode* GEO_System::newNode(const axSourceLoc& loc, axStrView type, IDiaSystem* system, DiaGroup* parent) {
	auto* allocator = ax_scope_allocator();
	auto node = _nodeFactory.createByName(loc, allocator, type, system, parent);
	if (node) {
		node->initProps();
		return node.detach();
	}
	return _diaSystem->newNode(loc, type, system, parent);
}

}