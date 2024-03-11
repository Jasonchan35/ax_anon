#include "axDXILCompiler.h"

#if AX_OS_WINDOWS

class axDXILCompiler::IncludeHandler : public IDxcIncludeHandler, axDXILCompilerHelprBase{
public:

	IncludeHandler(axDXILCompiler* compiler, axStrView currentFile, axArray<axString>& includeDirs) 
		: _compiler(compiler) 
	{
		_currentFileDir = axPath::dirname(currentFile);
	}

	virtual HRESULT STDMETHODCALLTYPE LoadSource(
		_In_ LPCWSTR pFilename,                                   // Candidate filename.
		_COM_Outptr_result_maybenull_ IDxcBlob **ppIncludeSource  // Resultant source object for included file, nullptr if not found.
	) override
	{
		axTempString filename(_currentFileDir);
		filename.appendUtf_c_str(pFilename);

		axComPtr<IDxcBlobEncoding> fileBlob;

		if (!_compiler->tryLoadFile(fileBlob, filename)) {
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		*ppIncludeSource = fileBlob.detach();

		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef	() override { return 10; }
	virtual ULONG STDMETHODCALLTYPE Release	() override { return 10; }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) override { return E_FAIL; }

private:
	axString	_currentFileDir;
	axDXILCompiler*	_compiler;
};

axDXILCompiler::axDXILCompiler() {
	HRESULT hr;
	hr = DxcCreateInstance(CLSID_DxcLibrary,  IID_PPV_ARGS(_dxcLib.ptrForInit()));
	throwIfError(hr);

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(_dxc.ptrForInit()));
	throwIfError(hr);
}

bool axDXILCompiler::tryLoadFile(axComPtr<IDxcBlobEncoding>& outBlob, axStrView filename) {
	outBlob.unref();

	if (!axFile::exists(filename))
		return false;

	axTempStringW filenameW;
	filenameW.setUtf(filename);

	UINT32 codePage = 0;
	HRESULT hr;
	hr = _dxcLib->CreateBlobFromFile(filenameW.c_str(), &codePage, outBlob.ptrForInit());
	if (!checkError(hr)) {
		return false;
	}

	if (!outBlob) {
		return false;
	}

	auto& f = _openedFileBlob.emplaceBack();
	f.filename = filename;
	f.blob.ref(outBlob);
	return true;
}

void axDXILCompiler::writeDepFile(axStrView filename) {
	axString outFilename;
	outFilename.set(filename, ".dep");

	axString o;
	o.append(filename, ":\\\n");

	for (auto& f : _openedFileBlob) {
		o.append("\t", f.filename, "\\\n");
	}
	o.append("\n");

	axFile::writeFile(outFilename, o, false);
}

