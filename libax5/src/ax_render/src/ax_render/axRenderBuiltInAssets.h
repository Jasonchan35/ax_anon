#pragma once

#include "shader/axMaterial.h"
#include "texture/axTexture.h"
#include "mesh/axRenderMeshEdit.h"

class axRenderer;

#define axRenderBuiltInAssets_ShaderId_EnumList(E) \
	E(Unknown,) \
	E(BlinnPhong,) \
	E(Wireframe,) \
	E(Gizmo,) \
	E(UIText,) \
	E(UITexture0,) \
	E(UITexture1,) \
	E(UITexture2,) \
	E(UITextTexture0,) \
	E(UIColorMaskTexture,) \
	E(TextBillboardTexture0,) \
	E(UnlitTexture0,) \
	E(UnlitTexture1,) \
	E(UnlitTexture2,) \
	E(SimpleLighting,) \
//-------
ax_ENUM_CLASS(axRenderBuiltInAssets_ShaderId, axUInt16)

#define axRenderBuiltInAssets_ComputeShaderId_EnumList(E) \
	E(Unknown,) \
	E(ComputeTriangles,) \
//-------
	// E(ComputeTest,) \ // require Float64

ax_ENUM_CLASS(axRenderBuiltInAssets_ComputeShaderId, axUInt16)

class axRenderBuiltInAssets : public axNonCopyable {
public:
	using Shader	= axShader;
	using Material	= axMaterial;
	using Texture2D	= axTexture2D;
	using DataType	= axRenderDataType;

	static axRenderBuiltInAssets* s_instance();

	void load (axStrView path);

	struct Shaders {
	#define AX_MACRO_OP(E,...) \
		axSPtr<Shader> E; \
	//------
		axRenderBuiltInAssets_ShaderId_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP
	};
	Shaders shaders;

	struct ComputeShaders {
	#define AX_MACRO_OP(E,...) \
		axSPtr<Shader> E; \
	//------
		axRenderBuiltInAssets_ComputeShaderId_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP
	};
	ComputeShaders computeShaders;

	axSPtr<Texture2D>	uvCheckerTex;

	axSPtr<Texture2D>	dummyMissingTex1D;
	axSPtr<Texture2D>	dummyMissingTex2D;
	axSPtr<Texture2D>	dummyMissingTex3D;
};
