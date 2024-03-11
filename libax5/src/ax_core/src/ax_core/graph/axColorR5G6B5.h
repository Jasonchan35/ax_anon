#pragma once

class axColorR5G6B5 {
public:
	using Element = void;
	using ElemUtil = axColorElementUtil;

	static constexpr axColorType kColorType = axColorType::R5G6B5;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits = 0;

	axUInt16 value;
};