#include "../../ax_render_config.h"

#if AX_USE_VULKAN

#include "axVkRenderContext.h"
#include "axVkCommonBase.h"

axVkRenderContext::axVkRenderContext(axVkRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
	ax_Vulkan_TODO
}

#endif //AX_USE_VULKAN
