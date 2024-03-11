#pragma once

#include <ax_render.h>

#if AX_OS_WINDOWS

#if AX_OS_WINDOWS
	using axWinBOOL = BOOL;
//	#include <dxcapi.h> // from Windows SDK
//	#include <dxc/dxcapi.h> // from Vulkan SDK
	#include <dxcapi.h> // from vcpkg directx-dxc
	#include <d3d12shader.h>
	#pragma comment(lib, "dxcompiler.lib")
#else
	#define BOOL axWinBOOL
	using axWinBOOL = bool;
	#include <dxc/dxcapi.h>
	AX_STATIC_ASSERT(axTypeTraits::isSame<BOOL, axWinBOOL>::value);
	#undef BOOL
#endif

inline
axStrView axStrView_make(IDxcBlob* blob) {
	if (blob == nullptr)
		return axStrView();
	auto* s = static_cast<const char*>(blob->GetBufferPointer());
	auto  n = static_cast<axInt>(blob->GetBufferSize());
	return axStrView(s, n);
}

class axDXILCompilerHelprBase : public axNonCopyable {
public:
	bool checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}

	void throwIfError(HRESULT hr) {
		if (!checkError(hr)) {
			throw axError_Undefined(AX_LOC);
		}
	}
};

class axDXILCompiler : axDXILCompilerHelprBase {
public:
	using DataType = axRenderDataType;

	axDXILCompiler();

	void compile(	axStrView outFilename,
					axStrView filename,
					axStrView profile,
					axStrView entryFunc,
					axArray<axString>& include_dirs);

	bool tryLoadFile(axComPtr<IDxcBlobEncoding>& outBlob, axStrView filename);

	void writeDepFile(axStrView filename);

private:
	void _compileReflect(axStrView outFilename, IDxcBlob* byteCode, axStrView profile);
	void _compileReflect_inputs			(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_uniformBuffers	(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_textures		(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_samplers		(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);
	void _compileReflect_storageBuffers	(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc);

	class IncludeHandler;

	bool checkError(HRESULT hr) {
		if (FAILED(hr))
			return false;
		return true;
	}

	void throwIfError(HRESULT hr) {
		if (!checkError(hr)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	axComPtr<IDxcLibrary>				_dxcLib;
	axComPtr<IDxcCompiler2>				_dxc;

	struct OpenedFile {
		axString filename;
		axComPtr<IDxcBlobEncoding>	blob;
	};
	axArray<OpenedFile>		_openedFileBlob;
};

#endif
