#pragma once
#include "axRenderVertexDesc.h"


class alignas(axRenderVertex_kAlignment) axRenderVertexBase_ {
	using This = axRenderVertexBase_;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = void;
	uint8_t _padding[axRenderVertex_kAlignment];

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
	}
};

inline axRenderVertexDesc axRenderVertexBase_::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_Pos {
	using This = axRenderVertexBase_Pos;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
	}
};

template<class POS>
inline axRenderVertexDesc axRenderVertexBase_Pos<POS>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosCustom {
	using This = axRenderVertexBase_PosCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosCustom<POS, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUv {
	using This = axRenderVertexBase_PosUv;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
	}
};

template<class POS, class UV, axInt UV_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUv<POS, UV, UV_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvCustom {
	using This = axRenderVertexBase_PosUvCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvCustom<POS, UV, UV_COUNT, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColor {
	using This = axRenderVertexBase_PosColor;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosColor<POS, COLOR, COLOR_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorCustom {
	using This = axRenderVertexBase_PosColorCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosColorCustom<POS, COLOR, COLOR_COUNT, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColor {
	using This = axRenderVertexBase_PosUvColor;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvColor<POS, UV, UV_COUNT, COLOR, COLOR_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorCustom {
	using This = axRenderVertexBase_PosUvColorCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = void;
	static constexpr NormalType kNormalType = NormalType::None;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorCustom<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormal {
	using This = axRenderVertexBase_PosNormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
	}
};

template<class POS, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosNormal<POS, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormalCustom {
	using This = axRenderVertexBase_PosNormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosNormalCustom<POS, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormal {
	using This = axRenderVertexBase_PosUvNormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormal<POS, UV, UV_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormalCustom {
	using This = axRenderVertexBase_PosUvNormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormalCustom<POS, UV, UV_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormal {
	using This = axRenderVertexBase_PosColorNormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormal<POS, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormalCustom {
	using This = axRenderVertexBase_PosColorNormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormalCustom<POS, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormal {
	using This = axRenderVertexBase_PosUvColorNormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormal<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormalCustom {
	using This = axRenderVertexBase_PosUvColorNormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::Normal;
	Normal normal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormalCustom<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormalTangent {
	using This = axRenderVertexBase_PosNormalTangent;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
	}
};

template<class POS, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosNormalTangent<POS, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormalTangentCustom {
	using This = axRenderVertexBase_PosNormalTangentCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosNormalTangentCustom<POS, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormalTangent {
	using This = axRenderVertexBase_PosUvNormalTangent;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormalTangent<POS, UV, UV_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormalTangentCustom {
	using This = axRenderVertexBase_PosUvNormalTangentCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormalTangentCustom<POS, UV, UV_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormalTangent {
	using This = axRenderVertexBase_PosColorNormalTangent;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormalTangent<POS, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormalTangentCustom {
	using This = axRenderVertexBase_PosColorNormalTangentCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormalTangentCustom<POS, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormalTangent {
	using This = axRenderVertexBase_PosUvColorNormalTangent;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormalTangent<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormalTangentCustom {
	using This = axRenderVertexBase_PosUvColorNormalTangentCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangent;
	Normal normal;
	Normal tangent;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormalTangentCustom<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormalTangentBinormal {
	using This = axRenderVertexBase_PosNormalTangentBinormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
	}
};

template<class POS, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosNormalTangentBinormal<POS, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosNormalTangentBinormalCustom {
	using This = axRenderVertexBase_PosNormalTangentBinormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosNormalTangentBinormalCustom<POS, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormalTangentBinormal {
	using This = axRenderVertexBase_PosUvNormalTangentBinormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormalTangentBinormal<POS, UV, UV_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvNormalTangentBinormalCustom {
	using This = axRenderVertexBase_PosUvNormalTangentBinormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = void;
	static constexpr axInt kColorCount = 0;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvNormalTangentBinormalCustom<POS, UV, UV_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormalTangentBinormal {
	using This = axRenderVertexBase_PosColorNormalTangentBinormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormalTangentBinormal<POS, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosColorNormalTangentBinormalCustom {
	using This = axRenderVertexBase_PosColorNormalTangentBinormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = void;
	static constexpr axInt kUvCount = 0;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosColorNormalTangentBinormalCustom<POS, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormalTangentBinormal {
	using This = axRenderVertexBase_PosUvColorNormalTangentBinormal;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = void;
	static constexpr axInt kCustomCount = 0;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormalTangentBinormal<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
class alignas(axRenderVertex_kAlignment) axRenderVertexBase_PosUvColorNormalTangentBinormalCustom {
	using This = axRenderVertexBase_PosUvColorNormalTangentBinormalCustom;
public:
	using NormalType = axRenderNormalType;

	using VertexType = axRenderVertexType;

	using Pos = POS;
	Pos pos;

	using Uv = UV;
	static constexpr axInt kUvCount = UV_COUNT;
	axFixedArray<Uv, kUvCount> uv;

	using Color = COLOR;
	static constexpr axInt kColorCount = COLOR_COUNT;
	axFixedArray<Color, kColorCount> color;

	using Normal = NORMAL;
	static constexpr NormalType kNormalType = NormalType::NormalTangentBinormal;
	Normal normal;
	Normal tangent;
	Normal binormal;

	using Custom = CUSTOM;
	static constexpr axInt kCustomCount = CUSTOM_COUNT;
	axFixedArray<Custom, kCustomCount> custom;

	static constexpr VertexType kType
		= axRenderVertexType_makeT<
			Pos,
			Uv, kUvCount,
			Color, kColorCount,
			Normal, kNormalType,
			Custom, kCustomCount>();

	static axRenderVertexDesc s_desc();
	void onFormat(axFormat& f) const {
		f << "\n  pos      =" << pos;
		f << "\n  uv       =" << uv;
		f << "\n  color    =" << color;
		f << "\n  normal   =" << normal;
		f << "\n  tangent  =" << tangent;
		f << "\n  binormal =" << binormal;
		f << "\n  custom   =" << custom;
	}
};

template<class POS, class UV, axInt UV_COUNT, class COLOR, axInt COLOR_COUNT, class NORMAL, class CUSTOM, axInt CUSTOM_COUNT>
inline axRenderVertexDesc axRenderVertexBase_PosUvColorNormalTangentBinormalCustom<POS, UV, UV_COUNT, COLOR, COLOR_COUNT, NORMAL, CUSTOM, CUSTOM_COUNT>::s_desc() {
	static axRenderVertexDescMakeT<This> s;
	return &s;
}

