#pragma once

#include "../Core/GEO_Modifier.h"

namespace Anon {

AX_CLASS()
class GEO_Transform : public GEO_Modifier {
	AX_CLASS_SPEC(GEO_Transform, GEO_Modifier)

	AX_PROP()	Vec3d	_translate	= Vec3d(0,0,0);
	AX_PROP()	Vec3d	_rotate		= Vec3d(0,0,0);
	AX_PROP()	Vec3d	_scale		= Vec3d(1,1,1);

public:
	GEO_Transform(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;
};

} //namespace
