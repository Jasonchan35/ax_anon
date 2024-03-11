#if AX_USE_VULKAN

#include "axCppVkShaderModule.h"

axCppVkShaderModule::axCppVkShaderModule(axCppVkShaderModule&& r) {
	_h = r._h;
	_dev = r._dev;
	r._h = VK_NULL_HANDLE;
	r._dev = nullptr;
}

void axCppVkShaderModule::destroy() {
	if (_h) {
		vkDestroyShaderModule(*_dev, _h, Util::allocCallbacks());
		_h = VK_NULL_HANDLE;
		_dev = nullptr;
	}
}

void axCppVkShaderModule::create(axCppVkDevice& dev, axByteSpan byteCode) {
	destroy();
	_dev = &dev;

	if (byteCode.size() % 4 != 0) {
		throw axError_Undefined(AX_LOC);
	}

	VkShaderModuleCreateInfo info = {};
	info.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.pNext		= nullptr;
	info.flags		= 0;
	info.codeSize	= ax_safe_static_cast<size_t>(byteCode.size()); 
	info.pCode		= reinterpret_cast<const uint32_t*>(byteCode.data());

	auto err = vkCreateShaderModule(dev, &info, Util::allocCallbacks(), &_h);
	Util::throwIfError(err);
}

void axCppVkShaderModule::createFromFile(axCppVkDevice& dev, axStrView filename) {
	axMemMap byteCode(filename);
	create(dev, byteCode);
}

#endif