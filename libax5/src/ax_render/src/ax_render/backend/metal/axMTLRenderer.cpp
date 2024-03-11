#include "../../ax_render_config.h"

#if AX_USE_METAL

#include "axMTLRenderer.h"
#include "axMTLRenderContext.h"
#include "axMTLGpuBuffer.h"
#include "axMTLStorageBuffer.h"
#include "axMTLStageBuffer.h"
#include "axMTLShader.h"
#include "axMTLMaterial.h"
#include "axMTLTexture.h"
#include "axMTLFence.h"
#include "axMTLRenderRequest.h"

axRenderer_InterfaceFunctions_Impl(axMTL);

axMTLRenderer::axMTLRenderer(CreateDesc& desc)
	: Base(desc)
{
	_mtlDevice = MTLCreateSystemDefaultDevice();
	if (!_mtlDevice) throw axError_Undefined(AX_LOC);

	_adapterInfo.adapterName = axStrView_make([_mtlDevice name]);
	ax_safe_assign(_adapterInfo.memorySize, [_mtlDevice recommendedMaxWorkingSetSize]);

	_adapterInfo.computeShader = true;
	_adapterInfo.shaderFloat64 = false; // Metal doesn't support float64 Mac/iOs

#if AX_OS_OSX
	if ([_mtlDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v1]) {
	}
#endif

// TODO
//	_semaphore = dispatch_semaphore_create(axRender_kFrameAheadCount);
	_semaphore = dispatch_semaphore_create(1);
}

void axMTLRenderer::onGetMemoryInfo(MemoryInfo& info) {
	ax_safe_assign(info.budget, [_mtlDevice recommendedMaxWorkingSetSize]);
	ax_safe_assign(info.used, [_mtlDevice currentAllocatedSize]);
}

axMTLRenderer::~axMTLRenderer() {
	onDestroy();
}

#endif //AX_USE_METAL
