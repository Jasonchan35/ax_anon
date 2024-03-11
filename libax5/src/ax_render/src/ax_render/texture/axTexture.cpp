#include <ax_render/texture/axTexture.h>

axTexture2D::axTexture2D(CreateDesc& desc) {
	_type = DataType::Texture2D;
	_colorType = desc.colorType;
	_mipmapCount = desc.mipmapCount;
	_size = desc.size;
}