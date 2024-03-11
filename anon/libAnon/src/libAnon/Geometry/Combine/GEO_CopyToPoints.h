#pragma once
#include "../Core/GEO_Modifier.h"

namespace Anon {

AX_CLASS()
class GEO_CopyToPoints : public GEO_Modifier {
	AX_CLASS_SPEC(GEO_CopyToPoints, GEO_Modifier)

	AX_PROP()	GEO_GeoData	_inputPoints;

public:
	GEO_CopyToPoints(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;

};

}