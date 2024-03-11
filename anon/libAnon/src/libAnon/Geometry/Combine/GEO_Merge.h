#pragma once

#include "../Core/GEO_Modifier.h"

namespace Anon {

AX_CLASS()
class GEO_Merge : public GEO_Modifier {
	AX_CLASS_SPEC(GEO_Merge, GEO_Modifier)

public:
	AX_PROP(DontSave)	GEO_GeoData		_input2;

	GEO_Merge(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;

};

}