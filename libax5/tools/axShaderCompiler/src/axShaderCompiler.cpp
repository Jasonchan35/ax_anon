#include "axShaderCompiler.h"
#include "axHLSLCompiler.h"
#include "axDXILCompiler.h"
#include "axSpirvInfoGenerator.h"
#include "InfoParser.h"

axShaderCompiler::axShaderCompiler() {
	axError::s_setEnableAssertion(true);
}

void axShaderCompiler::genMakefile(axStrView outdir, axStrView filename) {
	if (!outdir) {
		AX_LOG("error: please specify outdir");
		throw axError_Undefined(AX_LOC);
	}

	InfoParser parser;
	parser.readFile(outdir, filename);

	axString mk;

	axString relSourceFilename;
	axPath::getRelative(relSourceFilename, filename, outdir);

	auto relSourceDir = axPath::dirname(relSourceFilename);

	axInt passIndex = 0;

	mk.append("all:\n\n");
	mk.append("source.hlsl: info.json\n"
			  "\t""$(axFileCmd) -touch source.hlsl\n"
			  "\n");

	mk.append("CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))\n\n");

	for (auto& pass : parser.info.passes) {
		genMakefile_DX11		(mk, passIndex, pass, relSourceDir);
		genMakefile_DX12		(mk, passIndex, pass, relSourceDir);
		genMakefile_SPIRV		(mk, passIndex, pass, relSourceDir);
		genMakefile_GLSL		(mk, passIndex, pass, relSourceDir);
		genMakefile_GLSL_ES		(mk, passIndex, pass, relSourceDir);
		genMakefile_Metal		(mk, passIndex, pass, relSourceDir);
		genMakefile_Metal_iOS	(mk, passIndex, pass, relSourceDir);
		genMakefile_ISPC		(mk, passIndex, pass, relSourceDir);
		passIndex++;
	}

	{
		axTempString outFilename(outdir, "/GNUmakefile");
		axFile::writeFileIfChanged(outFilename, mk, false);
	}
}

