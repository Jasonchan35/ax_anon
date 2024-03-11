#pragma once

class axColorBC1 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC1;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 1;
	static constexpr axInt kBytesPerPixelBlock = 8;
};

class axColorBC2 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC2;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 4;
	static constexpr axInt kBytesPerPixelBlock = 16;
};

class axColorBC3 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC3;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 8;
	static constexpr axInt kBytesPerPixelBlock = 16;
};

class axColorBC4 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC4;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 0;
	static constexpr axInt kBytesPerPixelBlock = 8;
};

class axColorBC5 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC5;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 0;
	static constexpr axInt kBytesPerPixelBlock = 16;
};

class axColorBC6h {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC6h;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 0;
	static constexpr axInt kBytesPerPixelBlock = 16;
};

class axColorBC7 {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::BC7;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 8;
	static constexpr axInt kBytesPerPixelBlock = 16;
};

class axColorETC {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::ETC;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 0;
	static constexpr axInt kBytesPerPixelBlock = 0;
};

class axColorPVRTC {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::PVRTC;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 4;
	static constexpr axInt kBytesPerPixelBlock = 0;
};

class axColorASTC {
public:
	using Element = void;
	static constexpr axColorType kColorType = axColorType::ASTC;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits	= 8;
	static constexpr axInt kBytesPerPixelBlock = 0;
};



