#pragma once

#include "../axRenderDataType.h"
#include "../texture/axSamplerState.h"

class axRenderState {
	using This = axRenderState;
public:
	using Cull			= axRenderState_Cull;
	using DepthTestOp	= axRenderState_DepthTestOp;
	using BlendOp		= axRenderState_BlendOp;
	using BlendFactor	= axRenderState_BlendFactor;
	using Rect2			= axRect2f;

	static const axInt kMaxRenderTargetCount = axRender_kMaxRenderTargetCount;

	Cull	cull = Cull::Back;
	bool	antialiasedLine = false;
	bool	wireframe = false;
	float	pointSize = 5;
	
	struct DepthTest {
		DepthTestOp	op = DepthTestOp::LessEqual;
		bool writeMask = true;

		AX_INLINE
		bool operator==(const DepthTest& r) const {
			return op == r.op && writeMask == r.writeMask;
		}

		bool isEnable() const { return op != DepthTestOp::Always; }

		template<class SE>
		void onSerialize(SE& se) {
			se.named_io("op", op);
			se.named_io("writeMask", writeMask);
		}
	};
	DepthTest	depthTest;

	struct BlendFunc {
		BlendOp		op;
		BlendFactor	srcFactor;
		BlendFactor	dstFactor;

		void set(BlendOp op_, BlendFactor srcFactor_, BlendFactor dstFactor_) {
			op = op_;
			srcFactor = srcFactor_;
			dstFactor = dstFactor_;
		}

		template<class SE>
		void onSerialize(SE& se) {
			se.named_io("op", op);
			se.named_io("srcFactor", srcFactor);
			se.named_io("dstFactor", dstFactor);
		}
	};

	struct Blend {
		BlendFunc	rgb;
		BlendFunc	alpha;
		axColor		constColor;

		Blend();

		bool isEnable() const { return rgb.op != BlendOp::Disable || alpha.op != BlendOp::Disable; }

		template<class SE>
		void onSerialize(SE& se) {
			se.named_io("rgb",			rgb);
			se.named_io("alpha",		alpha);
			se.named_io("constColor",	constColor);
		}
	};
	Blend blend;

	template<class SE>
	void onSerialize(SE& se) {
		se.named_io("cull", cull);
		se.named_io("blend", blend);
		se.named_io("depthTest", depthTest);
		se.named_io("antialiasedLine", antialiasedLine);
		se.named_io("wireframe", wireframe);
	}
};


