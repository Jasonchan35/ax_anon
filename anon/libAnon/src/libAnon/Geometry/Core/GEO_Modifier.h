#pragma once
#include "GEO_Node.h"

namespace Anon {

AX_CLASS()
class GEO_Modifier : public GEO_Node {
	AX_CLASS_SPEC(GEO_Modifier, GEO_Node)
	AX_PROP(DontSave)	GEO_GeoData	_input;

	bool _bypass = false;

public:
	const GEO_GeoData&	input() const { return _input; }
	bool bypass() const { return _bypass; }
	void setBypass(bool b);

	virtual void onCompute() override;
	GEO_Modifier(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}	

};

} //namespace