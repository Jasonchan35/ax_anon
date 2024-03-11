#pragma once

#include "../string/axString.h"
#include "../string/axStrLiteral.h"
#include "../pointer/axSPtr.h"
#include "../container/axSList.h"
#include "../container/axMap.h"
#include "axType_macro.h"

class axType;
class axType_FieldEnumerator;
class axTypeManager;
class axAttribute;

template<class T> const axType* ax_typeof();

#define axType_ADD_FIELD(V) \
	static axFieldInfo fi_##V(&This::V, #V); \
	typeInfo->addField(fi_##V); \
//--------

#define axType_ADD_FIELD_WITH_NAME(V, NAME, DISPLAY_NAME) \
	static axFieldInfo fi_##V(&This::V, NAME, DISPLAY_NAME); \
	typeInfo->addField(fi_##V); \
//--------

template<class T> const axType* ax_typeof(const T& v);

class axValuePtr {
public:
	axValuePtr() = default;
	explicit axValuePtr(const axValuePtr& r) = default;
	explicit axValuePtr(const axType* type, void* voidPtr) : _type(type), _voidPtr(voidPtr) {}

	template<class T>
	explicit axValuePtr(T& v) : _type(ax_typeof(v)), _voidPtr(&v) {};

	template<class T>
	T& value() {
		if (_type != ax_typeof<T>()) {
			throw axError_Undefined(AX_LOC);
		}
		return *reinterpret_cast<T*>(_voidPtr);
	}

	const axType* _type = nullptr;
	void* _voidPtr = nullptr;
};

class axTypedBase {
public:
	virtual ~axTypedBase() {}

	using axReflection_ClassInfo = ::axReflection_ClassInfo;

	AX_INLINE	virtual const axType* _onGetTypeOf() const { return s_rtti(); }
					static  const axType* s_rtti();

	template<class T>
	AX_INLINE bool isKindOf() const;
	AX_INLINE bool isKindOf(const axType* type) const;
};

class axFieldInfo : public axTypedBase, public axNonCopyable {
	using This = axFieldInfo;
public:
	axFieldInfo(axType* owner, axInt offset, const axType* type, axStrLiteral name, axStrLiteral displayName = axStrLiteral("")) 
		: _owner(owner)
		, _type(type)
		, _offset(offset)
		, _name(name)
		, _displayName(displayName)
		, _isDynamic(false)
	{
		_addToOwner(owner);
	}

	template<class OBJ, class MEMBER>
	axFieldInfo(axType* owner, 
				MEMBER OBJ::*ptrToMember,
				axStrLiteral name,
				axStrLiteral displayName = axStrLiteral(""))
	: axFieldInfo(owner, axPtrUtil::memberOffset(ptrToMember), ax_typeof<MEMBER>(), name, displayName)
	{}

	axStrLiteral	name		() const { return _name; }
	const axType*	owner		() const { return _owner; }
	const axType*	type		() const { return _type; }
	axInt			index		() const { return _index; }
	axInt			offset		() const { return _offset; }
	bool			isDynamic	() const { return _isDynamic; }

	void onFormat(axFormat& f) const;

	void setToDefaultValue(void* obj) const {
		axValuePtr p(type(), reinterpret_cast<char*>(obj) + _offset);
		onSetToDefaultValue(p); 
	}

	template<class ATTR> const ATTR*	findAttr() const; // implement in axAttribute.h

friend class axType;
protected:
	virtual void onSetToDefaultValue(axValuePtr& value) const = 0;

	void _addToOwner(axType* owner); // implement in axType.h

	const axType*			_owner = nullptr;
	const axType*			_type = nullptr;
	axInt					_index;
	axInt					_offset = 0;
	axStrLiteral			_name;
	axStrLiteral			_displayName;
	axSpan<axAttribute*>	_attributes;
	bool					_isDynamic : 1;
};

template<class OBJ, class MEMBER>
class axFieldInfoT : public axFieldInfo {
	using Base = axFieldInfo;
public:
	 axFieldInfoT(axType* owner, MEMBER OBJ::*ptrToMember, axStrLiteral name, axStrLiteral displayName)
		: Base(owner, ptrToMember, name, displayName)
	 {}

protected:
	virtual void onSetToDefaultValue(axValuePtr& data) const {
		axTypeTraits::defaultValue<MEMBER>::get(data.value<MEMBER>());
	}
};

template<class OBJ, class MEMBER>
axFieldInfoT<OBJ, MEMBER> axFieldInfo_make(axType* owner, MEMBER OBJ::*ptrToMember, axStrLiteral name, axStrLiteral displayName = "") {
	return axFieldInfoT<OBJ, MEMBER>(owner, ptrToMember, name, displayName);
}
