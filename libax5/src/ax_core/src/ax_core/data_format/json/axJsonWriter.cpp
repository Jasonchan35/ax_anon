#include "axJsonWriter.h"

axJsonWriter::axJsonWriter(axIString & outJson) {
	_json = &outJson;
}

axJsonWriter::~axJsonWriter() {
	if (_level.size() != 0) {
		assert(false); //"axJsonWriter end within scope"
	}
}

void axJsonWriter::beginObject(axStrView name) {
	writeMemberName(name);
	beginObject();
}

void axJsonWriter::beginObject() {
	preWriteValue();
	_json->append('{');
	_commaNeeded = false;
	_level.append(LevelType::Object);
}

void axJsonWriter::endObject() {
	newline(-1);
	_json->append('}');
	_commaNeeded = true;

	if (_level.size() == 0) {
		throw axJsonWriterError_EndObject();
	}
	if (_level.back() != LevelType::Object) {
		throw axJsonWriterError_EndObject();
	}
	_level.popBack();

}

void axJsonWriter::beginArray(axStrView name) {
	writeMemberName(name);
	beginArray();
}

void axJsonWriter::beginArray() {
	preWriteValue();
	_json->append('[');
	_commaNeeded = false;
	_level.append(LevelType::Array);
}

void axJsonWriter::endArray() {
	_json->append(']');
	_commaNeeded = true;

	if (_level.size() == 0) {
		throw axJsonWriterError_EndArray();
	}
	if (_level.back() != LevelType::Array) {
		throw axJsonWriterError_EndArray();
	}
	_level.popBack();
}

void axJsonWriter::writeMemberName(axStrView name) {
	if (_level.size() == 0 || _level.back() != LevelType::Object) {
		throw axJsonWriterError_MemberOutsideOfObject(); // missing root object {} ?
	}

	if (_commaNeeded)
		_json->append(", ");
	_commaNeeded = true;
//-------
	newline();
	quoteString(name);
	_json->append(": ");
	_commaNeeded = false;
	_level.append(LevelType::Member);
}

void axJsonWriter::writeNull() {
	preWriteValue();
	_json->append("null");
}

void axJsonWriter::writeValue(axStrView value) {
	preWriteValue();
	quoteString(value);
}

void axJsonWriter::writeValue(bool value) {
	preWriteValue();
	_json->append(axStrLiteral_bool(value));
}

void axJsonWriter::writeValue(axInt8  value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axInt16 value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axInt32 value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axInt64 value) { preWriteValue(); _json->appendFormat("{?}", value); }

void axJsonWriter::writeValue(axUInt8  value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axUInt16 value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axUInt32 value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axUInt64 value) { preWriteValue(); _json->appendFormat("{?}", value); }

void axJsonWriter::writeValue(axFloat32  value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axFloat64  value) { preWriteValue(); _json->appendFormat("{?}", value); }
void axJsonWriter::writeValue(axFloat128 value) { preWriteValue(); _json->appendFormat("{?}", value); }

void axJsonWriter::preWriteValue()
{
	if (_commaNeeded)
		_json->append(", ");
	_commaNeeded = true;

	if (_level.size() > 0) {
		auto lv = _level.back();
		switch (lv) {
		case LevelType::Member:
			_level.popBack();
			break;
		case LevelType::Array:
			break;
		case LevelType::Object:
			throw axJsonWriterError_MemberNameRequired();
			break;
		}
	}
}

void axJsonWriter::quoteString(axStrView v) {
	if (!v.size()) {
		_json->append("\"\"");
		return;
	}

	_json->append('\"');

	const char* p = v.data();
	const char* end = p + v.size();
	for (; *p && p < end; p++) {
		char ch = *p;

		if (static_cast<axByte>(ch) <= 0x1F) {
			const int kTempBufSize = 100;
			char tmp[kTempBufSize+1];
			snprintf(tmp, kTempBufSize, "\\u%04x", ch);
			tmp[kTempBufSize] = 0;

			_json->append(axStrView_c_str(tmp));
			continue;
		}

		switch (ch) {
			case '/':	AX_FALLTHROUGH
			case '\\':	AX_FALLTHROUGH
			case '\"':	_json->append('\\').append(ch);	break;
			case '\b':	_json->append("\\b");		break;
			case '\f':	_json->append("\\f");		break;
			case '\n':	_json->append("\\n");		break;
			case '\r':	_json->append("\\r");		break;
			case '\t':	_json->append("\\t");		break;
			default:	_json->append(ch);		break;
		}
	}
	_json->append('\"');
}

void axJsonWriter::newline(axInt offset) {
	if (_newlineNeeded) {
		_json->append('\n');
		axInt n = _level.size() + offset;
		for(axInt i = 0; i < n; i++) {
			_json->append("  ");
		}
	}
}

axJsonWriter::ObjectScope::ObjectScope(axJsonWriter* p)  {
	_p = p;
}

axJsonWriter::ObjectScope::ObjectScope(ObjectScope && rhs) noexcept {
	_p = rhs._p;
	rhs._p = nullptr;
}

axJsonWriter::ObjectScope::~ObjectScope() {
	if (_p) { _p->endObject(); _p = nullptr; }
}

axJsonWriter::ArrayScope::ArrayScope(axJsonWriter* p)  {
	_p = p;
}

axJsonWriter::ArrayScope::ArrayScope(ArrayScope && rhs) noexcept {
	_p = rhs._p;
	rhs._p = nullptr;
}

axJsonWriter::ArrayScope::~ArrayScope() {
	if (_p) { _p->endArray(); _p = nullptr; }
}
