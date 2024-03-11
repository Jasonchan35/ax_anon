#if AX_USE_DX11

#include "axDX11MaterialPass.h"
#include "axDX11StorageBuffer.h"
#include "axDX11Texture.h"
#include "axDX11RenderContext.h"
#include "axDX11CommandDispatcher.h"

void axDX11MaterialPass::bind(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(dispatcher, drawCall);
	_bindParams(dispatcher);
}

void axDX11MaterialPass::bind(axDX11CommandDispatcher& dispatcher, ComputeCall& compteCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->bind(dispatcher, compteCall);
	_bindParams(dispatcher);
}

void axDX11MaterialPass::unbind(axDX11CommandDispatcher& dispatcher, DrawCall& drawCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->unbind(dispatcher, drawCall);
	_unbindParams(dispatcher);
}

void axDX11MaterialPass::unbind(axDX11CommandDispatcher& dispatcher, ComputeCall& compteCall) {
	auto* shaderPass_ = shaderPass();
	if (!shaderPass_) throw axError_Undefined(AX_LOC);
	shaderPass_->unbind(dispatcher, compteCall);
	_unbindParams(dispatcher);
}

void axDX11MaterialPass::_bindParams(axDX11CommandDispatcher& dispatcher) {
	{ //const buffer
//		bool uploadNeeded = pass.dataChanged;
		axInt cbIndex = 0;
		for (auto& cb : _uniformBuffers) {
			UINT bindPoint = Util::castUINT(cb.bindPoint());

			auto* gpuBuf = ax_type_cast_debug<axDX11GpuBuffer>(cb.currentGpuBuffer());
			if (!gpuBuf) throw axError_Undefined(AX_LOC);

			ID3D11Buffer* d3dBuf = gpuBuf->_d3dBuf;

			auto stage = ax_enum(cb.stageFlags());
			if (stage.hasFlags(StageFlags::Vertex	)) dispatcher.d3dContext->VSSetConstantBuffers(bindPoint, 1, &d3dBuf);
			if (stage.hasFlags(StageFlags::Pixel	)) dispatcher.d3dContext->PSSetConstantBuffers(bindPoint, 1, &d3dBuf);
			if (stage.hasFlags(StageFlags::Compute	)) dispatcher.d3dContext->CSSetConstantBuffers(bindPoint, 1, &d3dBuf);
			cbIndex++;
		}
	}

	{ //bind texture
		for (auto& texParam : _texParams) {
			auto* tex = texParam.getUpdatedTexture();
			switch (tex->type()) {
				case DataType::Texture2D: {
					auto* tex2d = ax_type_cast_debug<axDX11Texture2D>(tex);
					if (!tex2d) throw axError_Undefined(AX_LOC);

					auto* rv = tex2d->_resourceView.ptr();
					auto* ss = tex2d->_samplerState.ptr();

					auto stage = ax_enum(texParam.stageFlags());
					if (stage.hasFlags(StageFlags::Vertex)) {
						dispatcher.d3dContext->VSSetShaderResources(0, 1, &rv);
						dispatcher.d3dContext->VSSetSamplers(0, 1, &ss);
					}

					if (stage.hasFlags(StageFlags::Pixel)) {
						dispatcher.d3dContext->PSSetShaderResources(0, 1, &rv);
						dispatcher.d3dContext->PSSetSamplers(0, 1, &ss);
					}

					if (stage.hasFlags(StageFlags::Compute)) {
						dispatcher.d3dContext->CSSetShaderResources(0, 1, &rv);
						dispatcher.d3dContext->CSSetSamplers(0, 1, &ss);
					}
				}break;

				default: throw axError_Undefined(AX_LOC);
			}
		}
	}

	{ // bind storage buffers
		for (auto& bufParam : _storageBufParams) {
			auto* sbuf = ax_type_cast_debug<axDX11StorageBuffer>(bufParam.storageBuffer());
			if (!sbuf) continue;

			auto stage = ax_enum(bufParam.stageFlags());
			auto* uav = sbuf->_uav.ptr();
			if (stage.hasFlags(StageFlags::Compute)) {
				dispatcher.d3dContext->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
			}			
		}
	}
}

void axDX11MaterialPass::_unbindParams(axDX11CommandDispatcher& dispatcher) {
	{ // bind storage buffers
		for (auto& bufParam : _storageBufParams) {
			auto* sbuf = ax_type_cast_debug<axDX11StorageBuffer>(bufParam.storageBuffer());
			if (!sbuf) continue;

			auto stage = ax_enum(bufParam.stageFlags());
			ID3D11UnorderedAccessView* uav[] = {nullptr};
			if (stage.hasFlags(StageFlags::Compute)) {
				dispatcher.d3dContext->CSSetUnorderedAccessViews(0, 1, uav, nullptr);
			}			
		}
	}
}

#endif
