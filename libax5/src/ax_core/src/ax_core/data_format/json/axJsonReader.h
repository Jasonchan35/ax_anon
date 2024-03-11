#pragma once

#include <ax_core/log/axLog.h>
#include "axJsonWriter.h"

class axJsonReaderError : public axError {};
class axJsonReaderError_EndObject : public axJsonReaderError {};
class axJsonReaderError_EndArray : public axJsonReaderError {};
class axJsonReaderError_UnhandledMember : public axJsonReaderError {
public:
	axJsonReaderError_UnhandledMember(axStrView8 name) {

	}
};
class axJsonReaderError_ReadMemberOutsideOfObject : public axJsonReaderError {};
class axJsonReaderError_ParseNumber : public axJsonReaderError {};
class axJsonReaderError_UnexpectedEndOfJson : public axJsonReaderError {};
class axJsonReaderError_UnexpectedStringEscapeChar : public axJsonReaderError {
public:
	axJsonReaderError_UnexpectedStringEscapeChar(char ch) {}
};

class axJsonReader;
template<class T> void axJsonReader_handler(axJsonReader& json_, T& value);

//! Copyable - no copy for json, therefore json and filename should not be changed during paring
class axJsonReader {
public:
	enum class ValueType {
		Invalid,
		BeginObject,
		EndObject,
		BeginArray,
		EndArray,
		Number,
		Bool,
		Null,
		String,
		Member,
	};

	axJsonReader();
	axJsonReader(axStrView json, axStrView filenameForErrorMessage);
	axJsonReader(axJsonReader && rhs) noexcept { operator=(rhs); }

	void readJson(axStrView json, axStrView filename);

	ValueType valueType() { return _valueType; }

	bool valueIsString	() const	{ return _valueType == ValueType::String; }
	bool valueIsNumber	() const	{ return _valueType == ValueType::Number; }
	bool valueIsBool	() const	{ return _valueType == ValueType::Bool; }
	bool valueIsObject	() const	{ return _valueType == ValueType::BeginObject; }
	bool valueIsArray	() const	{ return _valueType == ValueType::BeginArray; }
	bool valueIsNull	() const	{ return _valueType == ValueType::Null; }

	axStrView valueStrView() const	{ return valueIsString() ? _token.str.view() : axStrView(); }

	bool beginObject(axStrView name);
	void beginObject();
	bool endObject();

	bool beginArray(axStrView name);
	void beginArray();
	bool endArray();
	bool expectEndArray();

	bool isMember(axStrView name);
	axStrView isMemberHasPrefix(axStrView prefix);

	template<class T>
	bool readMember(axStrView name, T& out_value);

	void readMemberName(axIString& out_value);
	bool peekMemberName(axIString& out_value);

	void readValue(bool&			out_value);


	void readValue(axInt8&		out_value)	{ return _readNumericValue(out_value); }
	void readValue(axInt16&		out_value)	{ return _readNumericValue(out_value); }
	void readValue(axInt32&		out_value)	{ return _readNumericValue(out_value); }
	void readValue(axInt64&		out_value)	{ return _readNumericValue(out_value); }
	void readValue(axUInt8&		out_value)	{ return _readNumericValue(out_value); }
	void readValue(axUInt16&	out_value)	{ return _readNumericValue(out_value); }
	void readValue(axUInt32&	out_value)	{ return _readNumericValue(out_value); }
	void readValue(axUInt64&	out_value)	{ return _readNumericValue(out_value); }
	void readValue(axFloat32&	out_value)	{ return _readNumericValue(out_value); }
	void readValue(axFloat64&	out_value)	{ return _readNumericValue(out_value); }
	void readValue(axFloat128&	out_value)	{ return _readNumericValue(out_value); }

	template<class R>
	void readValue(axIStringT<R>&	out_value);

	template<class R, axInt N>
	void readValue(axStringT<R, N>&	out_value) { readValue(out_value.asIString()); }

	template<class R>
	void readValue(axMutSpan<R> out_value);

	template<class R>
	void readValue(axIArray<R>& out_value);

	template<class R, axInt N>
	void readValue(axArray<R, N>& out_value) { readValue(out_value.asIArray()); }

	template<class T>
	void readValue(T& value) { axJsonReader_handler(*this, value); }

	template<class T>
	void reflection(T& v);

	void skipValue();
	void skipMemberName();

	void reset();
	bool next();

	template<class... ARGS> void info	(axStrView fmtStr, ARGS&&... args) { log(axLog::Level::Info,	fmtStr, std::forward<ARGS>(args)...); }
	template<class... ARGS> void warning(axStrView fmtStr, ARGS&&... args) { log(axLog::Level::Warning,	fmtStr, std::forward<ARGS>(args)...); }
	template<class... ARGS> void error	(axStrView fmtStr, ARGS&&... args) { log(axLog::Level::Error,	fmtStr, std::forward<ARGS>(args)...); }

