#pragma once

#include "SCN_LightBase.h"

namespace Anon {

AX_CLASS()
class SCN_PointLight : public SCN_LightBase {
	AX_CLASS_SPEC(SCN_PointLight, SCN_LightBase)
public:
	SCN_PointLight(SCN_Object* object) : Base(object) {}

	AX_PROP()
	float	lightPower = 100;

	AX_PROP()
	ColorRGB	lightColor {1,1,1};
};

} //namespace
