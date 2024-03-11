#pragma once

namespace Anon { namespace Editor {

class EdUIDiagram;
class EdUIDiaGroup : public axUIDiaGroup {
	AX_RTTI_CLASS(EdUIDiaGroup, axUIDiaGroup)
public:
	struct CreateDesc {
		axUIDiaGroup*	parent   = nullptr;
		DiaGroup*		diaGroup = nullptr;
	};

	EdUIDiaGroup(CreateDesc& desc);

	DiaGroup*		diaGroup	()	{ return _diaGroup; }
	EdUIDiagram* diagram();

private:
	axLink<DiaGroup>	_diaGroup;
};

}}