void axShaderCompiler::genMakefile_DX11(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_DX11";
	if (pass.csFunc) tag = "BUILD_DX11_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	axStrLiteral fmt =	"-include DX11_pass{0}.{1}.bin.dep\n"
						"all: DX11_pass{0}.{1}.bin\n"
						"DX11_pass{0}.{1}.bin: $(CURRENT_MAKEFILE) $(axShaderCompiler) \n"
						"DX11_pass{0}.{1}.bin: source.hlsl \n"
						"	\"$(axShaderCompiler)\" -hlsl -file=\"$<\" -out=\"$@\" -profile={1} -entry={2} -I=\"{3}\"\n"
						"\n";
	if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::DX11_VS, pass.vsFunc, relSourceDir);
	if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::DX11_PS, pass.psFunc, relSourceDir);
	if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::DX11_CS, pass.csFunc, relSourceDir);
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_DX12(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_DX12";
	if (pass.csFunc) tag = "BUILD_DX12_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	axStrLiteral fmt =	"-include DX12_pass{0}.{1}.bin.dep\n"
						"all: DX12_pass{0}.{1}.bin\n"
						"DX12_pass{0}.{1}.bin: $(CURRENT_MAKEFILE) $(axShaderCompiler) \n"
						"DX12_pass{0}.{1}.bin: source.hlsl \n"
						"	\"$(axShaderCompiler)\" -hlsl -file=\"$<\" -out=\"$@\" -profile={1} {2} -I=\"{4}\"\n"
						"	\"$(dxc)\" -Fo \"$@\" \"$<\" -T {1} {3} -I \"{4}\"\n "
						"\n";
	if (pass.vsFunc			) mk.appendFormat(fmt, passIndex, Profile::DX12_VS,  axString("-entry=", pass.vsFunc), axString("-E ", pass.vsFunc), relSourceDir);
	if (pass.psFunc			) mk.appendFormat(fmt, passIndex, Profile::DX12_PS,  axString("-entry=", pass.psFunc), axString("-E ", pass.psFunc), relSourceDir);
	if (pass.csFunc			) mk.appendFormat(fmt, passIndex, Profile::DX12_CS,  axString("-entry=", pass.csFunc), axString("-E ", pass.csFunc), relSourceDir);
	if (pass.rayTracingFunc	) mk.appendFormat(fmt, passIndex, Profile::DX12_Lib, "",                               "",                           relSourceDir);
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_SPIRV(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_SPIRV";
	if (pass.csFunc) tag = "BUILD_SPIRV_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{
		axStrLiteral fmt =	"all: SPIRV_pass{0}.{1}.bin\n"
							"SPIRV_pass{0}.{1}.bin: $(CURRENT_MAKEFILE) $(glslc) \n"
							"SPIRV_pass{0}.{1}.bin: source.hlsl\n"
							"	\"$(glslc)\""
									" -fshader-stage={2}"
									" -fentry-point={3}"
									" -fssbo-binding-base 4"
									" -ftexture-binding-base 8"
									" -fsampler-binding-base 8"
									" -I\"{4}\""
									" -o \"$@\" \"$<\""
									"\n"
							"\n";
		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_VS, "vertex",   pass.vsFunc, relSourceDir);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_PS, "fragment", pass.psFunc, relSourceDir);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_CS, "compute",  pass.csFunc, relSourceDir);
	}

	{	// SPIRV-Reflect
		axStrLiteral fmt =	"all: SPIRV_pass{0}.{1}.reflect.json\n"
							"SPIRV_pass{0}.{1}.reflect.json: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"SPIRV_pass{0}.{1}.reflect.json: SPIRV_pass{0}.{1}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" \\\n"
							"		--reflect\\\n"
							"		--remove-unused-variables \\\n"
							"\n"
							"	\"$(axFileCmd)\" -dos2unix \"$@\" \n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_VS);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_PS);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_CS);
	}

	{	// genSpirvInfo
		axStrLiteral fmt =	"all: SPIRV_pass{0}.{1}.bin.json\n"
							"SPIRV_pass{0}.{1}.bin.json: $(CURRENT_MAKEFILE) $(axShaderCompiler)\n"
							"SPIRV_pass{0}.{1}.bin.json: SPIRV_pass{0}.{1}.reflect.json\n"
							"	\"$(axShaderCompiler)\" -genSpirvInfo -file=\"$<\" -out=\"$@\" \\\n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_VS);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_PS);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::SPIRV_CS);
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_GLSL(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_GLSL";
	if (pass.csFunc) tag = "BUILD_GLSL_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{	// GLSL
		auto glsl_vsVer = Profile::GLSL_VS.constView().extractFromPrefix("vs_");
		auto glsl_psVer = Profile::GLSL_PS.constView().extractFromPrefix("ps_");
		auto glsl_csVer = Profile::GLSL_CS.constView().extractFromPrefix("cs_");

		axStrLiteral fmt =	"all: GLSL_pass{0}.{1}.glsl\n"
							"GLSL_pass{0}.{1}.glsl: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"GLSL_pass{0}.{1}.glsl: SPIRV_pass{0}.{2}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" \\\n"
							"		--version {3} --no-es \\\n"
							"		--extension GL_ARB_separate_shader_objects \\\n"
							"		--remove-unused-variables \\\n"
							// MacOSX doesn't support layout (binding=1)
							"		--no-420pack-extension \\\n"
							//MacOSX require variable name from VS output and PS input are the same, that's why remap it
							"		--rename-interface-variable {4}  0 \"_axVsToPs_00\"  \\\n" 
							"		--rename-interface-variable {4}  1 \"_axVsToPs_01\"  \\\n" 
							"		--rename-interface-variable {4}  2 \"_axVsToPs_02\"  \\\n" 
							"		--rename-interface-variable {4}  3 \"_axVsToPs_03\"  \\\n" 
							"		--rename-interface-variable {4}  4 \"_axVsToPs_04\"  \\\n" 
							"		--rename-interface-variable {4}  5 \"_axVsToPs_05\"  \\\n" 
							"		--rename-interface-variable {4}  6 \"_axVsToPs_06\"  \\\n" 
							"		--rename-interface-variable {4}  7 \"_axVsToPs_07\"  \\\n" 
							"		--rename-interface-variable {4}  8 \"_axVsToPs_08\"  \\\n" 
							"		--rename-interface-variable {4}  9 \"_axVsToPs_09\"  \\\n" 
							"		--rename-interface-variable {4} 10 \"_axVsToPs_10\"  \\\n" 
							"		--rename-interface-variable {4} 11 \"_axVsToPs_11\"  \\\n" 
							"		--rename-interface-variable {4} 12 \"_axVsToPs_12\"  \\\n" 
							"		--rename-interface-variable {4} 13 \"_axVsToPs_13\"  \\\n" 
							"		--rename-interface-variable {4} 14 \"_axVsToPs_14\"  \\\n" 
							"		--rename-interface-variable {4} 15 \"_axVsToPs_15\"  \\\n" 
							"		--rename-interface-variable {4} 16 \"_axVsToPs_16\"  \\\n" 
							"\n"
							"	\"$(axFileCmd)\" -dos2unix \"$@\" \n"
							"\n\n";
		
		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_VS, Profile::SPIRV_VS, glsl_vsVer, "out");
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_PS, Profile::SPIRV_PS, glsl_psVer, "in ");
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_CS, Profile::SPIRV_CS, glsl_csVer, "in ");
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_GLSL_ES(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_GLSL_ES";
	if (pass.csFunc) tag = "BUILD_GLSL_ES_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{	// GLSL
		auto glsl_vsVer = Profile::GLSL_VS.constView().extractFromPrefix("vs_");
		auto glsl_psVer = Profile::GLSL_PS.constView().extractFromPrefix("ps_");
		auto glsl_csVer = Profile::GLSL_CS.constView().extractFromPrefix("cs_");

		axStrLiteral fmt =	"all: GLSL_ES_pass{0}.{1}.glsl\n"
							"GLSL_ES_pass{0}.{1}.glsl: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"GLSL_ES_pass{0}.{1}.glsl: SPIRV_pass{0}.{2}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" \\\n"
							"		--version {3} --es \\\n"
							//"		--extension GL_ARB_separate_shader_objects \\\n"
							"		--remove-unused-variables \\\n"
							// iOS doesn't support layout (binding=1)
							"		--no-420pack-extension \\\n"
							// iOS require variable name from VS output and PS input are the same, that's why remap it
							"		--rename-interface-variable {4}  0 \"_axVsToPs_00\"  \\\n"
							"		--rename-interface-variable {4}  1 \"_axVsToPs_01\"  \\\n"
							"		--rename-interface-variable {4}  2 \"_axVsToPs_02\"  \\\n"
							"		--rename-interface-variable {4}  3 \"_axVsToPs_03\"  \\\n"
							"		--rename-interface-variable {4}  4 \"_axVsToPs_04\"  \\\n"
							"		--rename-interface-variable {4}  5 \"_axVsToPs_05\"  \\\n"
							"		--rename-interface-variable {4}  6 \"_axVsToPs_06\"  \\\n"
							"		--rename-interface-variable {4}  7 \"_axVsToPs_07\"  \\\n"
							"		--rename-interface-variable {4}  8 \"_axVsToPs_08\"  \\\n"
							"		--rename-interface-variable {4}  9 \"_axVsToPs_09\"  \\\n"
							"		--rename-interface-variable {4} 10 \"_axVsToPs_10\"  \\\n"
							"		--rename-interface-variable {4} 11 \"_axVsToPs_11\"  \\\n"
							"		--rename-interface-variable {4} 12 \"_axVsToPs_12\"  \\\n"
							"		--rename-interface-variable {4} 13 \"_axVsToPs_13\"  \\\n"
							"		--rename-interface-variable {4} 14 \"_axVsToPs_14\"  \\\n"
							"		--rename-interface-variable {4} 15 \"_axVsToPs_15\"  \\\n"
							"		--rename-interface-variable {4} 16 \"_axVsToPs_16\"  \\\n"
							"\n"
							"	\"$(axFileCmd)\" -dos2unix \"$@\" \n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_ES_VS, Profile::SPIRV_VS, glsl_vsVer, "out");
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_ES_PS, Profile::SPIRV_PS, glsl_psVer, "in ");
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::GLSL_ES_CS, Profile::SPIRV_CS, glsl_csVer, "in ");
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_Metal(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_METAL";
	if (pass.csFunc) tag = "BUILD_METAL_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{	// Metal SL
		axStrLiteral fmt =	"all: MTL_pass{0}.{1}.metal\n"
							"MTL_pass{0}.{1}.metal: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"MTL_pass{0}.{1}.metal: SPIRV_pass{0}.{2}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" --msl \n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_VS_Mac, Profile::SPIRV_VS);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_PS_Mac, Profile::SPIRV_PS);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_CS_Mac, Profile::SPIRV_CS);
	}

	{	// Metal byteCode
		axString airFiles;
		axStrLiteral fmt =	"all: MTL_pass{0}.{1}.air\n"
							"MTL_pass{0}.{1}.air: $(CURRENT_MAKEFILE)\n"
							"MTL_pass{0}.{1}.air: MTL_pass{0}.{1}.metal\n"
							"	xcrun -sdk macosx metal -c \"$<\" -o \"$@\" \n"
							"\n\n";

		if (pass.vsFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_VS_Mac); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_VS_Mac); }
		if (pass.psFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_PS_Mac); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_PS_Mac); }
		if (pass.csFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_CS_Mac); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_CS_Mac); }

		// Metal Lib
		if (airFiles) {
			mk.appendFormat("all: MTL_pass{0}.{1}.metallib\n"
							"MTL_pass{0}.{1}.metallib: $(CURRENT_MAKEFILE) $(metallib)\n"
							"MTL_pass{0}.{1}.metallib: {2}\n"
							"	xcrun -sdk macosx metallib {2}  -o \"$@\" \n"
							"\n\n",
							passIndex, Profile::MSL_Lib_Mac, airFiles);
		}
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_Metal_iOS(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_METAL_IOS";
	if (pass.csFunc) tag = "BUILD_METAL_IOS_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{
		// Metal SL iOS
		axStrLiteral fmt =	"all: MTL_pass{0}.{1}.metal\n"
							"MTL_pass{0}.{1}.metal: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"MTL_pass{0}.{1}.metal: SPIRV_pass{0}.{2}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" --msl --msl-ios\n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_VS_iOS, Profile::SPIRV_VS);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_PS_iOS, Profile::SPIRV_PS);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::MSL_CS_iOS, Profile::SPIRV_CS);
	}

	{	// Metal byteCode
		axString airFiles;			
		axStrLiteral fmt =	"all: MTL_pass{0}.{1}.air\n"
							"MTL_pass{0}.{1}.air: $(CURRENT_MAKEFILE)\n"
							"MTL_pass{0}.{1}.air: MTL_pass{0}.{1}.metal\n"
							"	xcrun -sdk iphoneos metal -c \"$<\" -o \"$@\" \n"
							"\n\n";

		if (pass.vsFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_VS_iOS); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_VS_iOS); }
		if (pass.psFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_PS_iOS); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_PS_iOS); }
		if (pass.csFunc) { mk.appendFormat(fmt, passIndex, Profile::MSL_CS_iOS); airFiles.appendFormat(" MTL_pass{0}.{1}.air", passIndex, Profile::MSL_CS_iOS); }

		// Metal Lib iOS
		if (airFiles) {
			mk.appendFormat("all: MTL_pass{0}.{1}.metallib\n"
							"MTL_pass{0}.{1}.metallib: $(CURRENT_MAKEFILE) $(metallib)\n"
							"MTL_pass{0}.{1}.metallib: {2}\n"
							"	xcrun -sdk iphoneos metallib {2}  -o \"$@\" \n"
							"\n\n",
							passIndex, Profile::MSL_Lib_iOS, airFiles);
		}
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::genMakefile_ISPC(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir) {
	axStrView tag = "BUILD_ISPC";
	if (pass.csFunc) tag = "BUILD_ISPC_COMPUTE";

	mk.append("ifeq ($(",tag,"), true)\n\n");
	{
		// ISPC
		axStrLiteral fmt =	"all: ISPC_pass{0}.{1}.ispc.c\n"
							"ISPC_pass{0}.{1}.ispc.c: $(CURRENT_MAKEFILE) $(spirv_cross)\n"
							"ISPC_pass{0}.{1}.ispc.c: SPIRV_pass{0}.{2}.bin\n"
							"	\"$(spirv_cross)\" \"$<\" --output \"$@\" --ispc\n"
							"\n\n";

		if (pass.vsFunc) mk.appendFormat(fmt, passIndex, Profile::ISPC_VS, Profile::SPIRV_VS);
		if (pass.psFunc) mk.appendFormat(fmt, passIndex, Profile::ISPC_PS, Profile::SPIRV_PS);
		if (pass.csFunc) mk.appendFormat(fmt, passIndex, Profile::ISPC_CS, Profile::SPIRV_CS);
	}
	mk.append("endif\n\n");
}

