#pragma once

#include "../../Asset/Asset.h"
#include "GEO_Diagram.h"

namespace Anon {

class GeoAsset : public Asset {
	AX_RTTI_CLASS(GeoAsset, Asset);
public:
	GeoAsset();

	GEO_Diagram* diagram() { return &_diagram; }

friend class AnonContext;
protected:
	void onLoadFile(axMemMap& mem);

private:
	GEO_Diagram	_diagram;
};

} //namespace