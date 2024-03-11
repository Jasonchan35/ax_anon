
class axRenderVertex_ClassGenerator {
public:
	axInt pos, uv, color, normal, custom;

	axTempString classSuffix;
	axTempString baseClassSuffix;
	axTempString templateParams;

	axRenderVertex_ClassGenerator(axInt pos_, axInt uv_, axInt color_, axInt normal_, axInt custom_)
		: pos(pos_)
		, uv(uv_)
		, color(color_)
		, normal(normal_)
		, custom(custom_)
	{
		if (pos) {
			classSuffix.append("Pos");
			baseClassSuffix.append("Pos");
			templateParams.append("axFloat32x3");
		}

		if (uv) {
			classSuffix.append("Uv");
			if (uv > 1) classSuffix.appendFormat("{?}", uv);
			baseClassSuffix.append("Uv");
			templateParams.appendFormat(", axFloat32x2, {?}", uv);
		}

		if (color) {
			classSuffix.append("Color");
			if (color > 1) classSuffix.appendFormat("{?}", color);
			baseClassSuffix.append("Color");
			templateParams.appendFormat(", axColorRGBAh, {?}", color);
		}

		if (normal) {
			if (normal >= 1) {
				classSuffix.append("Normal");
				baseClassSuffix.append("Normal");
			} 
			if (normal >= 2) {
				classSuffix.append("Tangent");
				baseClassSuffix.append("Tangent");
			}
			if (normal >= 3) {
				classSuffix.append("Binormal");
				baseClassSuffix.append("Binormal");
			}

			templateParams.appendFormat(", axFloat32x3");
		}

		if (custom) {
			classSuffix.append("Custom");
			if (custom > 1) classSuffix.appendFormat("{?}", custom);
			baseClassSuffix.append("Custom");
			templateParams.appendFormat(", axFloat32x4, {?}", custom);
		}
	}

	void genVertexInputHLSL(axString& o) {
		o.append("struct VS_Input_", classSuffix, " {\n");
		o.append("    float4 pos : POSITION0;\n");

		for (axInt i=0; i<uv; i++) {
			o.appendFormat("    float2 uv{?} : TEXCOORD{?};\n", i, i);
		}
		for (axInt i=0; i<color; i++) {
			o.appendFormat("    float4 color{?} : COLOR{?};\n", i, i);
		}

		if (normal >= 3) o.append("    float3 binormal : BINORMAL0;\n");
		if (normal >= 2) o.append("    float3 tangent  : TANGENT0;\n");
		if (normal >= 1) o.append("    float3 normal   : NORMAL0;\n");

		for (axInt i=0; i<custom; i++) {
			o.appendFormat("    float4 custom{?} : CUSTOM{?};\n", i, i);
		}

		o.append("};\n");
		o.append("float4 vs_main_", classSuffix, "(VS_Input_", classSuffix, " i) : SV_POSITION { return float4(0,0,0,1); }\n\n");
	}

	void genVertex(axString& o) {
		o.appendFormat("using axRenderVertex_{?,34} = axRenderVertexBase_{?}<{?}>;\n", classSuffix, baseClassSuffix, templateParams);
	}
};


class axRenderVertexBase_ClassGenerator {
public:
	axTempString templateParams;
	axTempString useTemplateParams;
	axTempString className;

	axInt pos, uv, color, normal, custom;

	axRenderVertexBase_ClassGenerator(axInt pos_, axInt uv_, axInt color_, axInt normal_, axInt custom_) 
		: pos(pos_)
		, uv(uv_)
		, color(color_)
		, normal(normal_)
		, custom(custom_)
	{
		if (pos) {
			className.append("Pos");
			templateParams.append("class POS");
			useTemplateParams.append("POS");
		}

		if (uv) {
			className.append("Uv");
			templateParams.append(", class UV, axInt UV_COUNT");
			useTemplateParams.append(", UV, UV_COUNT");
		}

		if (color) {
			className.append("Color");
			templateParams.append(", class COLOR, axInt COLOR_COUNT");
			useTemplateParams.append(", COLOR, COLOR_COUNT");
		}

		if (normal) {
			templateParams.append(", class NORMAL");
			useTemplateParams.append(", NORMAL");

			if (normal >= 1) {
				className.append("Normal");
			}
			if (normal >= 2) {
				className.append("Tangent");
			}
			if (normal >= 3) {
				className.append("Binormal");
			}
		}

		if (custom) {
			className.append("Custom");
			templateParams.append(", class CUSTOM, axInt CUSTOM_COUNT");
			useTemplateParams.append(", CUSTOM, CUSTOM_COUNT");
		}
	}

