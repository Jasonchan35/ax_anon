#pragma once
#include "axFieldInfo.h"

#include "../reflection/axReflection.h"

enum class axType_InternalType {
	Unknown,
	Void,
	Bool,
	Int8,		Int16,		Int32,		Int64,
	UInt8,		UInt16,		UInt32,		UInt64,
	Char8,		Char16,		Char32,
				Float16,	Float32,	Float64,	Float128,
//--------
	_EndOfBuiltInType,
//-- Containers --
	Array,
	Dict,
	SPtr,
//-- Other --
	Pod,		// TrivialType - no custom constructor / destructor
	Struct,		// Standard Layout Struct - no virtual function
	Class,
};

using axType_InitFunc = void (axType* type);

class axType : public axDListNode<axType> {
public:
	using This = axType;
	using InternalType = axType_InternalType;
	using InitFunc = axType_InitFunc;

	~axType();

		 axStrLiteral	name() const		{ return _name; }
		 axStrLiteral	internalUniqueName() const { return _internalUniqueName; }

	const axType*		baseClass() const	{ return _baseClass; }

					bool	isKindOf(const axType* t) const;

					template<class K> inline
					bool	isKindOf() const;

				void		onFormat(axFormat& f) const;

				void		setValueByPath	(void* obj, axStrView path, axStrView newValue) const;
				bool		getDataPtrByPath(void* obj, axStrView path, void* & outData, const axType* & outType) const;

				axInt		ownFieldCount	() const { return _ownFields.size(); }
				axInt		fieldCount		() const { return _fields.size(); }

	axSpan<axFieldInfo*>	ownFields		() const { return _ownFields; }
	axSpan<axFieldInfo*>	fields			() const { return _fields; }

	const axFieldInfo*		findField		(axStrView name) const;
	const axFieldInfo*		findField		(axInt index) const;

			InternalType	internalType	() const { return _internalType; }
			const axType*	elementType		() const { return _elementType; }

	bool isUnknown() const { return _internalType == InternalType::Unknown; }
	bool isArray() const { return _internalType == InternalType::Array; }
	bool isDict () const { return _internalType == InternalType::Dict;  }
	bool isContainer() const { return _elementType != nullptr; }

	axTypeFlags	flags() const { return _flags; }
	constexpr bool isSingleRowLayout() const { return (_flags & axTypeFlags::SingleRowLayout) != axTypeFlags::None; }

	bool operator==(const axType& r) const;

	template<class R>
	void initByReflection();

	void initArray(const axType* elementType);
	void initDict (const axType* dictKeyType, const axType* elementType);
	void initSPtr(const axType* elementType);

	void readJson (axJsonReader& json, void* data) const { if(_readJson ) _readJson (json, data); }
	void writeJson(axJsonWriter& json, void* data) const { if(_writeJson) _writeJson(json, data); }
	void voidPtrAssign(void* dst, const void* src) const { if(_voidPtrAssign) _voidPtrAssign(dst, src); }

	using VoidPtrAssignFunc = void(*)(void* dst, const void* src);
	using ReadJsonFunc  = void(*)(axJsonReader& json,       void* data);
	using WriteJsonFunc = void(*)(axJsonWriter& json, const void* data);

	void setVoidPtrAssignFunc	(VoidPtrAssignFunc f) { _voidPtrAssign = f; }
	void setReadJsonFunc		(ReadJsonFunc      f) { _readJson  = f; }
	void setWriteJsonFunc		(WriteJsonFunc     f) { _writeJson = f; }

friend class axTypeManager;
friend class axFieldInfo;
friend class axType_Init;
//protected:
	void _addField(axFieldInfo& fieldInfo);
	void _postInitCheck();

