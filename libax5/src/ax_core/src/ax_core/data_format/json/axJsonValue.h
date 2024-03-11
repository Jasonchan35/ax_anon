#pragma once

#include "axJsonUtil.h"

#define axJsonValueType_EnumList(E) \
	E(Undefined,) \
	E(Null,) \
	E(Bool,) \
	E(Double,) \
	E(Int64,) \
	E(UInt64,) \
	E(String,) \
	E(Object,) \
	E(Array,) \
//----

ax_ENUM_CLASS(axJsonValueType, axInt8)

class axJsonObject;
class axJsonArray;

class axJsonValue : public axNonCopyable {
public:
	using Type = axJsonValueType;

	axJsonValue() = default;
	explicit axJsonValue(axJsonValue && r) noexcept { operator=(ax_move(r)); }

	template<class V> explicit axJsonValue(const V& v) { setValue(v); }
	template<class V> void operator=(const V& v) noexcept { setValue(v); }

	~axJsonValue() { _reset(); }

	void initFromJson(axStrView8 json, axStrView filenameForErrorMessage);
	void initFromFile(axStrView filename);

//---------
	bool isNull		() const { return _type == Type::Null; }
	bool isUndefined() const { return _type == Type::Undefined; }
	bool isNullOrUndefined() const { return isNull() || isUndefined(); }

	bool isNumber	() const { return isDouble() || isInt64() || isUInt64(); }
	bool isDouble	() const { return _type == Type::Double; }
	bool isInt64	() const { return _type == Type::Int64; }
	bool isUInt64	() const { return _type == Type::UInt64; }

	bool isString	() const { return _type == Type::String; }
	bool isObject	() const { return _type == Type::Object; }
	bool isArray	() const { return _type == Type::Array;  }

	explicit operator bool() const { return !isNullOrUndefined(); }

//---------
	void setUndefined();
	void setNull();

	void setValue(axNull) { setNull(); }

	void setValue(bool      b);

	void setValue(float     v) { setValue(static_cast<double>(v)); }
	void setValue(double    v);

	void setValue(axInt8    v) { setValue(static_cast<axInt64>(v)); }
	void setValue(axInt16   v) { setValue(static_cast<axInt64>(v)); }
	void setValue(axInt32   v) { setValue(static_cast<axInt64>(v)); }
	void setValue(axInt64   v);

	void setValue(axUInt8   v) { setValue(static_cast<axUInt64>(v)); }
	void setValue(axUInt16  v) { setValue(static_cast<axUInt64>(v)); }
	void setValue(axUInt32  v) { setValue(static_cast<axUInt64>(v)); }
	void setValue(axUInt64  v);
	
	void setValue(axStrView8  s);
	void setValue(axStrView16 s);
	void setValue(axStrView32 s);
	void setValue(axStrViewW  s);

	template<axInt N> void setValue(const axChar8  (&sz)[N]) { setValue(axStrView8 (sz)); }
	template<axInt N> void setValue(const axChar16 (&sz)[N]) { setValue(axStrView16(sz)); }
	template<axInt N> void setValue(const axChar32 (&sz)[N]) { setValue(axStrView32(sz)); }
	template<axInt N> void setValue(const axCharW  (&sz)[N]) { setValue(axStrViewW (sz)); }

	axJsonObject*	setObjectType();
	axJsonArray*	setArrayType(axInt size);
	axIString8*		setStringType();

	void operator=(axJsonValue && r) noexcept;
	
//---------
	axJsonValue*	findMember(axStrView name);

	template<class VALUE>
	bool tryGetMemberValue(axStrView memberName, VALUE& value);
	axJsonObject*	tryGetMemberObject(axStrView memberName);
	axJsonArray*	tryGetMemberArray (axStrView memberName);
	axIString8*		tryGetMemberStringPtr(axStrView memberName);

