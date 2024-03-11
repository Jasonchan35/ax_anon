#pragma once

#if AX_USE_VULKAN

#include "../../axRenderer.h"
#include "../../vertex/axRenderVertexDesc.h"
#include "axVkCommonBase.h"
#include "axCppVk/axCppVk.h"
#include "axVkGpuBuffer.h"
#include "axVkMaterialPass.h"
#include "axVkSwapChain.h"

class axVkRenderer : public axRenderer {
	AX_RTTI_CLASS(axVkRenderer, axRenderer)
public:
	ax_DOWNCAST_GET_INSTANCE()

	using Util = axVkUtil;

	axVkRenderer(CreateDesc& desc);
	virtual ~axVkRenderer();

	axRenderer_InterfaceFunctions(axVk);

	axCppVkPhysicalDeviceList&	physicalDevice()		{ return _physicalDeviceList; }
	axCppVkPhysicalDevice*		defaultPhysicalDevice() { return _physicalDeviceList.defaultDevice(); }

	axCppVkDevice&				device()					{ return _device; }
	const axVkQueueFamilyIndices&	queueFamilyIndices()	{ return _queueFamilyIndices; }

	axCppVkRenderPass&			renderPass() { return _renderPass; }

	const VkSurfaceFormatKHR&	colorFormat() const			{ return _colorFormat; }
	const VkFormat&				depthStencilFormat() const	{ return _depthStencilFormat; }

	virtual void onGetMemoryInfo(MemoryInfo& info) override;

	axCppVkInstance&	inst() { return _inst; }

	template<class FUNC> inline
	void getProc(FUNC& outFunc, axStrLiteral name) {
		outFunc = reinterpret_cast<FUNC>(vkGetInstanceProcAddr(_inst, name.c_str()));
	}

	axVkProcList*	procList() { return &_procList; }

	bool enableDebugReport() const { return _enableDebugReport; }

friend class axVkCommonBase;
private:

	void _setupEnvVar();

	axVkProcList _procList;

	static VkBool32 VKAPI_PTR s_debugReport(
		VkDebugReportFlagsEXT      flags,
		VkDebugReportObjectTypeEXT objectType,
		uint64_t                   object,
		size_t                     location,
		int32_t                    messageCode,
		const char*                pLayerPrefix,
		const char*                pMessage,
		void*                      pUserData);

	VkSurfaceFormatKHR	_colorFormat;
	VkFormat			_depthStencilFormat;

	axCppVkInstance				_inst;
	axCppVkPhysicalDeviceList	_physicalDeviceList;
	axCppVkDevice				_device;
	axCppVkRenderPass			_renderPass;

	VkDebugReportCallbackEXT	_debugReportCallback = nullptr;
	//axSPtr<axVkRenderContext_Base>	_rootContext;
	axVkQueueFamilyIndices		_queueFamilyIndices;

	bool _enableDebugReport : 1;

#if AX_OS_OSX
	axNSObj<MTKView>	_nsView;
#endif
};

#endif ///AX_USE_VULKAN
