#pragma once

#include "axColorRGBA.h"

template<class T>
class axColorL_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 1;
	static constexpr axColorType kColorType = axColorType_L<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::L;
	static constexpr axInt kAlphaBits = 0;

	T luma;

	AX_STRUCT_BEGIN(axColorL_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(luma)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---
	static constexpr This kBlack	() { return This(kElemMin()); }
	static constexpr This kWhite	() { return This(kElemMax()); }
	static constexpr This kGray		() { return This(kElemHalf()); }
//---

	AX_INLINE explicit axColorL_() = default;
	AX_INLINE          axColorL_(T luma_) { set(luma_); }

	AX_INLINE		 T* data()			{ return &luma; }
	AX_INLINE const T* data() const	{ return &luma; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(luma); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(luma); }

	AX_INLINE void set(T luma_) { luma=luma_; }

	AX_INLINE axColorRGB_<T>	rgb() const			{ return axColorRGB_<T>(luma, luma, luma); }
	AX_INLINE axColorRGBA_<T>	rgba() const		{ return axColorRGBA_<T>(luma, luma, luma); }
	AX_INLINE axColorLA_<T>	toColorLA() const;

	AX_INLINE This	operator+	(const This& rhs) const { return This(luma+rhs.luma); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(luma-rhs.luma); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(luma,rhs.luma)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(luma,rhs.luma)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return luma == rhs.luma; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return luma != rhs.luma; }

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.toColorL());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorL_<SRC_E>, SRC_E> {
		static This make(const axColorL_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.luma)
			);
		}
	};

	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorL_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorL_<T> & v) {
	auto span = axMutSpan_make(v.data);
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorL_<T> & v) {
	auto span = axMutSpan_make(v.data);
	se.io(span);
}

template<class T> AX_INLINE
axColorL_<T>	 axColorRGBA_<T>::toColorL() const	{ return axColorL_<T>(luma()); }

template<class T> AX_INLINE
axColorL_<T>	 axColorRGB_<T>::toColorL() const	{ return axColorL_<T>(luma()); }
