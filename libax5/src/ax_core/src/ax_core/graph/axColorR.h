#pragma once

#include "axColorElement.h"

template<class T>
class axColorR_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 1;
	static constexpr axColorType kColorType = axColorType_R<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::R;
	static constexpr axInt kAlphaBits = 0;

	T r;

	AX_STRUCT_BEGIN(axColorR_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(r)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kHalf()	{ return ElemUtil::kHalf<T>(); }
//---
	static constexpr This kBlack	() { return This(kMin()); }
//---
	static constexpr This kRed		() { return This(kMax()); }
//---
	AX_INLINE explicit axColorR_() = default;
	AX_INLINE          axColorR_(T r_) { set(r_); }

	AX_INLINE		 T* data()			{ return &r; }
	AX_INLINE const T* data() const	{ return &r; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(r); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(r); }

	AX_INLINE void set(T r_) { r=r_; }

	AX_INLINE This	operator+	(const This& rhs) const { return This(r+rhs.r); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(r-rhs.r); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(r,rhs.r)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(r,rhs.r)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return r == rhs.r; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return r != rhs.r; }

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.red());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorRGBA_<SRC_E>, SRC_E> {
		static This make(const axColorRGBA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.r)
			);
		}
	};

private:
	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorR_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axColorR_<T> & v) {
	auto span = axMutSpan_make(v.data);
	se.io(span);
}

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axColorR_<T> & v) {
	auto span = axMutSpan_make(v.data);
	se.io(span);
}
