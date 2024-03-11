#pragma once

#include "../axRenderResource.h"

class axRenderGpuBuffer_CreateDesc {
public:
	using Type = axRenderBufferType;
	Type	type = Type::Unknown;
	axInt	byteStride = 16;
};

class axRenderGpuBuffer : public axRenderResource {
	AX_RTTI_CLASS(axRenderGpuBuffer, axRenderResource)
public:
	using Type = axRenderBufferType;
	using CreateDesc = axRenderGpuBuffer_CreateDesc;

	void create(const CreateDesc& desc, axInt bufferSize) {
		_desc = desc;
		_bufferSize = bufferSize;
		onCreate(desc, bufferSize);
	}

	void createWithData(const CreateDesc& desc, axByteSpan data) {
		create(desc, data.sizeInBytes());
		uploadToGpu(data);
	}

	void uploadToGpu(axInt offset, axByteSpan data) {
		if (offset < 0 || offset + data.size() > _bufferSize)
			throw axError_Undefined(AX_LOC);

		onUploadToGpu(offset, data);
	}

	void uploadToGpu(axByteSpan data) {
		uploadToGpu(0, data);
	}

	Type  type() const { return _desc.type; }
	axInt bufferSize() const { return _bufferSize; }

protected:
	virtual void onCreate(const CreateDesc& desc, axInt bufferSize) = 0;
	virtual void onUploadToGpu(axInt offset, axByteSpan data) = 0;

private:
	CreateDesc	_desc;
	axInt		_bufferSize = 0;
};
