#pragma once

#include "axGLRenderContext.h"

class axGLFence : public axRenderFence {
	AX_RTTI_CLASS(axGLFence, axRenderFence)
public:
	~axGLFence() { destroy(); }

	void destroy() {
		auto d = _mdata.scopedLock();
		if (d->fence) {
			glDeleteSync(d->fence);
			d->fence = nullptr;
		}
	}

	void addToGpu() {
		auto d = _mdata.scopedLock();
		if (d->fence) {
			glDeleteSync(d->fence);
			throw axError_Undefined(AX_LOC);
		}

		d->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		if (!d->fence) throw axError_Undefined(AX_LOC);
	}

	bool onCheckCompleted() override {
		auto d = _mdata.scopedLock();
		if (!d->fence) return false;

		GLint result;
		glGetSynciv(d->fence, GL_SYNC_STATUS, sizeof(result), nullptr, &result);
		if (result == GL_UNSIGNALED) return false;
		if (result == GL_SIGNALED) return true;

		throw axError_Undefined(AX_LOC);
		return false;
	}

	struct MData {
		GLsync fence = nullptr;
	};
	axSpinLockProtected<MData> _mdata;
};