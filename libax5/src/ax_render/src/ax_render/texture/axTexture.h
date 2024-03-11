#pragma once

#include <ax_render/axRenderResource.h>
#include <ax_render/image/axImage.h>
#include "axSamplerState.h"

class axTexture_CreateDesc {
public:
	axString	name;
	axColorType colorType   = axColorType::RGBAb;
	axInt		mipmapCount = 1;
};

class axTexture : public axRenderResource {
	AX_RTTI_CLASS(axTexture, axRenderResource)
public:
	using CreateDesc = axTexture_CreateDesc;
	using DataType = axRenderDataType;

	DataType type() const { return _type; }

protected:
	axTexture() = default;

	DataType _type = DataType::Unknown;
};

//---------------------------

class axTexture2D_UploadRequest : public axNonCopyable {
public:	
	axVec2i			offset{0,0};
	axImageInfo		imageInfo;
	axByteSpan		pixelData;
	axSPtr<axTexture2D> tex;
};

class axTexture2D_CreateDesc : public axTexture_CreateDesc {
public:
	axVec2i			size;
	axSamplerState	samplerState;
	axTexture2D_UploadRequest* uploadRequest = nullptr;
};

class axTexture2D : public axTexture {
	AX_RTTI_CLASS(axTexture2D, axTexture)
public:
	using CreateDesc = axTexture2D_CreateDesc;
	using UploadRequest = axTexture2D_UploadRequest;
	
	axColorType	colorType	() const	{ return _colorType; }
	axInt		mipmapCount	() const	{ return _mipmapCount; }
	axInt		width		() const	{ return _size.x; }
	axInt		height		() const	{ return _size.y; }
	axVec2i		size		() const	{ return _size; }

protected:
	axTexture2D(CreateDesc& desc); // please create from axRenderer::createTexture2D()

	axColorType _colorType;
	axInt	_mipmapCount = 0;
	axVec2i	_size {0,0};
};
