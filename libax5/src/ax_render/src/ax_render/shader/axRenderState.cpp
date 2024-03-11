#include "axRenderState.h"

axRenderState::Blend::Blend() {
	rgb.set(  BlendOp::Disable, BlendFactor::One, BlendFactor::OneMinusSrcAlpha);
	alpha.set(BlendOp::Disable, BlendFactor::One, BlendFactor::OneMinusSrcAlpha);
}
