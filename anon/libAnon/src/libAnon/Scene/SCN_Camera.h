#pragma once

#include "SCN_Component.h"

namespace Anon {

AX_CLASS()
class SCN_Camera : public SCN_Component {
	AX_CLASS_SPEC(SCN_Camera, SCN_Component)
public:
	SCN_Camera(SCN_Object* object) : Base(object) {}

};

} //namespace
