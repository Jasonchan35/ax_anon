#pragma once

#if AX_USE_VULKAN

//
//  axVkRenderContext_Base.h
//  libax_core
//
//  Created by Jason on 2017-02-05.
//
//

#include "axVkSwapChain.h"
#include "../../axRenderContext.h"

class axVkRenderContext_Base : public axRenderContext {
	AX_RTTI_CLASS(axVkRenderContext_Base, axRenderContext)
public:
	using Util = axVkUtil;

	axVkRenderContext_Base(axVkRenderer* renderer, CreateDesc& desc);

	axVkRenderer* renderer() { return ax_type_cast_debug<axVkRenderer>(Base::renderer()); }

			void cmdSwapBuffers(axVkCommandDispatcher& dispatcher);

	axCppVkSurfaceKHR&		surface			() { return _surface; }

protected:
			void _createDevice();

	virtual void onDispatchCommands(axRenderRequest& req) override;

	virtual void onPreDestroy() override;

	virtual void onSetNativeViewRect(const axRect2f& rect) override;

	virtual void onCreateVkSurface(axCppVkSurfaceKHR& surface) = 0;

private:
	axVkProcList*				_procList = nullptr;
	axCppVkSurfaceKHR			_surface;
	VkQueue						_graphQueue   = nullptr;
	VkQueue						_presentQueue = nullptr;

	struct MData {
		axSPtr<axVkSwapChain>	swapChain;
		bool swapChainDirty = false;
	};
	axSpinLockProtected<MData>	_mdata;
	bool _lastVSync : 1;
};

#endif //AX_USE_VULKAN
