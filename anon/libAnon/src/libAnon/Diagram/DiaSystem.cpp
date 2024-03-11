#include "DiaSystem.h"

namespace Anon {

DiaSystem*	DiaSystem_instance;

DiaSystem* DiaSystem::s_instance() { return DiaSystem_instance; }

DiaSystem::DiaSystem() {
	AX_ASSERT(DiaSystem_instance == nullptr);
	DiaSystem_instance = this;
}

void DiaSystem::create() {
}

DiaNode* DiaSystem::newNode(const axSourceLoc& loc, axStrView type, IDiaSystem* system, DiaGroup* parent) {
	if (auto node = _nodeFactory.createByName(loc, ax_scope_allocator(), type, system, parent)) {
		node->initProps();
		return node.detach();
	}
	return new(loc, ax_scope_allocator()) DiaNode(system, parent);
}

}