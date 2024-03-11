#pragma once

template<class COLOR> class axImagePainter_;

class axImageInfo {
public:
	axVec2i		size {0,0};
	axInt		strideInBytes = 0;
	axInt		mipmapCount = 1;
	axInt		pixelSizeInBytes() const { return axColorUtil::pixelSizeInBytes(colorType); }
	axColorType	colorType = axColorType::Unknown;
};


//! Image for any color type
class axImage : public axNonCopyable {
public:
	using Info = axImageInfo;

	axImage() = default;
	axImage(axImage && r);

	void operator=(axImage && r);

					void		clear();

					void		loadFile	(axStrView filename);

					void		loadPngFile	(axStrView filename);
					void		loadPngMem	(axByteSpan data);

					void		loadJpegFile(axStrView filename);
					void		loadJpegMem	(axByteSpan data);

					void		loadDdsFile	(axStrView filename);
					void		loadDdsMem	(axByteSpan data);

					void		create		(axColorType colorType, axInt width, axInt height);
					void		create		(axColorType colorType, axInt width, axInt height, axInt strideInBytes);
					void		create		(axColorType colorType, axInt width, axInt height, axInt strideInBytes, axInt mipmapCount, axInt pixelDataSize);

	AX_INLINE	const Info&		info			() const { return _info; }
	AX_INLINE	const axVec2i&	size			() const { return _info.size; }
	AX_INLINE	axInt			strideInBytes	() const { return _info.strideInBytes; }
	AX_INLINE	axInt			width			() const { return _info.size.x; }
	AX_INLINE	axInt			height			() const { return _info.size.y; }
	AX_INLINE	axColorType		colorType		() const { return _info.colorType; }

					void		subImage	(axVec2i pos, const axImageInfo& srcInfo, axByteSpan srcPixelData);

					void		copy		(const axImage& srcImage);

	template<class COLOR> AX_INLINE	axMutSpan<COLOR>	row(axInt y)					{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }
	template<class COLOR> AX_INLINE	axSpan<COLOR>		row(axInt y) const				{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }

	template<class COLOR> AX_INLINE	const COLOR&		pixel(axInt x, axInt y) const	{ return row<COLOR>(y)[x]; }
	template<class COLOR> AX_INLINE		  COLOR&		pixel(axInt x, axInt y)			{ return row<COLOR>(y)[x]; }

	AX_INLINE	axByteMutSpan		rowBuffer(axInt y)			{ return axByteMutSpan(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }
	AX_INLINE	axByteSpan			rowBuffer(axInt y) const	{ return    axByteSpan(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }

	AX_INLINE	axByteMutSpan		pixelData()					{ return _pixelData; }
	AX_INLINE	axByteSpan			pixelData() const			{ return _pixelData; }
	
	void convertToPremultipiedAlpha();

	template<class COLOR> AX_INLINE axMutSpan<COLOR>	row_noCheck(axInt y)		{ return axMutSpan<COLOR>(reinterpret_cast<      COLOR*>(rowBuffer(y).data()), _info.size.x); }
	template<class COLOR> AX_INLINE    axSpan<COLOR>	row_noCheck(axInt y) const	{ return    axSpan<COLOR>(reinterpret_cast<const COLOR*>(rowBuffer(y).data()), _info.size.x); }

	template<class COLOR> void fill(const COLOR& color);
	template<class COLOR> axImagePainter_<COLOR> painter();

private:
	void _checkType(axColorType colorType) const {
		if (colorType != _info.colorType) throw axError_Undefined(AX_LOC);
	}

	Info _info;
	axByteArray	_pixelData;
};

//! Image for specific color type
template<class COLOR>
class axImage_ : public axImage {
	using Base = axImage;
public:
	using Color = COLOR;

	void create(axInt width, axInt height, axInt strideInBytes)	{ Base::create(Color::kColorType, width, height, strideInBytes); }
	void create(axInt width, axInt height)						{ Base::create(Color::kColorType, width, height); }

	axMutSpan<COLOR>	row(axInt y)				{ return Base::row<COLOR>(y); }
	   axSpan<COLOR>	row(axInt y) const			{ return Base::row<COLOR>(y); }

	const COLOR&		pixel(axInt x, axInt y) const	{ return Base::pixel<COLOR>(y)[x]; }
		  COLOR&		pixel(axInt x, axInt y)			{ return Base::pixel<COLOR>(y)[x]; }

		void			fill(const Color& color)	{ Base::fill<COLOR>(color);	}

	axImagePainter_<COLOR> painter() { return Base::painter<COLOR>(); }
};

template<class COLOR> inline
void axImage::fill(const COLOR& color) {
	_checkType(COLOR::kColorType);
	for (axInt y=0; y<_info.size.y; y++) {
		row_noCheck<COLOR>(y).fillValues(color);
	}
}

#define AX_MACRO_OP(E, ...)                                   \
	using axImage##E		= axImage_<          axColor##E>; \
//---
	axColorType_EnumList(AX_MACRO_OP);
#undef AX_MACRO_OP
