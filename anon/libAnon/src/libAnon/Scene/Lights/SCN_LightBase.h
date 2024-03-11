#pragma once

#include "../SCN_Component.h"

namespace Anon {

AX_CLASS()
class SCN_LightBase : public SCN_Component, public axDListNode<SCN_LightBase> {
	AX_CLASS_SPEC(SCN_LightBase, SCN_Component)
public:
	SCN_LightBase(SCN_Object* object);
	virtual ~SCN_LightBase();
};

} //namespace
