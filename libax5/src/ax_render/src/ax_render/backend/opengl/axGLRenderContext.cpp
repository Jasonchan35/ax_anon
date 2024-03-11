#include "../../ax_render_config.h"

#if AX_USE_OPENGL

#include "axGLRenderContext.h"
#include "axGLCommonBase.h"

axGLRenderContext::axGLRenderContext(axGLRenderer* renderer, CreateDesc& desc) 
	: Base(renderer, desc)
{
	ScopedContext scopedContext(this);
	Util::checkGLError(AX_LOC);
	glEnable(GL_SCISSOR_TEST);

	AX_TracyGpuContext;
}

#endif //AX_USE_OPENGL
