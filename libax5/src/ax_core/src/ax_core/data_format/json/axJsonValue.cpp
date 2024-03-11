#include "axJsonValue.h"
#include "axJsonWriter.h"

void axJsonValue::setNull() {
	_reset();
	_type = Type::Null;
}

void axJsonValue::initFromJson(axStrView8 json, axStrView filenameForErrorMessage) {
	_reset();

	axJsonReader reader(json, filenameForErrorMessage);
	onJsonReader(reader);
}

void axJsonValue::initFromFile(axStrView filename) {
	_reset();

	axString8 json;
	axFile::readUtf8(filename, json);
	initFromJson(json, filename);
}

void axJsonValue::setUndefined() {
	_reset();
}

void axJsonValue::setValue(bool b) {
	_reset();
	_type = Type::Bool;
	_value.v_bool = b;
}

void axJsonValue::setValue(double v) {
	_reset();
	_type = Type::Double;
	_value.v_double = v;
}

void axJsonValue::setValue(axInt64 v) {
	_reset();
	_type = Type::Int64;
	_value.v_int64 = v;
}

void axJsonValue::setValue(axUInt64 v) {
	_reset();
	_type = Type::UInt64;
	_value.v_uint64 = v;
}

template<class CH> AX_INLINE
void axJsonValue::_setValue(axStrViewT<CH> s) {
	setStringType()->setUtf(s);
}

void axJsonValue::setValue(axStrView8  s) { _setValue(s); }
void axJsonValue::setValue(axStrView16 s) { _setValue(s); }
void axJsonValue::setValue(axStrView32 s) { _setValue(s); }
void axJsonValue::setValue(axStrViewW  s) { _setValue(s); }

axJsonObject* axJsonValue::setObjectType() {
	if (!isObject()) {
		_reset();
		_type = Type::Object;
		_value.v_object = ax_new axJsonObject;
	}
	return _value.v_object;
}

axJsonArray* axJsonValue::setArrayType(axInt size) {
	if (!isArray()) {
		_reset();
		_type = Type::Array;
		_value.v_array = ax_new axJsonArray;
	}
	auto& arr = *_value.v_array;
	arr.resize(size);
	return &arr;
}

axIString8* axJsonValue::setStringType() {
	if (!isString()) {
		_reset();
		_type = Type::String;
		_value.v_string = ax_new axString8;
	}
	return _value.v_string;
}

void axJsonValue::operator=(axJsonValue && r) noexcept {
	_reset();
	_type  = r._type;
	_value = r._value;
	r._type  = Type::Undefined;
}

axJsonValue* axJsonValue::findMember(axStrView name) {
	auto* obj = tryGetObject();
	if (!obj) return nullptr;
	return obj->findMember(name);
}

void axJsonValue::_reset() {
	switch (_type) {
	case Type::String: ax_delete_set_null(_value.v_string); break;
	case Type::Object: ax_delete_set_null(_value.v_object); break;
	case Type::Array:  ax_delete_set_null(_value.v_array ); break;
	}
	_type = Type::Undefined;
}

void axJsonValue::appendToJson(axIString& outJson) const {
	axJsonWriter wr(outJson);
	onJsonWriter(wr);
}

void axJsonValue::onJsonWriter(axJsonWriter& wr) const {
	switch (_type) {
		case Type::Undefined:	AX_ASSERT(false);				break;
		case Type::Null:		wr.writeNull();					break;
		case Type::Bool:		wr.writeValue(_value.v_bool);	break;
		case Type::Double:		wr.writeValue(_value.v_double);	break;
		case Type::Int64:		wr.writeValue(_value.v_int64);	break;
		case Type::UInt64:		wr.writeValue(_value.v_uint64);	break;
		case Type::String:		wr.writeValue(*_value.v_string);break;
		case Type::Object:		wr.writeValue(*_value.v_object);break;
		case Type::Array:		wr.writeValue(*_value.v_array);	break;
		default: AX_ASSERT(false);
	}
}

void axJsonValue::onJsonReader(axJsonReader& rd) {
	using InType = axJsonReader::ValueType;
	_setLoc(rd);
	auto type = rd.valueType();
	switch (type) {
		case InType::Number: {
			double v = 0;
			rd.readValue(v);
			setValue(v);
		}break;
		case InType::Bool: {
			bool v = false;
			rd.readValue(v);
			setValue(v);
		}break;
		case InType::Null: {
			setNull();
		}break;
		case InType::String:{
			auto* s = setStringType();
			rd.readValue(*s);
		}break;
		case InType::BeginObject: {
			auto* obj = setObjectType();
			rd.readValue(*obj);
		}break;
		case InType::BeginArray: {
			auto* arr = setArrayType(0);
			rd.readValue(*arr);
		}break;

		//case InType::EndObject:
		//case InType::EndArray:
		//case InType::Invalid:
		//case InType::Member:
		default: {
			throw axError_Undefined(AX_LOC);
		}break;
	}
}

void axJsonValue::_setLoc(axJsonReader& rd) {
	loc.lineNumber = rd.lineNumber();
	loc.column     = rd.column();
}

bool axJsonValue::tryGetValue(bool & outValue) const noexcept {
	if (_type != Type::Bool) return false;
	outValue = _value.v_bool;
	return true;
}

void axJsonValue::onFormat(axFormat& f) const {
	axTempString tmp;
	axJsonWriter wr(tmp);
	onJsonWriter(wr);
	f << tmp;
}

void axJsonObject::onFormat(axFormat& f) const {
	axTempString tmp;
	axJsonWriter wr(tmp);
	onJsonWriter(wr);
	f << tmp;
}

void axJsonArray::onFormat(axFormat& f) const {
	axTempString tmp;
	axJsonWriter wr(tmp);
	onJsonWriter(wr);
	f << tmp;
}

void axJsonObject::onJsonWriter(axJsonWriter& wr) const {
	for (auto& m : _members) {
		wr.writeMemberName(m.name());
		wr.writeValue(m.value);
	}
}

void axJsonObject::onJsonReader(axJsonReader& rd) {
	using InType = axJsonReader::ValueType;
	if (rd.valueType() != InType::BeginObject)
		throw axError_Undefined(AX_LOC);

	if (!rd.next())
		throw axError_Undefined(AX_LOC);

	for(;;) {
		auto type = rd.valueType();
		switch (type) {
			case InType::EndObject: {
				rd.next();
				return;
			} break;

			case InType::Member: {
				axTempString name;
				rd.readMemberName(name);
				auto& m = getOrAddMember(name);
				m.onJsonReader(rd);
			} break;

			default: {
				rd.error("unexpected type inside object");
				return;
			}break;
		}
	}
}

void axJsonArray::onJsonWriter(axJsonWriter& wr) const {
	auto scoped_arr = wr.array();
	for (auto& e : *this) {
		wr.writeValue(e);
	}
}

void axJsonArray::onJsonReader(axJsonReader& rd) {
	using InType = axJsonReader::ValueType;
	if (rd.valueType() != InType::BeginArray)
		throw axError_Undefined(AX_LOC);

	if (!rd.next())
		throw axError_Undefined(AX_LOC);

	for (;;) {
		auto type = rd.valueType();
		switch (type) {
			case InType::EndArray: {
				rd.next();
				return;
			}break;

			default: {
				auto& e = emplaceBack();
				e.onJsonReader(rd);
			}break;
		}
	}
}
