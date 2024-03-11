#include "../../ax_render_config.h"

#if AX_USE_METAL

#include "axMTLRenderContext.h"
#include "axMTLCommonBase.h"

axMTLRenderContext::axMTLRenderContext(axMTLRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
	ax_Metal_TODO
}

#endif //AX_USE_METAL
