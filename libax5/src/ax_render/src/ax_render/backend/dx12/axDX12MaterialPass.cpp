#if AX_USE_DX12

#include "axDX12MaterialPass.h"
#include "axDX12StorageBuffer.h"
#include "axDX12Texture.h"

void axDX12MaterialPass::bind(ID3D12GraphicsCommandList* cmdList, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(cmdList, drawCall);
	_bindParams(cmdList);
}

void axDX12MaterialPass::bind(ID3D12GraphicsCommandList* cmdList, ComputeCall& computeCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(cmdList, computeCall);
	_bindParams(cmdList);
}

void axDX12MaterialPass::_bindParams(ID3D12GraphicsCommandList* cmdList) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);

	{ //const buffer
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			auto* gpuBuf = ax_type_cast_debug<axDX12GpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

			if (shaderPass_->isCompute()) {
				cmdList->SetComputeRootConstantBufferView(Util::castUINT(cbIndex), gpuBuf->gpuAddress());
			} else {
				cmdList->SetGraphicsRootConstantBufferView(Util::castUINT(cbIndex), gpuBuf->gpuAddress());
			}
			cbIndex++;
		}
	}

	auto texCount = _texParams.size();
	if (texCount > 0) {
		_texDescHeap.init(texCount);
		_samplerDescHeap.init(texCount);

		ID3D12DescriptorHeap* ppHeaps[] = {
			_texDescHeap.d3dHeap(),
			_samplerDescHeap.d3dHeap()
		};
		cmdList->SetDescriptorHeaps(ARRAYSIZE(ppHeaps), ppHeaps);

		if (shaderPass_->isCompute()) {
			cmdList->SetComputeRootDescriptorTable(shaderPass_->_psoTexDescTable._rootParamIndex,         _texDescHeap.heapStartHandle());
			cmdList->SetComputeRootDescriptorTable(shaderPass_->_psoSamplerDescTable._rootParamIndex, _samplerDescHeap.heapStartHandle());
		} else {
			cmdList->SetGraphicsRootDescriptorTable(shaderPass_->_psoTexDescTable._rootParamIndex,         _texDescHeap.heapStartHandle());
			cmdList->SetGraphicsRootDescriptorTable(shaderPass_->_psoSamplerDescTable._rootParamIndex, _samplerDescHeap.heapStartHandle());
		}

		//bind textures
		axInt texParamIndex = 0;
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axDX12Texture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);

					_texDescHeap.createViewSRV(texParamIndex, tex2d->_tex);

					auto& ss = tex2d->_samplerState;

					D3D12_SAMPLER_DESC desc = {};
					desc.Filter	  = Util::getDxTextureFilter(ss.filter);
					desc.AddressU = Util::getDxTextureWrap(ss.wrapU);
					desc.AddressV = Util::getDxTextureWrap(ss.wrapV);
					desc.AddressW = Util::getDxTextureWrap(ss.wrapW);

					_samplerDescHeap.createSampler(texParamIndex, desc);
				}break;

				default: throw axError_Undefined(AX_LOC);
			}
			texParamIndex++;
		}
	}

	if (_storageBufParams.size() > 0 ) {
		_storageBufDescHeap.init(_storageBufParams.size());
		auto* p = _storageBufDescHeap.d3dHeap();
		cmdList->SetDescriptorHeaps(1, &p);

		if (shaderPass_->isCompute()) {
			cmdList->SetComputeRootDescriptorTable(shaderPass_->_psoStorageBufDescTable._rootParamIndex,  _storageBufDescHeap.heapStartHandle());
		} else {
			cmdList->SetGraphicsRootDescriptorTable(shaderPass_->_psoStorageBufDescTable._rootParamIndex, _storageBufDescHeap.heapStartHandle());
		}

		axInt sbIndex = 0;
		for (auto& sb : _storageBufParams) {
			auto* sbuf = ax_type_cast_debug<axDX12StorageBuffer>(sb.storageBuffer());
			if (!sbuf) throw axError_Undefined(AX_LOC);

			auto* gpuBuf = sbuf->gpuBuffer();
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			gpuBuf->resource().resourceBarrier(cmdList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			if (sb.rawUAV()) {
				_storageBufDescHeap.createViewRawUAV(sbIndex, gpuBuf->resource());
			} else {
				_storageBufDescHeap.createViewStructuredUAV(sbIndex, gpuBuf->resource());
			}

			sbIndex++;
		}
	}
}

void axDX12MaterialPass::onSetShaderPass(axShaderPass* shaderPass) {
}

#endif
