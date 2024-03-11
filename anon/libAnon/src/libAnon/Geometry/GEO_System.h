#pragma once

#include "../Diagram/DiaSystem.h"
#include "Core/GEO_Diagram.h"
#include "Core/GEO_Node.h"

namespace Anon {

class DiaSystem;

class GEO_System : public IDiaSystem {
public:
	GEO_System();

	void create(DiaSystem* diaSystem);

	static GEO_System* s_instance();

	template<class T> void addNodeType() { _nodeFactory.addEntry<T>(); }
	virtual DiaNode* newNode(const axSourceLoc& loc, axStrView type, IDiaSystem* system, DiaGroup* parent) override;

	axSpan<const axType*>	nodeTypes()	{ return _nodeFactory.entries(); }

private:
	IDiaSystem::NodeFactory _nodeFactory;
	DiaSystem*	_diaSystem = nullptr;
};

}