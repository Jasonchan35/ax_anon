//
//  axMTLFence.h
//  libax_render
//
//  Created by Jason on 13/6/2020.
//

#pragma once

#include "../../axRenderFence.h"

class axMTLFence : public axRenderFence {
	AX_RTTI_CLASS(axMTLFence, axRenderFence)
public:
	using Util = axMTLUtil;

	axMTLFence() {
//		auto* mtlDevice = Util::mtlDevice();
//		_ev = [mtlDevice newSharedEvent];
	}

	void addToGpu(id<MTLCommandBuffer> cmdBuf) {
//		if (!_ev) throw axError_Undefined(AX_LOC);
//		[cmdBuf encodeSignalEvent:_ev value:1];

		[cmdBuf addCompletedHandler:^(id<MTLCommandBuffer> cmdBuf) {
			_a = 1;
		}];
	}

	bool onCheckCompleted() override {
		if (_a) return true;
		return false;

//		auto v = _ev.signaledValue;
//		if (v) return true;
//		return false;
	}

private:
//	id<MTLSharedEvent>	_ev;
	axAtomicInt8	_a;
};

