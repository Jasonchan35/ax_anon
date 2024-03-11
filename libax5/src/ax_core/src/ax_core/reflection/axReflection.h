#pragma once

#include "../string/axStrLiteral.h"
#include "../container/axTuple.h"

enum class axTypeFlags : axUInt64 {
	None = 0,
	Copyable = 1 << 0,
	SingleRowLayout = 1 << 1,
};
AX_ENUM_OPERATORS(axTypeFlags)

template<class T> class axReflection_Impl;
template<class T> using axReflection = axReflection_Impl<T>;
template<class T> class axReflection_Impl : public T::axReflection_ClassInfo {};

class axReflection_ClassInfo {
	axReflection_ClassInfo() = delete;
public:
	using OwnFields	= axTuple<>;
	using Fields	= axTuple<>;

	static constexpr axTypeFlags flags() { return axTypeFlags::None; }
	static AX_INLINE constexpr auto ownFields() { return axTuple<>(); }
	static AX_INLINE constexpr auto fields() { return axTuple<>(); }
};

class axNoBase {
public:
	static const axType* s_rtti() { return nullptr; }
	inline const axType* _onGetTypeOf() const { return nullptr; }
	using axReflection_ClassInfo = ::axReflection_ClassInfo;
};

#define axReflection_PRIMITIVE_TYPE(TEMPLATE, T) \
	TEMPLATE \
	struct axReflection_Impl<T> : public axReflection_ClassInfo { \
		static AX_INLINE constexpr axStrLiteral s_typeName() { return #T; } \
	}; \
//-----

axReflection_PRIMITIVE_TYPE(template<>, axNoBase)
axReflection_PRIMITIVE_TYPE(template<>, void)

axReflection_PRIMITIVE_TYPE(template<>, axInt8)
axReflection_PRIMITIVE_TYPE(template<>, axInt16)
axReflection_PRIMITIVE_TYPE(template<>, axInt32)
axReflection_PRIMITIVE_TYPE(template<>, axInt64)

axReflection_PRIMITIVE_TYPE(template<>, axUInt8)
axReflection_PRIMITIVE_TYPE(template<>, axUInt16)
axReflection_PRIMITIVE_TYPE(template<>, axUInt32)
axReflection_PRIMITIVE_TYPE(template<>, axUInt64)

axReflection_PRIMITIVE_TYPE(template<>, axFloat32)
axReflection_PRIMITIVE_TYPE(template<>, axFloat64)
axReflection_PRIMITIVE_TYPE(template<>, axFloat128)

class axReflection_FieldInfo {
	axReflection_FieldInfo() = delete;
};

// compile-time reflection
template<class OWNER, class TYPE, TYPE OWNER::*PTR_TO_MEMBER>
class axReflection_FieldInfoT : public axReflection_FieldInfo {
public:
	using Type  = TYPE;
	using Owner = OWNER;

	AX_INLINE static constexpr axInt			s_offset()		{ return axPtrUtil::memberOffset(PTR_TO_MEMBER); }
	AX_INLINE static constexpr Type Owner::*	s_ptrToMember() { return PTR_TO_MEMBER; }

	AX_INLINE static       Type& s_value(      Owner* owner) { return owner->*PTR_TO_MEMBER; }
	AX_INLINE static const Type& s_value(const Owner* owner) { return owner->*PTR_TO_MEMBER; }

	AX_INLINE static constexpr axStrLiteral s_typeName();
};

#define AX_REFLECTION_BEGIN(T, T_BASE) \
	class axReflection_ClassInfo : public axReflection<T_BASE> { \
	public: \
		axReflection_ClassInfo() = delete; \
		using DataType = T; \
		using BaseInfo = axReflection<T_BASE>; \
		static AX_INLINE constexpr axStrLiteral s_typeName()	{ return #T; } \
		static AX_INLINE constexpr bool isSingleRowLayout()	{ return (flags() & axTypeFlags::SingleRowLayout) != axTypeFlags::None; } \
		static AX_INLINE constexpr bool isCopyable()			{ return (flags() & axTypeFlags::Copyable		) != axTypeFlags::None; } \
//----

#define ax_FIELD_INFO_STRUCT(M) \
	struct Field_##M : public axReflection_FieldInfoT<DataType, decltype(DataType::M), &DataType::M> { \
		using FieldBase     = axReflection_FieldInfoT<DataType, decltype(DataType::M), &DataType::M>; \
		using FieldBase::s_typeName; \
		using FieldBase::s_offset; \
		static AX_INLINE constexpr axStrLiteral s_name() { return #M; } \
		/* virtual axStrLiteral name() const final { return s_name(); } */ \
		/* void onFormat(axFormat& f) const { f.format("\n  {?}:{?} offset={?}", s_name(), s_typeName(), s_offset()); } */ \
		Field_##M() = delete; \
	}; \
	Field_##M field_##M; \
//----


#define ax_FIELD_INFO(M)		Field_##M
#define ax_FIELD_MAKE_TUPLE(M)	Field_##M()

#define ax_FIELDS(...) \
		AX_VA_ARGS_FOR_EACH(ax_FIELD_INFO_STRUCT, __VA_ARGS__ ) \
		using OwnFields = axTuple< AX_VA_ARGS_FOR_EACH_WITH_COMMA(ax_FIELD_INFO, __VA_ARGS__ ) >; \
		using Fields = typename axTuple_JoinType<BaseInfo::Fields, OwnFields>::Type; \
		/* static AX_INLINE constexpr OwnFields ownFields() { return axTuple_make( AX_VA_ARGS_FOR_EACH_WITH_COMMA(ax_FIELD_MAKE_TUPLE, __VA_ARGS__ ) ); } */ \
		/* static AX_INLINE constexpr Fields fields() { return axTuple_join(BaseInfo::fields(), ownFields()); } */ \
//----

#define AX_REFLECTION_END() \
	}; /* class axReflection_ClassInfo */ \
//------

#define AX_REFLECTION_IO_FUNC() \
	void onFormat(axFormat& f) const; \
	void onJsonReader(axJsonReader& rd); \
	void onJsonWriter(axJsonWriter& wr) const; \
//---

#define AX_REFLECTION_IO_FUNC_IMPL(TEMPLATE, T) \
	TEMPLATE void T::onJsonReader(axJsonReader& rd)			{ rd.reflection(*this); } \
	TEMPLATE void T::onJsonWriter(axJsonWriter& wr) const	{ wr.reflection(*this); } \
	TEMPLATE void T::onFormat(axFormat& f) const			{  f.reflection(*this); } \
//---

template<class OWNER, class TYPE, TYPE OWNER::*PTR_TO_MEMBER> AX_INLINE constexpr
axStrLiteral axReflection_FieldInfoT<OWNER, TYPE, PTR_TO_MEMBER>::s_typeName() {
	return axReflection<Type>::s_typeName();
}
