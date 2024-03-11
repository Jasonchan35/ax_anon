#pragma once

#include "axColorR.h"

template<class T>
class axColorRG_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 2;
	static constexpr axColorType kColorType = axColorType_RG<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::RG;
	static constexpr axInt kAlphaBits = 0;

	T r,g;

	AX_STRUCT_BEGIN(axColorRG_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(r, g)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---
	static constexpr This kBlack	() { return This(kElemMin(),  kElemMin()); }
//---
	static constexpr This kRed		() { return This(kElemMax(),  kElemMin()); }
	static constexpr This kGreen	() { return This(kElemMin(),  kElemMax()); }
//---
	static constexpr This kYellow	() { return This(kElemMax(),  kElemMax()); }
//---

	AX_INLINE explicit axColorRG_() = default;
	AX_INLINE          axColorRG_(T r_, T g_) { set(r_,g_); }

	AX_INLINE		 T* data()			{ return &r; }
	AX_INLINE const T* data() const	{ return &r; }

	AX_INLINE	axMutSpan<T> span()			{ return axMutSpan<T>(&r, kElementCount); }
	AX_INLINE     axSpan<T> span() const	{ return    axSpan<T>(&r, kElementCount); }

	AX_INLINE void set(T r_, T g_) { r=r_; g=g_; }

	AX_INLINE This	operator+	(const This& rhs) const { return This(r+rhs.r, g+rhs.g); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(r-rhs.r, g-rhs.g); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(r,rhs.r), _mul(g,rhs.g)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(r,rhs.r), _div(g,rhs.g)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return r == rhs.r && g == rhs.g; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return r != rhs.r || g != rhs.g; }

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.rg());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorRG_<SRC_E>, SRC_E> {
		static This make(const axColorRG_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.r),
				ElemUtil::to<T>(src.g)
			);
		}
	};

	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorRG_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorRG_<T> & v) {
	auto span = v.span();
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorRG_<T> & v) {
	auto span = v.span();
	se.io(span);
}
