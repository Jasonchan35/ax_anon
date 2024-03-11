#pragma once

#include "SCN_Component.h"
#include "../Geometry/Core/GeoAsset.h"

namespace Anon {

AX_CLASS()
class SCN_Geometry : public SCN_RendererBase {
	AX_CLASS_SPEC(SCN_Geometry, SCN_RendererBase)
	AX_PROP() GEO_Diagram _diagram {axTag::Reset};
public:
	SCN_Geometry(SCN_Object* object);

	virtual void onRender(axRenderRequest& req) override;

	virtual void onCompute() override;
	virtual void onSetComputeNeeded() override;

	GEO_Diagram*	diagram() { return &_diagram; }

private:
	axLazyUPtr<axRenderMesh>	_renderMesh;
	axLazyUPtr<axRenderMesh>	_renderIds;
	axLazyUPtr<axRenderMesh>	_renderNormals;
	float _renderNormalLength = 0;
};

} //namespace