	axSourceLoc				_loc;
	const axType*			_baseClass = nullptr;
	InternalType			_internalType = InternalType::Unknown;
	axStrLiteral			_name;
	axStrLiteral			_internalUniqueName;
	axSpan<axFieldInfo*>	_ownFields;
	axSpan<axFieldInfo*>	_fields;
	const axType*			_dictKeyType = nullptr;
	const axType*			_elementType = nullptr;
	VoidPtrAssignFunc		_voidPtrAssign = nullptr;
	ReadJsonFunc			_readJson = nullptr;
	WriteJsonFunc			_writeJson = nullptr;
	axInt					_initOwnFieldCount = 0;
	axTypeFlags				_flags = axTypeFlags::None;
};

class axType_Init : public axNonCopyable {
public:
	axType_Init(const axSourceLoc& loc, axStrLiteral8 fullname, const axType* baseClass, axStrLiteral name, axType::InternalType internalType, axType::InitFunc initFunc);
	axType _type;
};

template<class T> AX_INLINE const axType* ax_typeof(const T* v) { return v->_onGetTypeOf(); }
template<class T> AX_INLINE const axType* ax_typeof(      T* v) { return v->_onGetTypeOf(); }
template<class T> AX_INLINE const axType* ax_typeof(const T& v) { return v._onGetTypeOf(); }
template<class T> AX_INLINE const axType* ax_typeof(      T& v) { return v._onGetTypeOf(); }

template<class T> AX_INLINE
const axType* ax_typeof() { return T::s_rtti(); }

template<> const axType* ax_typeof<void>();
template<> const axType* ax_typeof<bool>();

template<> const axType* ax_typeof<axInt8 >();
template<> const axType* ax_typeof<axInt16>();
template<> const axType* ax_typeof<axInt32>();
template<> const axType* ax_typeof<axInt64>();

template<> const axType* ax_typeof<axUInt8 >();
template<> const axType* ax_typeof<axUInt16>();
template<> const axType* ax_typeof<axUInt32>();
template<> const axType* ax_typeof<axUInt64>();

template<> const axType* ax_typeof<axFloat32>();
template<> const axType* ax_typeof<axFloat64>();
template<> const axType* ax_typeof<axFloat128>();

template<> AX_INLINE const axType* ax_typeof(const bool&) { return ax_typeof<bool>(); }

template<> AX_INLINE const axType* ax_typeof(const axInt8 &) { return ax_typeof<axInt8 >(); }
template<> AX_INLINE const axType* ax_typeof(const axInt16&) { return ax_typeof<axInt16>(); }
template<> AX_INLINE const axType* ax_typeof(const axInt32&) { return ax_typeof<axInt32>(); }
template<> AX_INLINE const axType* ax_typeof(const axInt64&) { return ax_typeof<axInt64>(); }

template<> AX_INLINE const axType* ax_typeof(const axUInt8 &) { return ax_typeof<axUInt8 >(); }
template<> AX_INLINE const axType* ax_typeof(const axUInt16&) { return ax_typeof<axUInt8 >(); }
template<> AX_INLINE const axType* ax_typeof(const axUInt32&) { return ax_typeof<axUInt8 >(); }
template<> AX_INLINE const axType* ax_typeof(const axUInt64&) { return ax_typeof<axUInt8 >(); }

template<> AX_INLINE const axType* ax_typeof(const axFloat32 &) { return ax_typeof<axFloat32 >(); }
template<> AX_INLINE const axType* ax_typeof(const axFloat64 &) { return ax_typeof<axFloat64 >(); }
template<> AX_INLINE const axType* ax_typeof(const axFloat128&) { return ax_typeof<axFloat128>(); }



AX_INLINE 
bool axTypedBase::isKindOf(const axType* type) const {
	return ax_typeof(this)->isKindOf(type);
}

template<class T> AX_INLINE 
bool axTypedBase::isKindOf() const {
	return ax_typeof(this)->isKindOf<T>();
}

