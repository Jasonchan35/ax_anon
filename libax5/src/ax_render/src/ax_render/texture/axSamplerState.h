#pragma once

#define axTextureFilter_EnumList(E) \
	E(Unknown,) \
	E(Point,)      /* 1 texels on closest mipmap level */ \
	E(Linear,)     /* 1 texels on 2 mipmap levels */ \
	E(Bilinear,)   /* 4 texels on closest mipmap level */  \
	E(Trilinear,)  /* 4 texels on 2 mipmap levels */\
	E(Anisotropic,) \
//----
ax_ENUM_CLASS(axTextureFilter, axUInt8)

#define axTextureWrap_EnumList(E) \
	E(Unknown,) \
	E(Repeat,) \
	E(Clamp,) \
	E(Mirror,) \
	E(MirrorOnce,) \
//----
ax_ENUM_CLASS(axTextureWrap, axUInt8)

class axSamplerState {
public:
	using Filter	= axTextureFilter;
	using Wrap		= axTextureWrap;

	Filter	filter	= Filter::Bilinear;
	Wrap	wrapU	= Wrap::Repeat;
	Wrap	wrapV	= Wrap::Repeat;
	Wrap	wrapW	= Wrap::Repeat;

	float	minLOD	= 0;
	float	maxLOD	= axNumLimit<float>::max();
};
