#pragma once

#include "../EditorUI/EdUIObjectInspector.h"

namespace Anon{ namespace Editor {

class SCN_Component_Inspector : public EdUIObjectInspector {
	AX_RTTI_CLASS(SCN_Component_Inspector, EdUIObjectInspector)
public:
	SCN_Component_Inspector(CreateDesc& desc);

};

}} //namespace