template<class T, class BASE> inline
const axType* axType_struct_rtti(axStrLiteral name) {
	AX_STATIC_ASSERT_MSG(
		axTypeTraits::isSame<BASE, axNoBase>::value || axTypeTraits::isBaseOf<BASE, T>::value,
		"AX_RTTI_CLASS Invalid Base Class"
	);
	AX_STATIC_ASSERT_MSG(axTypeTraits::isStandardLayout<T>::value, "struct must be standard layout (has virtual function ?)");
	static axType_Init typeInit(AX_LOC, AX_FUNC_FULLNAME, BASE::s_rtti(), name, axType::InternalType::Struct, &T::s_initRtti);
	return &typeInit._type;
}

template<class T, class BASE> inline
const axType* axType_class_rtti(axStrLiteral name) {
	AX_STATIC_ASSERT_MSG(
		axTypeTraits::isSame<BASE, axNoBase>::value || axTypeTraits::isBaseOf<BASE, T>::value,
		"AX_RTTI_CLASS Invalid Base Class"
	);
	static axType_Init typeInit(AX_LOC, AX_FUNC_FULLNAME, BASE::s_rtti(), name, axType::InternalType::Class, &T::s_initRtti);
	return &typeInit._type;
}

template<class T, class R> AX_INLINE
const T* ax_type_cast_impl(const R* s) {
	if (!s) return nullptr;
	if (!s->template isKindOf<T>()) return nullptr;
	return static_cast<const T*>(s);
}

template<class T, class R> AX_INLINE
const T* ax_type_cast(const R* s) {
	return ax_type_cast_impl<T, R>(s);
}

template<class T, class R> AX_INLINE
T* ax_type_cast(R* s) {
	return ax_const_cast( ax_type_cast_impl<T, R>(s) );
}

#if _DEBUG
	template<class T, class R> AX_INLINE
	T* ax_type_cast_debug(R* s) {
		if (!s) return nullptr;
		if (T* o = ax_type_cast<T, R>(s)) { return o; }
		throw axError_Undefined(AX_LOC);
	}
#else
	template<class T, class R> AX_INLINE
	T* ax_type_cast_debug(R* s) {
		return static_cast<T*>(s);
	}
#endif

template<class K> inline
bool axType::isKindOf() const { return isKindOf(ax_typeof<K>()); }

AX_INLINE
void axFieldInfo::_addToOwner(axType* owner) {
	owner->_addField(*this);
}

//------------------------------------------
template<class T> inline
void axType_VoidPtrAssignWrapper(void* dst, const void* src) {
	auto* d = reinterpret_cast<T*>(dst);
	auto* s = reinterpret_cast<const T*>(src);
	*d = *s;
}

struct axReflection_InitRtti_Handler {
	axType* type = nullptr;
	axMutSpan<axFieldInfo*> fields;

	template<axInt index, class Field>
	void handle() {
		static auto field = axFieldInfo_make(type, Field::s_ptrToMember(), Field::s_name(), "");
		fields[index] = &field;
	}
};

template<class T> inline
void axType::initByReflection() {
	using R = axReflection<T>;
	const axInt kBaseClassFieldCount = R::BaseInfo::Fields::kSize;
	const axInt kFieldCount = R::Fields::kSize;
	static axFixedArray<axFieldInfo*, kFieldCount> fields;

	auto baseFields = fields.slice(0, kBaseClassFieldCount);
	auto ownFields  = fields.sliceFrom(kBaseClassFieldCount);

	if (_baseClass) {
		baseFields.copyValues(_baseClass->_fields);
	}

	axReflection_InitRtti_Handler h {this, ownFields};
	R::OwnFields::s_forEachType(h);

	_fields		= fields.span();
	_ownFields	= ownFields;
	_flags		= R::flags();

	AX_IF_CONSTEXPR (R::isCopyable()) {
		setVoidPtrAssignFunc(&axType_VoidPtrAssignWrapper<T>);
	}

	setReadJsonFunc (&axJsonReader_VoidPtrWrapper<T>);
	setWriteJsonFunc(&axJsonWriter_VoidPtrWrapper<T>);
}