	void genVertexBase(axString& o) {
		if (pos) {
			o.appendFormat("template<{?}>\n", templateParams);
		}

		o.appendFormat("class alignas(axRenderVertex_kAlignment) axRenderVertexBase_{?} {{\n", className);
		o.appendFormat("\t""using This = axRenderVertexBase_{?};\n", className);
		o.appendFormat("public:\n");
		o.append("\t""using NormalType = axRenderNormalType;\n\n");
		o.append("\t""using VertexType = axRenderVertexType;\n\n");

		if (pos) {
			o.append("\t""using Pos = POS;\n");
			o.append("\t""Pos pos;\n\n");
		} else {
			o.append("\t""using Pos = void;\n");
			o.append("\t""uint8_t _padding[axRenderVertex_kAlignment];\n\n");
		}

		if (uv) {
			o.append("\t""using Uv = UV;\n");
			o.append("\t""static constexpr axInt kUvCount = UV_COUNT;\n");
			o.append("\t""axFixedArray<Uv, kUvCount> uv;\n\n");
		} else {
			o.append("\t""using Uv = void;\n");
			o.append("\t""static constexpr axInt kUvCount = 0;\n\n");
		}

		if (color) {
			o.append("\t""using Color = COLOR;\n");
			o.append("\t""static constexpr axInt kColorCount = COLOR_COUNT;\n");
			o.append("\t""axFixedArray<Color, kColorCount> color;\n\n");
		} else {
			o.append("\t""using Color = void;\n");
			o.append("\t""static constexpr axInt kColorCount = 0;\n\n");
		}

		switch (normal) {
			case 0: {
				o.append("\t""using Normal = void;\n");
				o.append("\t""static constexpr NormalType kNormalType = NormalType::None;\n");
				o.append("\n");
			}break;
			case 1: {
				o.append("\t""using Normal = NORMAL;\n");
				o.append("\t""static constexpr NormalType kNormalType = NormalType::Normal;\n");
				o.append("\t""Normal normal;\n");
				o.append("\n");
			}break;
			case 2: {
				o.append("\t""using Normal = NORMAL;\n");
				o.append("\t""static constexpr NormalType kNormalType = NormalType::NormalTangent;\n");
				o.append("\t""Normal normal;\n");
				o.append("\t""Normal tangent;\n");
				o.append("\n");
			}break;
			case 3: {
				o.append("\t""using Normal = NORMAL;\n");
				o.append("\t""static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;\n");
				o.append("\t""Normal normal;\n");
				o.append("\t""Normal tangent;\n");
				o.append("\t""Normal binormal;\n");
				o.append("\n");
			}break;
		}

		if (custom) {
			o.append("\t""using Custom = CUSTOM;\n");
			o.append("\t""static constexpr axInt kCustomCount = CUSTOM_COUNT;\n");
			o.append("\t""axFixedArray<Custom, kCustomCount> custom;\n\n");
		} else {
			o.append("\t""using Custom = void;\n");
			o.append("\t""static constexpr axInt kCustomCount = 0;\n\n");
		}

		o.append("\t""static constexpr VertexType kType\n"
					"\t\t""= axRenderVertexType_makeT<\n"
					"\t\t\t""Pos,\n"
					"\t\t\t""Uv, kUvCount,\n"
					"\t\t\t""Color, kColorCount,\n"
					"\t\t\t""Normal, kNormalType,\n"
					"\t\t\t""Custom, kCustomCount>();\n\n");


		o.append("\t""static axRenderVertexDesc s_desc();\n");

		o.append("\t""void onFormat(axFormat& f) const {\n");
		if (pos)		 o.append("\t\t""f << \"\\n  pos      =\" << pos;\n");
		if (uv)			 o.append("\t\t""f << \"\\n  uv       =\" << uv;\n");
		if (color)		 o.append("\t\t""f << \"\\n  color    =\" << color;\n");
		if (normal >= 1) o.append("\t\t""f << \"\\n  normal   =\" << normal;\n");
		if (normal >= 2) o.append("\t\t""f << \"\\n  tangent  =\" << tangent;\n");
		if (normal >= 3) o.append("\t\t""f << \"\\n  binormal =\" << binormal;\n");
		if (custom)		 o.append("\t\t""f << \"\\n  custom   =\" << custom;\n");
		o.append("\t""}\n");

		o.appendFormat("};\n\n");

		if (pos) {
			o.appendFormat("template<{?}>\n", templateParams);
		}
		o.appendFormat("inline axRenderVertexDesc axRenderVertexBase_{?}", className);
		if (pos) {
			o.appendFormat("<{?}>", useTemplateParams);
		}
		o.append("::s_desc() {\n");
		o.append("\t""static axRenderVertexDescMakeT<This> s;\n");
		o.append("\t""return &s;\n");
		o.append("}\n\n");

	}
};


