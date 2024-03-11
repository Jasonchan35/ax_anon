#pragma once

#include "../Core/GEO_Modifier.h"

namespace Anon {

AX_CLASS()
class GEO_Bend : public GEO_Modifier {
	AX_CLASS_SPEC(GEO_Bend, GEO_Modifier)

	AX_PROP(Slider(-180, 180)) double	_angle;

	AX_PROP() Vec3d		_origin 	= Vec3d(0,0,0);
	AX_PROP() Vec3d		_direction 	= Vec3d(0,0,1);
	AX_PROP() Vec3d		_upVector 	= Vec3d(0,1,0);
	AX_PROP() double	_length 	= 1;

	virtual void onCompute() override;

public:
	GEO_Bend(IDiaSystem* system, DiaGroup* parent);
};

} //namespace
