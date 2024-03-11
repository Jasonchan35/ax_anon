#pragma once

#include "../Core/GEO_Node.h"

namespace Anon {

AX_CLASS()
class GEO_Cube : public GEO_Node {
	AX_CLASS_SPEC(GEO_Cube, GEO_Node)

public:
	AX_PROP()	Vec3d	center 	= Vec3d(0);
	AX_PROP()	Vec3d	size 	= Vec3d(1,1,1);

	GEO_Cube(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;
};

} //namespace
