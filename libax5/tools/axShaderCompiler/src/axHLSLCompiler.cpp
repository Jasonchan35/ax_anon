#include "axHLSLCompiler.h"

#if AX_OS_WINDOWS

axHLSLCompiler::IncludeHandler::IncludeHandler(axStrView currentFile, axArray<axString>& include_dirs_) 
	: _currentFile(currentFile)
	, include_dirs(include_dirs_)
{
}

HRESULT axHLSLCompiler::IncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes) {

	auto filename = axStrView_c_str(pFileName);
	axStrView dirname;
	
	if (!pParentData) {
		dirname = axPath::dirname(_currentFile);
	}else{
		for (auto& p : _openedFiles) {
			if (p->data() == pParentData) {
				dirname = axPath::dirname(p->filename());
			}
		}

		if (!dirname) {
			throw axError_Undefined(AX_LOC);
		}
	}

	axString fullpath;
	
	if(dirname) {
		fullpath.append(dirname, '/');
	}
	fullpath.append(filename);

	if (!axFile::exists(fullpath)) {
		fullpath.clear();

		for (auto& f : include_dirs) {
			fullpath.set(f, '/', filename);
			if (axFile::exists(fullpath)) {
				break;
			}else{
				fullpath.clear();
			}
		}
	}

	if (!fullpath) {
		return S_FALSE;
	}

	auto* newFile = _openedFiles.emplaceBack(fullpath);
	*ppData = newFile->data();
	*pBytes = static_cast<UINT>(newFile->size());

	return S_OK;
}

HRESULT axHLSLCompiler::IncludeHandler::Close(LPCVOID pData) {
	return S_OK;
}

void axHLSLCompiler::IncludeHandler::writeDepFile(axStrView filename) {
	axString outFilename;
	outFilename.set(filename, ".dep");

	axString o;
	o.append(filename, ":\\\n");

	for (auto& f : _openedFiles) {
		o.append("\t", f->filename(), "\\\n");
	}
	o.append("\n");

	axFile::writeFileIfChanged(outFilename, o, false);
}

void axHLSLCompiler::compile(axStrView outFilename, axStrView filename_, axStrView profile_, axStrView entryFunc_, axArray<axString>& include_dirs) {
	axString filename(filename_);
	axString entryFunc(entryFunc_);
	axString profile(profile_);

	axMemMap mm(filename_);

	auto hlsl = axStrView_make(mm);

	axComPtr<ID3DBlob>	byteCode;
	axComPtr<ID3DBlob>	errorMsg;

	axDict<axString, axString>	macroDefines;

	axArray<D3D_SHADER_MACRO> macroArray;

	for (auto& p : macroDefines) {
		D3D_SHADER_MACRO e;
		e.Name = p.key().c_str();
		e.Definition = p.value().c_str();

		macroArray.append(e);
	}

	{
		D3D_SHADER_MACRO e;
		e.Name = nullptr;
		e.Definition = nullptr;
		macroArray.append(e);
	}

	IncludeHandler includeHandler(filename, include_dirs);

	UINT flags1 = 0; // D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	UINT flags2 = 0;

#if _DEBUG
	flags1 |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	auto hr = D3DCompile2(	hlsl.data(), hlsl.size(), filename.c_str(),
							macroArray.data(), &includeHandler,
							entryFunc.c_str(), profile.c_str(),
							flags1, flags2,
							0, nullptr, 0,
							byteCode.ptrForInit(), errorMsg.ptrForInit());
	if (FAILED(hr)) {
		axStrView msg(reinterpret_cast<const char*>(errorMsg->GetBufferPointer()), errorMsg->GetBufferSize());
		AX_LOG("{?}", msg);

		throw axError_Undefined(AX_LOC);
	}

	includeHandler.writeDepFile(outFilename);

	axByteSpan byteCodeView(reinterpret_cast<const axByte*>(byteCode->GetBufferPointer()), byteCode->GetBufferSize());

	axFile::writeFile(outFilename, byteCodeView, false);
	_compileReflect(outFilename, byteCodeView, profile);
}

