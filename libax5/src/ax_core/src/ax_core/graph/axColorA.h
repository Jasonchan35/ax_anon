#pragma once

#include "axColorElement.h"

template<class T>
class axColorA_ {
public:
	using Element = T;
	using ElemUtil = axColorElementUtil;

	static constexpr axInt kElementCount = 2;
	static constexpr axColorType kColorType = axColorType_A<T>::value;
	static constexpr axColorModel kColorModel = axColorModel::A;
	static constexpr axInt kAlphaBits = ax_sizeof(T);

	T a;

	AX_STRUCT_BEGIN(axColorA_<T>, axNoBase)
		static constexpr axTypeFlags flags() {
			return axTypeFlags::SingleRowLayout;
		}
		ax_FIELDS(a)
	AX_STRUCT_END()
	AX_REFLECTION_IO_FUNC();

//---
	static constexpr T kElemMin()	{ return ElemUtil::kMin<T>(); }
	static constexpr T kElemMax()	{ return ElemUtil::kMax<T>(); }
	static constexpr T kElemHalf()	{ return ElemUtil::kHalf<T>(); }
//---

	AX_INLINE explicit axColorA_() = default;
	AX_INLINE          axColorA_(T a_) { set(a_); }

	AX_INLINE		 T* data()			{ return &a; }
	AX_INLINE const T* data() const	{ return &a; }

	AX_INLINE    axSpan<T>	span() const	{ return    axSpan<T>(a); }
	AX_INLINE axMutSpan<T>	span()			{ return axMutSpan<T>(a); }

	AX_INLINE void set(T a_) { a=a_; }

	AX_INLINE This	operator+	(const This& rhs) const { return This(a+rhs.a); }
	AX_INLINE This	operator-	(const This& rhs) const { return This(a-rhs.a); }
	AX_INLINE This	operator*	(const This& rhs) const { return This(_mul(a,rhs.a)); }
	AX_INLINE This	operator/	(const This& rhs) const { return This(_div(a,rhs.a)); }

	AX_INLINE bool	operator==	(const This& rhs) const { return a == rhs.a; }
	AX_INLINE bool	operator!=	(const This& rhs) const { return a != rhs.a; }

	template<class SRC>	static This s_make(const SRC& src) {
		return MakeHelper<SRC, typename SRC::Element>::make(src);
	}

private:
	template<class SRC, class SRC_E>
	struct MakeHelper {
		static This make(const SRC& src) {
			return s_make(src.alpha());
		}
	};

	template<class SRC_E>
	struct MakeHelper<axColorRGBA_<SRC_E>, SRC_E> {
		static This make(const axColorRGBA_<SRC_E>& src) {
			return This(
				ElemUtil::to<T>(src.a)
			);
		}
	};

	AX_INLINE static T _mul(T a, T b) { return ElemUtil::mul(a,b); }
	AX_INLINE static T _div(T a, T b) { return ElemUtil::div(a,b); }
};

namespace axTypeTraits {
	template<class T> struct isTriviallyCopyable< axColorA_<T> > { const static bool value = isTriviallyCopyable<T>::value; };
}