void axShaderCompiler::showHelp() {
	AX_LOG(	"==== axShaderCompiler Help: ===="
			"  axShaderCompiler -genMakefile file=<filename> out=<filename>\n"
			"  axShaderCompiler -hlsl -file=<filename> -out=<filename> -profile=<profile_name> -entry=<entry_func_name> \n");
}

int axShaderCompiler::onRun() {
	auto args = commandArguments();

	for (axInt i=1; i<args.size(); i++) {
		auto& a = args[i];
		if (auto v = a.extractFromPrefix("-file=")) {
			opt.file = v;
			continue;
		}

		if (auto v = a.extractFromPrefix("-out=")) {
			axPath::unixPath(opt.out, v);
			continue;
		}

		if (a == "-genMakefile") {
			opt.genMakefile = true;
			continue;
		}

		if (a == "-genSpirvInfo") {
			opt.genSpirvInfo = true;
			continue;
		}

		if (a == "-hlsl") {
			opt.hlsl = true;
			continue;
		}

		if (auto v = a.extractFromPrefix("-profile=")) {
			opt.profile = v;
			continue;
		}

		if (a == "-entry=") { // -entry="" <-- value can be empty
			opt.entry = ""; 
			continue;
		}

		if (auto v = a.extractFromPrefix("-entry=")) {
			opt.entry = v;
			continue;
		}

		if (auto v = a.extractFromPrefix("-I=")) {
			opt.include_dirs.emplaceBack(v);
			continue;
		}

		AX_LOG("Unknown argument {?}", a);
		showHelp();
		return -1;
	}

	if (!opt.file) { showHelp(); return -1; }

	if (opt.genMakefile) {
		genMakefile(opt.out, opt.file);

	} else if (opt.genSpirvInfo) {
		axSpirvInfoGenerator g;
		g.generate(opt.out, opt.file);

	} else if (opt.hlsl) {
#if AX_OS_WINDOWS
		HLSLProfile profile;
		if (!tryParse(profile, opt.profile)) {
			AX_LOG("error: invalid HLSL profile {?}", opt.profile);
			return -1;
		}

		if (profile.major >= 6) {
			axDXILCompiler c;
			c.compile(opt.out, opt.file, opt.profile, opt.entry, opt.include_dirs);
		} else {
			axHLSLCompiler c;
			c.compile(opt.out, opt.file, opt.profile, opt.entry, opt.include_dirs);
		}
#else
		AX_LOG("HLSL only support on Windows");
#endif
	}else{
		showHelp();
		return -1;
	}
	return 0;
}

bool axShaderCompiler::tryParse(HLSLProfile& outValue, axStrView profileStr) {
	outValue = HLSLProfile();

	axArray<axStrView, 4> tokens;
	profileStr.getTokens(tokens, '_');
	if (tokens.size() != 3) return false;

	outValue.stage = tokens[0];
	if (!tokens[1].tryParse(outValue.major)) return false;
	if (!tokens[2].tryParse(outValue.minor)) return false;

	return true;
}
