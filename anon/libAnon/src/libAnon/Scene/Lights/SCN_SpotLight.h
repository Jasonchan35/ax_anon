#pragma once

#include "SCN_LightBase.h"

namespace Anon {

AX_CLASS()
class SCN_SpotLight : public SCN_LightBase {
	AX_CLASS_SPEC(SCN_SpotLight, SCN_LightBase)
public:
	SCN_SpotLight(SCN_Object* object) : Base(object) {}

};

} //namespace