void axDXILCompiler::compile(	axStrView outFilename,
								axStrView filename,
								axStrView profile,
								axStrView entryFunc,
								axArray<axString>& include_dirs)
{
	axString cur;
	axPath::getCurrentDir(cur);
	//AX_LOG("Current dir = {?}", cur);
	//AX_DUMP_VAR(entryFunc, profile, include_dirs);

	auto filenameW	= axTempStringW::s_convert(filename);
	auto entryFuncW	= axTempStringW::s_convert(entryFunc);
	auto profileW	= axTempStringW::s_convert(profile);

	HRESULT hr;

	axComPtr<IDxcBlobEncoding> sourceBlob;
	if (!tryLoadFile(sourceBlob, filename)) {
		AX_LOG("File not found {?}", filename);
		return;
	}

	axArray<axStringW,	64>	argsStrings;
	axArray<LPCWSTR,	64>	args;

	for (auto& d : include_dirs) {
		argsStrings.emplaceBack().format("-I{?}", d);
	}

	for (auto& e : argsStrings) {
		args.emplaceBack(e.c_str());
	}

	axArray<DxcDefine,  128>	defines;

	IncludeHandler includeHandler(this, filename, include_dirs);

	axComPtr<IDxcOperationResult>	result;

	HRESULT resultStatus;

	if (0) {
		hr = _dxc->Preprocess(	sourceBlob, filenameW.c_str(), 
								args.data(),    ax_safe_static_cast<UINT>(args.size()),
								defines.data(), ax_safe_static_cast<UINT>(defines.size()),
								&includeHandler, result.ptrForInit());
		throwIfError(hr);
		hr = result->GetStatus(&resultStatus);
		throwIfError(hr);

		axComPtr<IDxcBlob> resultDetail;
		hr = result->GetResult(resultDetail.ptrForInit());
		throwIfError(hr);

		auto data = axStrView_make(resultDetail);
		AX_LOG("preprocess: ============= \n{?}\n===============\n", data);
	}

	axComPtr<IDxcBlob>	_debugBlob;
	hr = _dxc->CompileWithDebug(	sourceBlob,	filenameW.c_str(),
									entryFuncW.c_str(),
									profileW.c_str(),
									args.data(),    ax_safe_static_cast<UINT>(args.size()),
									defines.data(), ax_safe_static_cast<UINT>(defines.size()),
									&includeHandler, result.ptrForInit(),
									nullptr, _debugBlob.ptrForInit());
	throwIfError(hr);

//	HRESULT resultStatus;
	hr = result->GetStatus(&resultStatus);
	throwIfError(hr);

	if (!checkError(resultStatus)) {
		axComPtr<IDxcBlobEncoding> errorMsgBlob;
		hr = result->GetErrorBuffer(errorMsgBlob.ptrForInit());
		throwIfError(hr);

		axWinBOOL known;
		UINT32 codePage = CP_UTF8;
		hr = errorMsgBlob->GetEncoding(&known, &codePage);
		throwIfError(hr);

		if (codePage != CP_UTF8) {
			AX_LOG("Error message code page is not UTF8");
		}			

		auto errorMsg = axStrView_make(errorMsgBlob);
		AX_LOG("error: {?}", errorMsg);
		return;
	}

	auto debugMsg = axStrView_make(_debugBlob);
	if (debugMsg) {
		AX_LOG("debug message: {?}", debugMsg);
	}

	axComPtr<IDxcBlob>	byteCode;
	hr = result->GetResult(byteCode.ptrForInit());
	throwIfError(hr);

	writeDepFile(outFilename);

	// have to use dxc.exe from MS, because that add hash in the bin file
	//axByteSpan byteCodeView(reinterpret_cast<const axByte*>(byteCode->GetBufferPointer()),
	// 							 ax_safe_static_cast<axInt>(byteCode->GetBufferSize()));

	//auto u = axFile::writeFile(outFilename, byteCodeView, false);
	//if (u == '=') {
	//	axFile::touch(outFilename);
	//}

	_compileReflect(outFilename, byteCode, profile);
}

void axDXILCompiler::_compileReflect(axStrView outFilename, IDxcBlob* byteCode, axStrView profile) {

	axComPtr<IDxcContainerReflection>	container;
	axComPtr<ID3D12ShaderReflection>	reflect;

	HRESULT hr;

	hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(container.ptrForInit()));
	throwIfError(hr);

	hr = container->Load(byteCode);
	throwIfError(hr);

	UINT shaderIdx;
	hr = container->FindFirstPartKind(axFourCC("DXIL"), &shaderIdx);
	throwIfError(hr);

	hr = container->GetPartReflection(shaderIdx, IID_PPV_ARGS(reflect.ptrForInit()));
	if (checkError(hr)) {
		D3D12_SHADER_DESC desc;
		hr = reflect->GetDesc(&desc);
		throwIfError(hr);

		axShaderStageInfo	outInfo;
		outInfo.profile = profile;

		{
			_compileReflect_inputs			(outInfo, reflect, desc);
			_compileReflect_uniformBuffers	(outInfo, reflect, desc);
			_compileReflect_textures		(outInfo, reflect, desc);
			_compileReflect_samplers		(outInfo, reflect, desc);
			_compileReflect_storageBuffers	(outInfo, reflect, desc);
		}

		{
			axString jsonFilename;
			jsonFilename.append(outFilename, ".json");
			axJsonUtil::writeFile(jsonFilename, outInfo, false);
		}
	}
}

