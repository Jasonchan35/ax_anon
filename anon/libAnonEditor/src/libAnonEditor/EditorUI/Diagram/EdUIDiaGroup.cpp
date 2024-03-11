#include "EdUIDiaGroup.h"
#include "EdUIDiagram.h"

namespace Anon { namespace Editor {

EdUIDiagram* EdUIDiaGroup::diagram() {
	return ax_type_cast<EdUIDiagram>(Base::diagram());
}

EdUIDiaGroup::EdUIDiaGroup(CreateDesc& desc) 
	: Base(desc.parent)
{
	_diaGroup = desc.diaGroup;
}

}}