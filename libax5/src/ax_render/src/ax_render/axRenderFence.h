#pragma once

#include "axRenderResource.h"

class axRenderFence : public axRenderResource {
	AX_RTTI_CLASS(axRenderFence, axRenderResource)
public:
	struct MData {
		bool	done;
	};

	bool isDone() {
		if (_signaled) return true;

		if (onCheckCompleted()) {
			_signaled = 1;
			return true;
		}
		return false;
	}

friend class axRenderer;
friend class axRenderRequest;
friend class axRenderCommandDispatcher;
protected:
	virtual bool onCheckCompleted() = 0;
	axAtomicInt8	_signaled{0};
};