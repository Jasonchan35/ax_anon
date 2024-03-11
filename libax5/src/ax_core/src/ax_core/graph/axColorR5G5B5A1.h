#pragma once

class axColorR5G5B5A1 {
public:
	using Element = void;

	static constexpr axColorType kColorType = axColorType::R5G5B5A1;
	static constexpr axColorModel kColorModel = axColorModel::Compressed;
	static constexpr axInt kAlphaBits = 1;

	axUInt16 value;
};