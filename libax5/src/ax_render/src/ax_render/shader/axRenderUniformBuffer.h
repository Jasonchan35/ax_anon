#pragma once

#include <ax_render/axRenderResource.h>

// Const Buffer (DirectX) or Uniform Buffer (OpenGL)
class axRenderUniformBuffer : public axRenderResource {
	AX_RTTI_CLASS(axRenderUniformBuffer, axRenderResource)
public:
	axRenderUniformBuffer() = default;

	void uploadToGpu(axByteSpan data) {
		onUploadToGpu(data);
		_dataSize = data.sizeInBytes();
	}

	void uploadToGpu(axByteSpan data, const axBBox1i& range) {
		// TODO range
		onUploadToGpu(data);
		_dataSize = data.sizeInBytes();
	}

	virtual void onUploadToGpu(axByteSpan data) = 0;

	axInt dataSize() const { return _dataSize; }

private:
	axInt _dataSize = 0;
};
