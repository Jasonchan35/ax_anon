#pragma once

#include <ax_render.h>

#if AX_OS_WINDOWS

#include <d3dcompiler.h>
#include <d3d11shader.h>

#if AX_COMPILER_VC
	#pragma comment( lib, "d3dCompiler.lib")
	#pragma comment( lib, "dxguid.lib")
#endif

class axHLSLCompiler : public axNonCopyable {
public:
	using DataType = axRenderDataType;

	void compile(axStrView outFilename, axStrView filename, axStrView profile, axStrView entryFunc, axArray<axString>& include_dirs);

	void _compileReflect				(axStrView filename, axByteSpan byteCodeView, axStrView profile);
	void _compileReflect_inputs			(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _compileReflect_uniformBuffers	(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _compileReflect_textures		(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _compileReflect_samplers		(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc);
	void _compileReflect_storageBuffers	(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc);

	class IncludeHandler : public ID3DInclude { 
	public:
		IncludeHandler(axStrView currentFile, axArray<axString>& include_dirs_);

		HRESULT Open (D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) override;
		HRESULT Close(LPCVOID pData) override;

		axIndirectArray< axMemMap >	_openedFiles;
		axString _currentFile;

		void writeDepFile(axStrView filename);
		axArray<axString>& include_dirs;

		axAllocator*	allocator() { return nullptr; }
	};

	axAllocator*	allocator() { return nullptr; }

	void throwIfError(HRESULT hr);
};

#endif //AX_OS_WINDOWS
