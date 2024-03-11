#pragma once

#include "../Core/GEO_Modifier.h"

namespace Anon {

AX_CLASS()
class GEO_Subdiv : public GEO_Modifier {
	AX_CLASS_SPEC(GEO_Subdiv, GEO_Modifier)

	AX_PROP(Slider(1,5)) axInt	_level = 1;

	virtual void onCompute() override;

public:
	GEO_Subdiv(IDiaSystem* system, DiaGroup* parent);
};

} //namespace
