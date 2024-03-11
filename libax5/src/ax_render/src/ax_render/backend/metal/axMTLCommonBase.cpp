#if AX_USE_METAL

#include "axMTLCommonBase.h"
#include "axMTLRenderer.h"
#include "axMTLShader.h"

axMTLRenderer* axMTLUtil::renderer() { return axMTLRenderer::s_instance(); }
id<MTLDevice> axMTLUtil::mtlDevice() { return axMTLRenderer::s_instance()->mtlDevice(); }

#endif //AX_USE_METAL