void axDXILCompiler::_compileReflect_inputs(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.inputs.reserve(desc.InputParameters);

	for (UINT i=0; i<desc.InputParameters; i++) {
		auto& dst = outInfo.inputs.emplaceBack();

		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		hr = reflect->GetInputParameterDesc(i, &paramDesc);
		checkError(hr);

		axTempString attrId;
		attrId.format("{?}{?}", axStrView_c_str(paramDesc.SemanticName), paramDesc.SemanticIndex);

		if (!axStrView_tryParse(attrId, dst.attrId)) {
			AX_LOG("Unknown semantic name {?}", attrId);
			throw axError_Undefined(AX_LOC);
		}

		axString_<128> dataType;

		switch (paramDesc.ComponentType) {
			case D3D_REGISTER_COMPONENT_UINT32:		dataType.append("UInt32");  break;
			case D3D_REGISTER_COMPONENT_SINT32:		dataType.append("Int32");	break;
			case D3D_REGISTER_COMPONENT_FLOAT32:	dataType.append("Float32"); break;
			default: throw axError_Undefined(AX_LOC);
		}

		auto componentCount = ax_bitwise::count1(paramDesc.Mask);
		if (componentCount < 1 || componentCount > 4) {
			throw axError_Undefined(AX_LOC);
		}

		if (componentCount > 1) {
			dataType.appendFormat("x{?}", componentCount);
		}

		if (!axStrView_tryParse(dataType, dst.dataType)) {
			AX_LOG("Error: parse enum {?}", dataType);
			throw axError_Undefined(AX_LOC);
		}

		//wr.member("register", paramDesc.Register);
	}
}

void axDXILCompiler::_compileReflect_uniformBuffers(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.uniformBuffers.reserve(desc.BoundResources);

	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		checkError(hr);

		if (resDesc.Type != D3D_SIT_CBUFFER) continue;

		auto& outCB = outInfo.uniformBuffers.emplaceBack();

		D3D12_SHADER_BUFFER_DESC bufDesc;
		auto* cb = reflect->GetConstantBufferByName(resDesc.Name);
		hr = cb->GetDesc(&bufDesc);
		checkError(hr);

		outCB.name = axStrView_c_str(bufDesc.Name);
		ax_safe_assign(outCB.bindPoint, resDesc.BindPoint);
		ax_safe_assign(outCB.bindCount, resDesc.BindCount);
		ax_safe_assign(outCB.dataSize,  bufDesc.Size);

		{
			outCB.variables.reserve(bufDesc.Variables);
			for (UINT j=0; j<bufDesc.Variables; j++) {
				auto cbv = cb->GetVariableByIndex(j);
				D3D12_SHADER_VARIABLE_DESC varDesc;
				hr = cbv->GetDesc(&varDesc);
				checkError(hr);

				D3D12_SHADER_TYPE_DESC varType;
				hr = cbv->GetType()->GetDesc(&varType);
				checkError(hr);

				if (0 == (varDesc.uFlags & D3D_SVF_USED)) continue;

				auto& outVar = outCB.variables.emplaceBack();
				outVar.name = axStrView_c_str(varDesc.Name);
				ax_safe_assign(outVar.offset,   varDesc.StartOffset);
					
				//------------------------------
				axTempString dataType;
				switch (varType.Type) {
					case D3D_SVT_BOOL:	dataType.append("Bool");	break;
					case D3D_SVT_INT:	dataType.append("Int32");	break;
					case D3D_SVT_UINT:	dataType.append("UInt32");	break;
					case D3D_SVT_UINT8:	dataType.append("UInt8");	break;
					case D3D_SVT_FLOAT: dataType.append("Float32");	break;
					case D3D_SVT_DOUBLE:dataType.append("Float64");	break;
					default: throw axError_Undefined(AX_LOC);
				}

				switch (varType.Class) {
					case D3D_SVC_SCALAR: break;
					case D3D_SVC_VECTOR:			dataType.appendFormat("x{?}", varType.Columns); break;
					case D3D_SVC_MATRIX_COLUMNS:	dataType.appendFormat("_{?}x{?}", varType.Rows, varType.Columns); break;
					case D3D_SVC_MATRIX_ROWS:		dataType.appendFormat("_{?}x{?}", varType.Rows, varType.Columns); break;
					default: throw axError_Undefined(AX_LOC);
				}

				if (!axStrView_tryParse(dataType, outVar.dataType)) {
					AX_LOG("Error: parse enum {?}", dataType);
					throw axError_Undefined(AX_LOC);
				}

				if (outVar.dataType == DataType::Unknown) {
					throw axError_Undefined(AX_LOC);
				}
			}
		}
	}
}