void axHLSLCompiler::_compileReflect(axStrView outFilename, axByteSpan byteCodeView, axStrView profile) {
	axComPtr<ID3D11ShaderReflection>	reflect;
	auto hr = D3DReflect(byteCodeView.data(), byteCodeView.size(), IID_PPV_ARGS(reflect.ptrForInit()));
	throwIfError(hr);

	D3D11_SHADER_DESC desc;
	hr = reflect->GetDesc(&desc);
	throwIfError(hr);

	axShaderStageInfo outInfo;
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

void axHLSLCompiler::_compileReflect_inputs(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.inputs.reserve(desc.InputParameters);

	for (UINT i=0; i<desc.InputParameters; i++) {
		auto& dst = outInfo.inputs.emplaceBack();

		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		hr = reflect->GetInputParameterDesc(i, &paramDesc);
		throwIfError(hr);

		axTempString attrId;
		attrId.format("{?}{?}", axStrView_c_str(paramDesc.SemanticName), paramDesc.SemanticIndex);
		if (!axStrView_tryParse(attrId, dst.attrId)) {
			throw axError_Undefined(AX_LOC);
		}

		axString_<128> dataType;

		switch (paramDesc.ComponentType) {
			case D3D_REGISTER_COMPONENT_UINT32:		dataType.append("UInt8");	break;
			case D3D_REGISTER_COMPONENT_SINT32:		dataType.append("Int32");	break;
			case D3D_REGISTER_COMPONENT_FLOAT32:	dataType.append("Float32");	break;
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

void axHLSLCompiler::_compileReflect_uniformBuffers(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.uniformBuffers.reserve(desc.BoundResources);

	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_CBUFFER) continue;

		auto& outCB = outInfo.uniformBuffers.emplaceBack();

		D3D11_SHADER_BUFFER_DESC bufDesc;
		auto cb = reflect->GetConstantBufferByName(resDesc.Name);
		hr = cb->GetDesc(&bufDesc);
		throwIfError(hr);

		outCB.name = axStrView_c_str(bufDesc.Name);
		ax_safe_assign(outCB.bindPoint, resDesc.BindPoint);
		ax_safe_assign(outCB.bindCount, resDesc.BindCount);
		ax_safe_assign(outCB.dataSize,  bufDesc.Size);

		{
			outCB.variables.reserve(bufDesc.Variables);
			for (UINT j=0; j<bufDesc.Variables; j++) {
				auto cbv = cb->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC varDesc;
				hr = cbv->GetDesc(&varDesc);
				throwIfError(hr);

				D3D11_SHADER_TYPE_DESC varType;
				hr = cbv->GetType()->GetDesc(&varType);
				throwIfError(hr);

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
					case D3D_SVT_UINT8:	dataType.append("Uint8");	break;
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

void axHLSLCompiler::_compileReflect_textures(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	HRESULT hr;

	outInfo.textures.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

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
		//	case D3D_RETURN_TYPE_UNORM:		wr.writeMember("returnType", "UNORM");	 break;
		//	case D3D_RETURN_TYPE_SNORM:		wr.writeMember("returnType", "SNORM");	 break;
		//	case D3D_RETURN_TYPE_SINT:		wr.writeMember("returnType", "SINT");	 break;
		//	case D3D_RETURN_TYPE_UINT:		wr.writeMember("returnType", "UINT");	 break;
		//	case D3D_RETURN_TYPE_FLOAT:		wr.writeMember("returnType", "FLOAT");	 break;
		//	case D3D_RETURN_TYPE_DOUBLE:	wr.writeMember("returnType", "DOUBLE");	 break;
		//	default: throw axError_Undefined(AX_LOC);
		//}
	}
}

void axHLSLCompiler::_compileReflect_samplers(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.samplers.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

		if (resDesc.Type != D3D_SIT_SAMPLER) continue;

		auto& outSampler = outInfo.samplers.emplaceBack();
		outSampler.name = axStrView_c_str(resDesc.Name);
		ax_safe_assign(outSampler.bindPoint, resDesc.BindPoint);
		ax_safe_assign(outSampler.bindCount, resDesc.BindCount);
	}
}

void axHLSLCompiler::_compileReflect_storageBuffers(axShaderStageInfo& outInfo, ID3D11ShaderReflection* reflect, D3D11_SHADER_DESC& desc) {
	HRESULT hr;
	outInfo.storageBuffers.reserve(desc.BoundResources);
	for (UINT i=0; i<desc.BoundResources; i++) {
		D3D11_SHADER_INPUT_BIND_DESC resDesc;
		hr = reflect->GetResourceBindingDesc(i, &resDesc);
		throwIfError(hr);

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

void axHLSLCompiler::throwIfError(HRESULT hr) {
	if (FAILED(hr)) {
		throw axError_Undefined(AX_LOC);
	}
}

#endif //AX_OS_WINDOWS