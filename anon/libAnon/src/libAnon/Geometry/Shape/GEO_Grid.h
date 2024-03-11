#pragma once

#include "../Core/GEO_Node.h"

namespace Anon {

AX_CLASS()
class GEO_Grid : public GEO_Node {
	AX_CLASS_SPEC(GEO_Grid, GEO_Node)

public:					
	AX_PROP()	Vec3d	_center	= Vec3d(0,  0,  0);
	AX_PROP()	Vec2d	_size	= Vec2d(10, 10);
	AX_PROP()	Vec2i	_div	= Vec2i(10, 10);

	GEO_Grid(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;
};

} //namespace
