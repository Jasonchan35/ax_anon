#pragma once

#include "Diagram.h"
#include "DiaNode.h"

namespace Anon {

class IDiaSystem : public axNonCopyable {
public:
	using NodeFactory = axTypeFactoryT<DiaNode, IDiaSystem*, DiaGroup*>;
	virtual DiaNode* newNode(const axSourceLoc& loc, axStrView type, IDiaSystem* system, DiaGroup* parent) = 0;
	virtual ~IDiaSystem() = default;
};

class DiaSystem : public IDiaSystem {
public:
	static DiaSystem* s_instance();

	DiaSystem();

	void create();

	template<class T> void addNodeType() { _nodeFactory.addEntry<T>(); }
	virtual DiaNode* newNode(const axSourceLoc& loc, axStrView type, IDiaSystem* system, DiaGroup* parent) override;

private:
	IDiaSystem::NodeFactory _nodeFactory;
};

}
