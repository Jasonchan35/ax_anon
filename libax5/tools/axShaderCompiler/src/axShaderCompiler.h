#pragma once

#include <ax_render.h>

class axShaderCompiler : public axConsoleApp {
	AX_RTTI_CLASS(axShaderCompiler, axConsoleApp)
public:
	using Profile		= axShaderStageProfile;
	using Pass			= axShaderPassInfo;

	axShaderCompiler();

	void genMakefile				(axStrView outdir, axStrView filename);
	void genMakefile_DX11			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_DX12			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_SPIRV			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_GLSL			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_GLSL_ES		(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_Metal			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_Metal_iOS		(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);
	void genMakefile_ISPC			(axIString& mk, axInt passIndex, Pass& pass, axStrView relSourceDir);

	void showHelp();
	int onRun() override;

	struct Option {
		bool genMakefile = false;
		bool genSpirvInfo = false;
		bool hlsl = false;
		axStrView file;
		axString out;
		axString entry;
		axString profile;
		axArray<axString>	include_dirs;
	};
	Option opt;

	struct HLSLProfile {
		axString	stage;
		axInt		major = 0;
		axInt		minor = 0;
	};

	bool tryParse(HLSLProfile& outValue, axStrView profileStr);
};