void axDXILCompiler::_compileReflect_textures(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.textures.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		checkError(hr);

		if (resDesc.Type != D3D_SIT_TEXTURE) continue;

		auto& outTex = outInfo.textures.emplaceBack();
		outTex.name = axStrView_c_str(resDesc.Name);		
		ax_safe_assign(outTex.bindPoint, resDesc.BindPoint);
		ax_safe_assign(outTex.bindCount, resDesc.BindCount);

		switch (resDesc.Dimension) {
			case D3D_SRV_DIMENSION_TEXTURE1D:		outTex.dataType = DataType::Texture1D;   break;
			case D3D_SRV_DIMENSION_TEXTURE2D:		outTex.dataType = DataType::Texture2D;   break;
			case D3D_SRV_DIMENSION_TEXTURE3D:		outTex.dataType = DataType::Texture3D;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBE:		outTex.dataType = DataType::TextureCube; break;
		//----
			case D3D_SRV_DIMENSION_TEXTURE1DARRAY:	outTex.dataType = DataType::Texture1DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURE2DARRAY:	outTex.dataType = DataType::Texture2DArray;   break;
			case D3D_SRV_DIMENSION_TEXTURECUBEARRAY:outTex.dataType = DataType::TextureCubeArray; break;
		//----
			default: throw axError_Undefined(AX_LOC);
		}

		//switch (resDesc.ReturnType) {
		//	case D3D_RETURN_TYPE_UNORM:		wr.member("returnType", "UNORM");	 break;
		//	case D3D_RETURN_TYPE_SNORM:		wr.member("returnType", "SNORM");	 break;
		//	case D3D_RETURN_TYPE_SINT:		wr.member("returnType", "SINT");	 break;
		//	case D3D_RETURN_TYPE_UINT:		wr.member("returnType", "UINT");	 break;
		//	case D3D_RETURN_TYPE_FLOAT:		wr.member("returnType", "FLOAT");	 break;
		//	case D3D_RETURN_TYPE_DOUBLE:	wr.member("returnType", "DOUBLE");	 break;
		//	default: throw axError_Undefined(AX_LOC);
		//}
	}
}

void axDXILCompiler::_compileReflect_samplers(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.samplers.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		checkError(hr);

		if (resDesc.Type != D3D_SIT_SAMPLER) continue;

		auto& outSampler = outInfo.samplers.emplaceBack();
		outSampler.name = axStrView_c_str(resDesc.Name);
		ax_safe_assign(outSampler.bindPoint, resDesc.BindPoint);
		ax_safe_assign(outSampler.bindCount, resDesc.BindCount);
	}
}

void axDXILCompiler::_compileReflect_storageBuffers(axShaderStageInfo& outInfo, ID3D12ShaderReflection* reflect, D3D12_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.storageBuffers.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D12_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		checkError(hr);

		if (resDesc.Type != D3D_SIT_STRUCTURED
		 && resDesc.Type != D3D_SIT_UAV_RWSTRUCTURED
		 && resDesc.Type != D3D_SIT_UAV_RWBYTEADDRESS) continue;

		auto& sbuf = outInfo.storageBuffers.emplaceBack();
		sbuf.name = axStrView_c_str(resDesc.Name);
		ax_safe_assign(sbuf.bindPoint, resDesc.BindPoint);
		ax_safe_assign(sbuf.bindCount, resDesc.BindCount);

		sbuf.rawUAV = (resDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS);
	}
}

#endif
