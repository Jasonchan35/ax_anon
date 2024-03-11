#pragma once

#include "../../shader/axMaterial.h"
#include "axDX11Shader.h"

#if AX_USE_DX11

class axDX11Material : public axMaterial {
	AX_RTTI_CLASS(axDX11Material, axMaterial);
public:
	axDX11Material() = default;

	axDX11Shader*	shader() { return ax_type_cast_debug<axDX11Shader>(Base::shader()); }

	template<class CALL>
	struct ScopedBind {
		ScopedBind(axDX11Material& mtl_, axDX11CommandDispatcher& dispatcher_, CALL& call_)
			: mtl(mtl_)
			, dispatcher(dispatcher_)
			, call(call_)
		{
			mtl.bind(dispatcher, call);
		}
		~ScopedBind() {
			mtl.unbind(dispatcher, call);
		}

		axDX11Material&	mtl;
		axDX11CommandDispatcher& dispatcher;
		CALL& call;
	};

	ScopedBind<DrawCall>	scopedBind(axDX11CommandDispatcher& dispatcher, DrawCall& call) {
		return ScopedBind<DrawCall>(*this, dispatcher, call);
	}

	ScopedBind<ComputeCall>	scopedBind(axDX11CommandDispatcher& dispatcher, ComputeCall& call) {
		return ScopedBind<ComputeCall>(*this, dispatcher, call);
	}

protected:
	void bind	(axDX11CommandDispatcher& dispatcher, DrawCall&    drawCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, DrawCall&    drawCall);

	void bind	(axDX11CommandDispatcher& dispatcher, ComputeCall& drawCall);
	void unbind	(axDX11CommandDispatcher& dispatcher, ComputeCall& drawCall);
};

#endif //#if AX_USE_DX11
