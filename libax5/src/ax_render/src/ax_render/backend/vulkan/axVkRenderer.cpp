#include "../../ax_render_config.h"

#if AX_USE_VULKAN

#include "axVkRenderer.h"
#include "axVkRenderContext.h"
#include "axVkStorageBuffer.h"
#include "axVkShader.h"
#include "axVkMaterial.h"
#include "axVkMaterialPass.h"
#include "axVkTexture.h"
#include "axVkFence.h"
#include "axVkStageBuffer.h"
#include "axVkRenderRequest.h"

axRenderer_InterfaceFunctions_Impl(axVk);

axVkRenderer::axVkRenderer(CreateDesc& desc)
	: Base(desc)
	, _enableDebugReport(true)
{
#if !_DEBUG
	_enableDebugReport = false;
#endif

	_setupEnvVar();

	_colorFormat.format		= VK_FORMAT_B8G8R8A8_UNORM;
	_colorFormat.colorSpace	= VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	_depthStencilFormat		= VK_FORMAT_D32_SFLOAT;

	{
		axCppVkInstanceCreateInfo info;
		info.logInfo();

		{
			// extensions
			if (!info.enableExtension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME))
				_enableDebugReport = false;

			if (!info.enableExtension(VK_KHR_SURFACE_EXTENSION_NAME				)) throw axError_Undefined(AX_LOC);

			#if AX_OS_WINDOWS
				if (!info.enableExtension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME	)) throw axError_Undefined(AX_LOC);
			#elif AX_USE_X11
				if (!info.enableExtension(VK_KHR_XLIB_SURFACE_EXTENSION_NAME	)) throw axError_Undefined(AX_LOC);
			#elif AX_OS_OSX
				if (!info.enableExtension(VK_MVK_MACOS_SURFACE_EXTENSION_NAME	)) throw axError_Undefined(AX_LOC);
			//	if (!info.enableExtension(VK_EXT_METAL_SURFACE_EXTENSION_NAME	)) throw axError_Undefined(AX_LOC);
			#endif
		}

		{
			// layers
			#if !AX_OS_OSX
				if (!info.enableLayer("VK_LAYER_KHRONOS_validation")) { throw axError_Undefined(AX_LOC); }
			#endif
		}

		_inst.create(info);
	}

	_procList.init(this);

	if (_enableDebugReport) {
		VkDebugReportCallbackCreateInfoEXT info = {};
		info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		info.pNext = nullptr;
		info.flags = 0
			//| VK_DEBUG_REPORT_DEBUG_BIT_EXT
			//| VK_DEBUG_REPORT_INFORMATION_BIT_EXT
			| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
			| VK_DEBUG_REPORT_WARNING_BIT_EXT
			| VK_DEBUG_REPORT_ERROR_BIT_EXT;
		info.pfnCallback = s_debugReport;
		info.pUserData = this;

		_procList.vkCreateDebugReportCallbackEXT(_inst, &info, Util::allocCallbacks(), &_debugReportCallback);
	}

	_physicalDeviceList.create(_inst);

	//{
	//	axRenderContext_CreateDesc desc;
	//	auto rootContext = createRenderContext(desc);
	//	_rootContext.ref(ax_type_cast<axVkRenderContext>(rootContext.ptr()));
	//	if (!_rootContext) {
	//		throw axError_Undefined(AX_LOC);
	//	}
	//}

	{
		auto* phyDev = defaultPhysicalDevice();
		axCppVkSurfaceKHR surface;
		#if AX_OS_WINDOWS
			surface.create_Win32(*phyDev, nullptr, nullptr);
		#elif AX_OS_LINUX
			auto* app = axNativeUIApp::s_instance();
			surface.create_x11(*phyDev, app->xdisplay(), 0);
		#else
			_nsView = [[MTKView alloc] initWithFrame:axRect2f(0,0,16, 16)];
			surface.create_macOS(*phyDev, _nsView);
		#endif

		_adapterInfo.adapterName = phyDev->deviceName();
		_adapterInfo.memorySize  = phyDev->deviceMemorySize();

		auto& feature = phyDev->features();
		_adapterInfo.shaderFloat64 = feature.shaderFloat64;

		{
			uint32_t computeQueue = 0;
			if (phyDev->findComputeQueueFamilyIndex(computeQueue)) {
				_adapterInfo.computeShader = true;
			}
		}

		_queueFamilyIndices.create(surface);
		_device.create(*phyDev, _queueFamilyIndices);

		_renderPass.create(_device, _colorFormat.format, _depthStencilFormat, true);
	}
}

axVkRenderer::~axVkRenderer() {
	if (_enableDebugReport) {
		_procList.vkDestroyDebugReportCallbackEXT(_inst, _debugReportCallback, Util::allocCallbacks());
	}
	onDestroy();
}

void axVkRenderer::onGetMemoryInfo(MemoryInfo& info) {
	if (auto* phyDev = defaultPhysicalDevice()) {
		phyDev->getMemoryInfo(info);
	}
}

void axVkRenderer::_setupEnvVar() {
#ifdef AX_OS_OSX
//	axEnvVar::setValue("VULKAN_SDK", "/Applications/VulkanSDK/vulkansdk-macos-1.2.141.2/macOS");
//	axEnvVar::setValue("PATH","$VULKAN_SDK/bin:$PATH");

	axStrView icdFile   = "/usr/local/share/vulkan/icd.d/MoltenVK_icd.json";
	if (!axFile::exists(icdFile)) {
		AX_LOG("Vulkan SDK file not found = {?}", icdFile);
		throw axError_Undefined(AX_LOC);
	}

	axStrView layerPath = "/usr/local/share/vulkan/explicit_layer.d";
	if (!axDirectory::exists(layerPath)) {
		AX_LOG("Vulkan SDK path not found = {?}", layerPath);
		throw axError_Undefined(AX_LOC);
	}

	axEnvVar::setValue("VK_ICD_FILENAMES",		icdFile);
	axEnvVar::setValue("VK_LAYER_PATH",			layerPath);
#endif
}

VkBool32 VKAPI_PTR axVkRenderer::s_debugReport(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objectType,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData) 
{
	AX_LOG("Vulkan error {?}: {?} - location={?}", messageCode, axStrView_c_str(pMessage), location);
	return true;
}

void axVkProcList::init(axVkRenderer* renderer) {
	#define AX_MACRO_OP(E) \
		renderer->getProc(E, #E); \
		if (!E) { \
			AX_LOG("Error: cannot find vulkan proc {?}", #E); \
			throw axError_Undefined(AX_LOC); \
		} \
	//------
		if (renderer->enableDebugReport()) {
			AX_MACRO_OP(vkDestroyDebugReportCallbackEXT);
		}

		axVkProcList_EnumList(AX_MACRO_OP);
	#undef AX_MACRO_OP
}

#endif //AX_USE_VULKAN
