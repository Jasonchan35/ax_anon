#pragma once
#include <ax_ui.h>

namespace Anon {
	using Vec2i = axVec2i;
	using Vec3i = axVec3i;
	using Vec4i = axVec4i;

	using Vec2f = axVec2f;
	using Vec3f = axVec3f;
	using Vec4f = axVec4f;

	using Vec2d = axVec2d;
	using Vec3d = axVec3d;
	using Vec4d = axVec4d;

	using Mat4f = axMat4f;
	using Mat4d = axMat4d;

	using Quat4f = axQuat4f;
	using Quat4d = axQuat4d;

	using Ray3f	= axRay3f;

	using Color		= axColor;
	using ColorRGB	= axColorRGBf;

	using String	= axString;
	using StrView	= axStrView;

	template<class T>
	using Span		= axSpan<T>;

	template<class T>
	using MutSpan	= axMutSpan<T>;

	struct AnonNames : public axRenderNames {
		#define E(X) \
			static axNameId X() { static axNameId _s = axNameId::s_make(#X); return _s; } \
		//-----
			E(center)
			E(edge)
			E(normal)
			E(point)
			E(primitive)
			E(vertex)
			E(uv)
			E(uv1)
			E(uv2)
			E(uv3)
			E(uv4)
			E(uv5)
			E(uv6)
			E(uv7)
		#undef E
	};

} //namespace



