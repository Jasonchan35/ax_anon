#pragma once

#if AX_USE_VULKAN

#include "axCppVkDevice.h"

class axCppVkFence : public axCppVkBase {
public:
	const VkFence& handle() { return _h; }
	operator const VkFence&() { return _h; }

	axCppVkFence() = default;
	axCppVkFence(axCppVkFence&& r) {
		_h = r._h;
		_dev = r._dev;
		r._h = VK_NULL_HANDLE;
		r._dev = VK_NULL_HANDLE;
	}

	~axCppVkFence() { destroy(); }

	void destroy();
	void create(VkDevice dev, bool signaled);

	void reset();
	bool wait(uint64_t nanoseconds = UINT64_MAX);
	bool waitAndReset(uint64_t nanoseconds = UINT64_MAX) {
		if (!wait(nanoseconds))
			return false;
		reset();
		return true;
	}

	bool check(bool callReset) {
		auto res = vkGetFenceStatus(_dev, _h);
		if (res == VK_NOT_READY) return false;

		if (callReset) reset();

		if (res == VK_SUCCESS) return true;
		Util::throwIfError(res);
		return false;
	}

	VkDevice device() { return _dev; }

private:
	VkFence _h = VK_NULL_HANDLE;
	VkDevice _dev = VK_NULL_HANDLE;
};

#endif