	bool tryGetValue(axIString8    & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(axIString16   & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(axIString32   & outValue) const { return _tryGetStringValue(outValue); }
	bool tryGetValue(axIStringW    & outValue) const { return _tryGetStringValue(outValue); }

	bool tryGetValue(bool          & outValue) const noexcept;

	bool tryGetValue(float		   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(double		   & outValue) const noexcept { return _tryGetNumericValue(outValue); }

	bool tryGetValue(axInt8		   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axInt16	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axInt32	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axInt64	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }

	bool tryGetValue(axUInt8	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axUInt16	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axUInt32	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }
	bool tryGetValue(axUInt64	   & outValue) const noexcept { return _tryGetNumericValue(outValue); }

	template<class T> bool tryGetValue(axVec2<T>& outValue) const noexcept;
	template<class T> bool tryGetValue(axVec3<T>& outValue) const noexcept;
	template<class T> bool tryGetValue(axVec4<T>& outValue) const noexcept;

			axJsonObject*	tryGetObject()			{ return isObject() ? _value.v_object	: nullptr; }
	const	axJsonObject*	tryGetObject() const	{ return isObject() ? _value.v_object	: nullptr; }

			axJsonArray*	tryGetArray	()			{ return isArray()  ? _value.v_array	: nullptr; }
	const	axJsonArray*	tryGetArray	() const	{ return isArray()  ? _value.v_array	: nullptr; }

			axIString8*		tryGetStringPtr()		{ return isString() ? _value.v_string	: nullptr; }
	const	axIString8*		tryGetStringPtr() const	{ return isString() ? _value.v_string	: nullptr; }

	template<class V>
	void getValue(V& outValue) {
		if (!tryGetValue(outValue)) { throw axError_Undefined(AX_LOC); }
	}

	template<class V>
	void getValue(V & outValue) const { ax_const_cast(this)->getValue(outValue); }

//---------
	void onFormat(axFormat& f) const;
	void appendToJson(axIString& outJson) const;

	void onJsonWriter(axJsonWriter& wr) const;
	void onJsonReader(axJsonReader& rd);

	struct SourceLoc {
		axInt lineNumber = 0;
		axInt column = 0;
	};
	SourceLoc	loc;

private:
	void _setLoc(axJsonReader& rd);

	bool _tryGetValuePtr(axJsonObject* & outValue) noexcept;
	bool _tryGetValuePtr(axJsonArray*  & outValue) noexcept;
	bool _tryGetValuePtr(axIString8*   & outValue) noexcept;

	template<class CH> void _setValue(axStrViewT<CH> s);

	template<class VALUE>
	bool _tryGetNumericValue(VALUE & outValue) const noexcept {
		switch (_type) {
			case Type::Double: ax_static_cast(outValue, _value.v_double); return true;
			case Type::Int64:  ax_static_cast(outValue, _value.v_int64);  return true;
			case Type::UInt64: ax_static_cast(outValue, _value.v_uint64); return true;
		}
		return false;
	}

	template<class STR>
	bool _tryGetStringValue(STR & str) const {
		str.clear();
		auto* p = tryGetStringPtr();
		if (!p) return false;
		str.setUtf(*p);
		return true;
	}
	
	void _reset();

	Type	_type = Type::Undefined;
	union Value {
		bool			v_bool;
		double			v_double;
		axInt64			v_int64;
		axUInt64		v_uint64;
		axString8*		v_string;
		axJsonObject*	v_object;
		axJsonArray*	v_array;
	};
	Value _value;
};

class axJsonObjectMemeber : public axMapNode<axString, axJsonObjectMemeber> {
	using This = axJsonObjectMemeber;
public:
	using MapNode = axMapNode<axString, axJsonObjectMemeber>;

	axStrView	name() const { return MapNode::mapNode_key; }
	axJsonValue value;
};

class axJsonObject : public axNonCopyable {
public:
	using Member = axJsonObjectMemeber;
	using Value  = axJsonValue;

	axJsonObject*	tryGetMemberObject(axStrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetObject();
	}

	axJsonArray*	tryGetMemberArray (axStrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetArray();
	}

	axIString8*		tryGetMemberStringPtr(axStrView memberName) {
		auto* m = findMember(memberName);
		if (!m) return nullptr;
		return m->tryGetStringPtr();
	}

	Value* findMember(axStrView name) {
		auto* m = _members.find(name);
		if (!m) return nullptr;
		return &m->value;
	}

	Value& getOrAddMember(axStrView name) {
		auto* m = _members.getOrAdd(name);
		return m->value;
	}

	Value& operator[](axStrView name) { return getOrAddMember(name); }


	void onFormat(axFormat& f) const;
	void onJsonWriter(axJsonWriter& wr) const;
	void onJsonReader(axJsonReader& rd);

friend class axJsonValue;
protected:
	axJsonObject() = default;
private:
	axMap<axString8, Member> _members;
};

class axJsonArray : public axArray<axJsonValue, 4> {
public:
	void onFormat(axFormat& f) const;
	void onJsonWriter(axJsonWriter& wr) const;
	void onJsonReader(axJsonReader& rd);

friend class axJsonValue;
protected:
	axJsonArray() = default;
};


//--------- inline -----------------------

template<class VALUE> inline
bool axJsonValue::tryGetMemberValue(axStrView memberName, VALUE& value) {
	auto* m = findMember(memberName);
	if (!m) return false;
	return m->tryGetValue(value);
}

inline
axJsonObject*	axJsonValue::tryGetMemberObject(axStrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetObject();
}

inline
axJsonArray*	axJsonValue::tryGetMemberArray (axStrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetArray();
}

inline
axIString8*		axJsonValue::tryGetMemberStringPtr(axStrView memberName) {
	auto* m = findMember(memberName);
	if (!m) return nullptr;
	return m->tryGetStringPtr();
}

template<class T> inline
bool axJsonValue::tryGetValue(axVec2<T>	& outValue) const noexcept {
	auto* arr = tryGetArray();
	if (!arr) return false;
	if (arr->size() != 2) return false;
	if (!arr->at(0).tryGetValue(outValue.x)) return false;
	if (!arr->at(1).tryGetValue(outValue.y)) return false;
	return true;
}

template<class T> inline
bool axJsonValue::tryGetValue(axVec3<T> & outValue) const noexcept {
	auto* arr = tryGetArray();
	if (!arr) return false;
	if (arr->size() != 3) return false;
	if (!arr->at(0).tryGetValue(outValue.x)) return false;
	if (!arr->at(1).tryGetValue(outValue.y)) return false;
	if (!arr->at(2).tryGetValue(outValue.z)) return false;
	return true;
}

template<class T> inline
bool axJsonValue::tryGetValue(axVec4<T> & outValue) const noexcept {
	auto* arr = tryGetArray();
	if (!arr) return false;
	if (arr->size() != 4) return false;
	if (!arr->at(0).tryGetValue(outValue.x)) return false;
	if (!arr->at(1).tryGetValue(outValue.y)) return false;
	if (!arr->at(2).tryGetValue(outValue.z)) return false;
	if (!arr->at(3).tryGetValue(outValue.w)) return false;
	return true;
}
