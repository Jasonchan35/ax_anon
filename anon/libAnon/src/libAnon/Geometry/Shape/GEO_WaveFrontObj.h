#pragma once

#include "../Core/GEO_Node.h"

namespace Anon {

AX_CLASS()
class GEO_WaveFrontObj : public GEO_Node {
	AX_CLASS_SPEC(GEO_WaveFrontObj, GEO_Node)

public:
	AX_PROP()	String	_filename;

	GEO_WaveFrontObj(IDiaSystem* system, DiaGroup* parent) : Base(system, parent) {}
	virtual void onCompute() override;

private:
	String	_loadedFilename;
	axEditableMesh	_editableMesh;
};

} //namespace
