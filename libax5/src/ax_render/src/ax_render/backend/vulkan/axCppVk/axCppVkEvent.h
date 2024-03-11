#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkEvent {
public:
	using Util = axVkUtil;

	~axCppVkEvent() { destroy(); }

	void create(VkDevice dev) {
		destroy();

		_dev = dev;

		VkEventCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;

		vkCreateEvent(dev, &info, Util::allocCallbacks(), &_h);
	}

	void destroy() {
		if (_h != VK_NULL_HANDLE) {
			vkDestroyEvent(_dev, _h, Util::allocCallbacks());
			_h = VK_NULL_HANDLE;
		}
	}

	void setEvent(VkCommandBuffer cmdBuf) {
		if (!_h) throw axError_Undefined(AX_LOC);
		vkCmdSetEvent(cmdBuf, _h, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
	}

	bool getEventStatus() {
		if (!_h) return false;
		auto res = vkGetEventStatus(_dev, _h);
		if (res == VK_EVENT_RESET) return false;
		if (res == VK_EVENT_SET) return true;
		Util::throwIfError(res);
		return false;
	}

	explicit operator bool() const { return _h != VK_NULL_HANDLE; }

private:
	VkEvent	_h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};


#endif