	void log(axLog::Level lv, axStrView fmtStr);
	template<class... ARGS>
	void log(axLog::Level lv, axStrView fmtStr, ARGS&&... args);
	void logWithParamList(axLog::Level lv, axStrView8 fmt, const axSpan<axFormatParam>& paramList);

	void dumpToken();
	void dumpValue();

	void getLocationString(axIString& out_str, int maxNumLines);

	axStrView		filenameForErrorMessage() const { return _filenameForErrorMessage; }

	axJsonReader	clone() const { return axJsonReader(*this); }

	bool unhandledMember();
	bool skipUnhandledMember(bool showWarning);
	bool onUnhandledMemeber();

	axInt lineNumber() const { return _lineNumber; }
	axInt column	() const { return _column; }

private:
	template<class T>
	struct ReflectionForEachHandler {
		using R = axReflection<T>;
		axJsonReader& rd;
		T& value;
		axInt readSuccess = 0;

		template<axInt index, class Field> AX_INLINE
		void handle() {
			auto& v = Field::s_value(&value);
			if (rd.readMember(Field::s_name(), v))
				readSuccess++;
		}
	};

	template<class V>
	void _readNumericValue(V&	out_value);

	axJsonReader(const axJsonReader& rhs) { operator=(rhs); }
	void operator=(const axJsonReader& rhs);

	enum class TokenType {
		Invalid,
		BeginObject,
		EndObject,
		BeginArray,
		EndArray,
		Number,
		Identifier,
		Bool,
		Null,
		String,
		Comma,
		Colon,
	};

	bool _nextToken();
	void _nextChar();

	void _parseStringToken();

	int _findPrevNewlineLocation(const char* &out_pos, int maxNumLines);
	int _findNextNewlineLocation(const char* &out_pos, int maxNumLines);

	char _ch;
	const char* _r = nullptr;
	const char* _start = nullptr;
	const char* _end = nullptr;

	struct Token {
		TokenType		type = TokenType::Invalid;
		axTempString	str;
		bool			boolValue = false;
	};

	axStrView	_filenameForErrorMessage;
	axInt		_lineNumber = 0;
	axInt		_column = 0;
	Token		_token;
	ValueType	_valueType = ValueType::Invalid;

	struct Level {
		enum class Type {
			None,
			Object,
			Array,
		};

		Level(Type type_ = Type::None, const char* pos_ = nullptr) : type(type_), pos(pos_) {}

		Type type;
		const char* pos;
	};

	Level::Type _currentLevelType() {
		return _levels.size() ? _levels.back().type : Level::Type::None;
	}

	axArray<Level, 32>	_levels;
};

template<class T> inline 
void axJsonReader_handler(axJsonReader& json_, T& value) {
	value.onJsonReader(json_);
}

template<class T>
bool axJsonReader::readMember(axStrView name, T& out_value) {
	if (!isMember(name)) return false;
	readValue(out_value);
	return true;
}

template<class V>
void axJsonReader::_readNumericValue(V& out_value) {
	if (_valueType != ValueType::Number) error("value is not number");
	if (!_token.str.tryParse(out_value)) {
		throw axJsonReaderError_ParseNumber();;
	}
	next();	
}

template<class T> inline
void axJsonReader::reflection(T& v) {
	ReflectionForEachHandler<T> h{ *this, v };
	using R = axReflection<T>;
	beginObject();
	while (!endObject()) {
		h.readSuccess = 0;
		axReflection<R>::Fields::s_forEachType(h);
		if (h.readSuccess > 0)
			continue;
		skipUnhandledMember(true);
	}
}

template<class... ARGS> inline
void axJsonReader::log(axLog::Level lv, axStrView fmtStr, ARGS&&... args) {
	axFormatParam tmp[] {ax_forward(args)...};
	logWithParamList(lv, fmtStr, axSpan_make(tmp));
}

inline
void axJsonReader::log(axLog::Level lv, axStrView fmtStr) {
	logWithParamList(lv, fmtStr, axSpan<axFormatParam>());
}


template<class R> inline
void axJsonReader::readValue(axIStringT<R>& out_value) {
	if (_valueType != ValueType::String) error("value is not String");
	out_value.setUtf(_token.str);
	next();
}

template<class R> inline
void axJsonReader::readValue(axMutSpan<R> out_value) {
	axInt i = 0;
	beginArray();
	while (!endArray()) {
		if (i >= out_value.size()) {
			warning("too many array elements");
			return;
		}
		readValue(out_value[i]);
		i++;
	}

	if (i != out_value.size()) {
		warning("array size mismatch");
	}
}

template<class R> inline
void axJsonReader::readValue(axIArray<R>& out_value) {
	out_value.clear();
	beginArray();
	while (!endArray()) {
		auto& e = out_value.emplaceBack();
		readValue(e);
	}
}

template<class T> inline
void axJsonReader_VoidPtrWrapper(axJsonReader& rd, void* data) {
	auto* p = reinterpret_cast<T*>(data);
	rd.readValue(*p);
}
