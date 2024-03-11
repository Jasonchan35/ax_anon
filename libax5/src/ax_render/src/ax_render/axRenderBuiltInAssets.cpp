#include "axRenderContext.h"
#include "axRenderBuiltInAssets.h"
#include "axRenderer.h"
#include "shader/axShader.h"

axRenderBuiltInAssets* axRenderBuiltInAssets::s_instance() {
	return axRenderer::s_instance()->builtInAssets();
}

void axRenderBuiltInAssets::load(axStrView path) {
	auto* renderer = axRenderer::s_instance();

	axTempString tmp;

	#define AX_MACRO_OP(E,...) \
		{ \
			tmp.set(path,"/shaders/_outdir/builtIn/" #E ".axShader"); \
			shaders.E = renderer->createShaderFromFile(tmp); \
		} \
	//-------
		axRenderBuiltInAssets_ShaderId_EnumList(AX_MACRO_OP)
	#undef AX_MACRO_OP

	auto& info = renderer->adapterInfo();
	if (info.computeShader) {
		#define AX_MACRO_OP(E,...) \
			{ \
				tmp.set(path,"/shaders/_outdir/builtIn/" #E ".axShader"); \
				computeShaders.E = renderer->createShaderFromFile(tmp); \
			} \
		//-------
			axRenderBuiltInAssets_ComputeShaderId_EnumList(AX_MACRO_OP)
		#undef AX_MACRO_OP
	}

	tmp.set(path, "/images/uvCheckerGray.jpg");
//	tmp.set(path, "/images/uvCheckerAlpha.png");
	uvCheckerTex = renderer->createTexture2DFromFile(tmp);

	//dummyMissingTex2D = renderer->createTexture2DFromData();
}

