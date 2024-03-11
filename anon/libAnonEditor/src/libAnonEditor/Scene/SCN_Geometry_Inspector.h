#pragma once

#include "../EditorUI/EdUIObjectInspector.h"
#include "SCN_Component_Inspector.h"

namespace Anon{ namespace Editor {

class SCN_Geometry_Inspector : public SCN_Component_Inspector {
	AX_RTTI_CLASS(SCN_Geometry_Inspector, SCN_Component_Inspector)
public:
	SCN_Geometry_Inspector(CreateDesc& desc);

	virtual void onReloadContent() override;

	void _openDiagram(axUIButtonEvent& ev);

	EdUIButton	_diagramButton;
};

}} //namespace