int main() {
	axString outVertexInputHLSL;

	axString outVertex;
	axString outVertexBase;

	const axInt colorCount	= 2;
	const axInt uvCount		= 8;
	const axInt customCount = 1;
	const axInt normalCount = 3;

	{
		const axInt n = 16 * 1024 * 1024;
		outVertexInputHLSL.reserve(n);
		outVertexBase.reserve(n);
		outVertex.reserve(n);
	}

	outVertexBase.append("#pragma once\n");
	outVertexBase.append("#include \"axRenderVertexDesc.h\"\n");
	outVertexBase.append("\n\n");

//-------------	
	{
		axRenderVertexBase_ClassGenerator cls(0, 0, 0, 0, 0);
		cls.genVertexBase(outVertexBase);
	}
	for (axInt nl = 0; nl < 4; nl++) {
		for (axInt color = 0; color < 2; color++) {
			for (axInt uv = 0; uv < 2; uv++) {
				for (axInt custom = 0; custom < 2; custom++) {
					axRenderVertexBase_ClassGenerator cls(true, uv, color, nl, custom);
					cls.genVertexBase(outVertexBase);
				}
			}
		}
	}

//-------------
	outVertex.append("#pragma once\n");
	outVertex.append("#include \"axRenderVertexBase_Generated.h\"\n");
	outVertex.append("\n\n");

	axInt count = 0;
	for (axInt nl = 0; nl < normalCount; nl++) {
		for (axInt color = 0; color <= colorCount; color++) {
			for (axInt uv = 0; uv <= uvCount; uv++) {
				for (axInt custom = 0; custom <= customCount; custom++) {
					axRenderVertex_ClassGenerator cls(1, uv, color, nl, custom);
					cls.genVertex(outVertex);
					cls.genVertexInputHLSL(outVertexInputHLSL);
					count++;
				}
			}
		}
	}

	outVertex.appendFormat("\n//---- count = {?} ----\n", count);

//---------------------

	axString outdir;
	outdir.set(axPath::dirname(__FILE__), "/../../../render/src/ax/render/vertex/");
	axString filename;

	filename.set(outdir, "axRenderVertexBase_Generated.h");
	axFile::writeFileIfChanged(filename, outVertexBase, false);

	filename.set(outdir, "axRenderVertex_Generated.h");
	axFile::writeFileIfChanged(filename, outVertex, false);
	
	filename.set(outdir, "axRenderVertexInput_Generated.hlsl");
	axFile::writeFileIfChanged(filename, outVertexInputHLSL, false);

	return 0;
}
