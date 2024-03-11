#pragma once

#include "SCN_LightBase.h"

namespace Anon {

AX_CLASS()
class SCN_DirectionalLight : public SCN_LightBase {
	AX_CLASS_SPEC(SCN_DirectionalLight, SCN_LightBase)
public:
	SCN_DirectionalLight(SCN_Object* object) : Base(object) {}

};

} //namespace
