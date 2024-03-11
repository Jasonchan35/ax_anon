#pragma once

#include "axColorL.h"
#include "axColorA.h"

template<class T>
class axColorLA_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 2;
	static constexpr axColorType kColorType = axColorType_LA<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::LA;
	static constexpr axInt kAlphaBits = ax_sizeof(T);

	T luma, a;

	AX_STRUCT_BEGIN(axColorLA_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(luma, a)
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

	AX_INLINE explicit axColorLA_() = default;
	AX_INLINE          axColorLA_(T luma_, T a_ = kElemMax()) { set(luma_,a_); }

	AX_INLINE		 T* data()			{ return &luma; }
	AX_INLINE const T* data() const	{ return &luma; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&luma, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&luma, kElementCount); }

	AX_INLINE void set(T luma_, T a_ = kElemMax()) { luma=luma_; a=a_; }

	AX_INLINE axColorRGB_<T>	rgb() const		{ return axColorRGB_<T>(luma, luma, luma); }
	AX_INLINE axColorRGBA_<T>	rgba() const	{ return axColorRGBA_<T>(luma, luma, luma, a); }

	AX_INLINE This	operator+	(const This& rhs) const { return This(luma+rhs.luma, a+rhs.a); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(luma-rhs.luma, a-rhs.a); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(luma,rhs.luma), _mul(a,rhs.a)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(luma,rhs.luma), _div(a,rhs.a)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return luma == rhs.luma && a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return luma != rhs.luma || a != rhs.a; }

	This toPremultipliedAlpha	() const {
		auto scalar = ElemUtil::toDouble(a);
		return This(ElemUtil::mulDouble(luma, scalar), a);
	}
	This toNonPremultipliedAlpha() const {
		auto scalar = ax_div(1.0, ElemUtil::toDouble(a));
		return This(ElemUtil::mulDouble(luma, scalar), a);
	}

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.toColorLA());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorLA_<SRC_E>, SRC_E> {
		static This make(const axColorLA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.luma),
				ElemUtil::to<T>(src.a)
			);
		}
	};

private:
	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorLA_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorLA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorLA_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> AX_INLINE
axColorLA_<T>	 axColorRGBA_<T>::toColorLA() const	{ return axColorLA_<T>(luma(), a); }

template<class T> AX_INLINE
axColorLA_<T>	 axColorRGB_<T>::toColorLA() const	{ return axColorLA_<T>(luma()); }

template<class T> AX_INLINE
axColorLA_<T>	 axColorL_<T>::toColorLA() const	{ return axColorLA_<T>(luma); }
