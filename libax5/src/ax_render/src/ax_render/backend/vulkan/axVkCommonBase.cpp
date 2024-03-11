#if AX_USE_VULKAN

#include "axVkCommonBase.h"
#include "axVkRenderer.h"
#include "axVkRenderContext_Base.h"
#include "axVkShader.h"
#include "axCppVk/axCppVkSurfaceKHR.h"

bool axVkUtil::formatHasDepth(VkFormat f) {
	switch (f) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
	}
	return false;}

bool axVkUtil::formatHasStencil(VkFormat f) {
	switch (f) {
		case VK_FORMAT_S8_UINT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
	}
	return false;
}

axVkRenderer* axVkUtil::renderer() {
	return axVkRenderer::s_instance();
}

axCppVkDevice& axVkUtil::rootDevice() {
	return renderer()->device();
}

void* axVkAllocatorCallbacks::s_alloc(void* pUserData, size_t size, size_t alignment,
										VkSystemAllocationScope allocationScope) {
	return ax_malloc(axByte, ax_safe_static_cast<axInt>(size), ax_safe_static_cast<axInt>(alignment));
}

void* axVkAllocatorCallbacks::s_realloc(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) {
	return ax_malloc(axByte, ax_safe_static_cast<axInt>(size), ax_safe_static_cast<axInt>(alignment));
}

void  axVkAllocatorCallbacks::s_free(void* pUserData, void* pMemory) {
	if (pMemory) {
		ax_free(pMemory);
	}
}

void  axVkAllocatorCallbacks::s_allocNotification(void* pUserData, size_t size,
	VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {
	
}

void  axVkAllocatorCallbacks::s_freeNotification(void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {

}

axVkAllocatorCallbacks::axVkAllocatorCallbacks() {
	pfnAllocation = s_alloc;
	pfnReallocation = s_realloc;
	pfnFree = s_free;
	pfnInternalAllocation = s_allocNotification;
	pfnInternalFree = s_freeNotification;
}

void axVkQueueFamilyIndices::create(axCppVkSurfaceKHR& surface) {
	_graphIndex   = kInvalidIndex;
	_presentIndex = kInvalidIndex;

	if (!surface.physicalDevice().findQueueFamilyIndex(_graphIndex, VK_QUEUE_GRAPHICS_BIT)) {
		throw axError_Undefined(AX_LOC);
	}

	if (surface.checkQueueFamilyIndexSupportPresent(_graphIndex)) {
		_presentIndex = _graphIndex;
	} else {
		if (!surface.findQueueFamilyIndexSupportPresent(_presentIndex)) {
			throw axError_Undefined(AX_LOC);
		}
	}
}

#endif //AX_USE_VULKAN
