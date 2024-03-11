#pragma once

#define ax_DOWNCAST_GET_INSTANCE() \
	AX_INLINE static This* s_instance() { return static_cast<This*>(Base::s_instance()); }
//---------

//========== Declare ==============
#define AX_RTTI_INTERNAL_STRUCT(T) \
private:\
	using This = T; \
public: \
	inline const axType* _onGetTypeOf() const { return s_rtti(); } \
 	static const axType* s_rtti(); \
	static void s_initRtti(axType* type) {} \
//--------

#define AX_RTTI_INTERNAL_CLASS(T) \
private:\
	using This = T; \
public: \
	inline virtual const axType* _onGetTypeOf() const { return s_rtti(); } \
	static const axType* s_rtti(); \
	static void s_initRtti(axType* type) {} \
//--------

#define AX_RTTI_INTERNAL_STRUCT_IMPL(TEMPLATE, T) \
 	TEMPLATE const axType* T::s_rtti() { return axType_struct_rtti<T, axNoBase>(#T); } \
// ----

#define AX_RTTI_INTERNAL_CLASS_IMPL(TEMPLATE, T) \
 	TEMPLATE const axType* T::s_rtti() { return axType_class_rtti<T, axNoBase>(#T); } \
// ----

#define AX_RTTI_STRUCT_COMMON(T, BASE) \
private:\
	using This = T; \
	using Base = BASE; \
public: \
	inline const axType* _onGetTypeOf() const { return s_rtti(); } \
 	static const axType* s_rtti() { return axType_struct_rtti<T, BASE>(#T); } \
//--------

#define AX_RTTI_CLASS_COMMON(T, BASE) \
private:\
	using This = T; \
	using Base = BASE; \
public: \
	inline virtual const axType* _onGetTypeOf() const override { return s_rtti(); } \
	static const axType* s_rtti() { return axType_class_rtti<T, BASE>(#T); } \
//--------

#define AX_RTTI_STRUCT(T, BASE) \
	AX_RTTI_STRUCT_COMMON(T, BASE) \
	static void s_initRtti(axType* type) {} \
//---------

#define AX_RTTI_CLASS(T, BASE) \
	AX_RTTI_CLASS_COMMON(T, BASE) \
	static void s_initRtti(axType* type) {} \
//---------

#define AX_RTTI_STRUCT_CUSTOM_INIT(T, BASE) \
	AX_RTTI_STRUCT_COMMON(T, BASE) \
	static void s_initRtti(axType* type); /* custom implement */ \
//---------

#define AX_RTTI_CLASS_CUSTOM_INIT(T, BASE) \
	AX_RTTI_CLASS_COMMON(T, BASE) \
	static void s_initRtti(axType* type); /* custom implement */  \
//---------

#define ax_CLASS_BEGIN(T, BASE) \
	AX_RTTI_CLASS_COMMON(T, BASE) \
	AX_REFLECTION_BEGIN(T, BASE) \
//------

#define AX_STRUCT_BEGIN(T, BASE) \
	AX_RTTI_STRUCT_COMMON(T, BASE) \
	AX_REFLECTION_BEGIN(T, BASE) \
//------

#define AX_STRUCT_END_CUSTOM_INIT() \
	AX_REFLECTION_END() \
	static void s_initRtti(axType* type); /* custom implement */  \
//----

#define ax_CLASS_END_CUSTOM_INIT() \
	AX_REFLECTION_END() \
	static void s_initRtti(axType* type); /* custom implement */  \
//---

#define AX_STRUCT_END() \
	AX_REFLECTION_END() \
	static void s_initRtti(axType* type) { type->initByReflection<This>(); } \
//----

#define ax_CLASS_END() \
	AX_REFLECTION_END() \
	static void s_initRtti(axType* type) { type->initByReflection<This>(); } \
//---
