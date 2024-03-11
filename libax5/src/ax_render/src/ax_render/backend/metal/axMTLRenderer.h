#pragma once

#if AX_USE_METAL

#include "../../axRenderer.h"
#include "../../vertex/axRenderVertexDesc.h"
#include "axMTLCommonBase.h"

class axMTLRenderContext;

class axMTLRenderer : public axRenderer {
	AX_RTTI_CLASS(axMTLRenderer, axRenderer)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using Util = axMTLUtil;

	axMTLRenderer(CreateDesc& desc);
	virtual ~axMTLRenderer();

	id<MTLDevice>	mtlDevice() { return _mtlDevice; }

	dispatch_semaphore_t	_semaphore;

protected:
	axRenderer_InterfaceFunctions(axMTL);

	virtual void onGetMemoryInfo(MemoryInfo& info) override;

friend class axMTLCommonBase;
protected:
	id<MTLDevice>				_mtlDevice = nil;

};

#endif ///AX_USE_METAL
