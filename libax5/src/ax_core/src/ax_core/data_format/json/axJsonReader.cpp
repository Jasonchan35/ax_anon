#include "axJsonReader.h"
#include <ax_core/file/axFile.h>

axJsonReader::axJsonReader() {
	reset();
}

axJsonReader::axJsonReader(axStrView json, axStrView filenameForErrorMessage) {
	reset();
	readJson(json, filenameForErrorMessage);
}

void axJsonReader::operator=(const axJsonReader& rhs) {
	_ch 		= rhs._ch;
	_r 			= rhs._r;
	_start 		= rhs._start;
	_end 		= rhs._end;

	_filenameForErrorMessage = rhs._filenameForErrorMessage;
	_lineNumber = rhs._lineNumber;
	_column		= rhs._column;
	_token 		= rhs._token;
	_valueType 	= rhs._valueType;
	_levels		= rhs._levels;
}

void axJsonReader::reset() {
	_lineNumber = 0;
	_column = 0;
	_valueType = ValueType::Invalid;

	_start = nullptr;
	_end   = nullptr;
	_r     = nullptr;
}

void axJsonReader::readJson(axStrView json, axStrView filenameForErrorMessage) {
	_filenameForErrorMessage = filenameForErrorMessage;
	_start = json.data();
	_end = _start + json.size();
	_r = _start;

	_nextChar();
	next();
}

void axJsonReader::beginObject() {
	if (_valueType != ValueType::BeginObject) {
		error("ValueType is not {{");
	}
	next();
}

bool axJsonReader::beginObject(axStrView name) {
	if (!isMember(name)) return false;
	beginObject();
	return true;
}

bool axJsonReader::endObject() {
	if (_valueType != ValueType::EndObject) {
		if (onUnhandledMemeber()) return false;

		if (!_levels.size()) {
			throw axJsonReaderError_EndObject();
		}
		_levels.back().pos = _r;
		if (_valueType != ValueType::EndObject) { // valueType may changed after skipValue
			return false;
		}
	}		
	next();
	return true;
}

bool axJsonReader::unhandledMember() {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("value is not member");

	if (!_levels.size()) {
		throw axJsonReaderError_ReadMemberOutsideOfObject();
	}
	
	auto& lv = _levels.back();
	if (lv.type != Level::Type::Object) {
		throw axJsonReaderError_ReadMemberOutsideOfObject();
	}
	
	if (lv.pos == _r) {
		return true;
	}
	return false;
}

bool axJsonReader::skipUnhandledMember(bool showWarning) {
	if (!unhandledMember()) return false;
	if (showWarning) {
		axTempString loc;
		getLocationString(loc, 3);
		warning("unhandled object member \"{?}\"\n{?}", _token.str, loc);
	}
	skipMemberName();
	skipValue();
	return true;
}

bool axJsonReader::onUnhandledMemeber() {
	if (!unhandledMember()) return false;
	warning("unhandled object member \"{?}\"", _token.str);
	skipValue();
	return true;
}

void axJsonReader::beginArray() {
	if (_valueType != ValueType::BeginArray) error("ValueType is not [");
	next();
}

bool axJsonReader::beginArray(axStrView name) {
	if (!isMember(name)) return false;
	beginArray();
	return true;
}

bool axJsonReader::endArray() {
	if (_valueType != ValueType::EndArray) {
		if (!_levels.size()) {
			throw axJsonReaderError_EndArray();
		}
		return false;
	}
	next();
	return true;
}

bool axJsonReader::expectEndArray() {
	if (_valueType == ValueType::EndArray)
		return true;

	warning("too many values in array");
	do {
		skipValue();
	} while (!endArray());

	return true;
}

bool axJsonReader::isMember(axStrView name) {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("value is not member");
	if (_token.str != name) return false;
	next();
	return true;
}

axStrView axJsonReader::isMemberHasPrefix(axStrView prefix) {
	if (_valueType != ValueType::Member) error("value is not member");
	auto name = axStrView(_token.str);	
	auto suffix = name.extractFromPrefix(prefix);
	if (suffix) next();
	return suffix;
}

void axJsonReader::readMemberName(axIString& out_value) {
	if (_valueType != ValueType::Member) error("is not member name");
	peekMemberName(out_value);
	next();
}

bool axJsonReader::peekMemberName(axIString& out_value) {
	if (_valueType == ValueType::EndObject) return false;
	if (_valueType != ValueType::Member) error("is not member name");
	out_value = _token.str;
	return true;
}

void axJsonReader::skipMemberName() {
	if (_valueType != ValueType::Member) error("is not member name");
	next();	
}

void axJsonReader::readValue(bool& out_value) {
	if (_valueType != ValueType::Bool) error("value is not bool");
	out_value = _token.boolValue;
	next();
}

void axJsonReader::skipValue() {
	switch (_valueType) {
		case ValueType::BeginArray: {
			beginArray();
			while (!endArray()) {
				skipValue();
			}
		}break;

		case ValueType::BeginObject: {
			beginObject();
			while (!endObject()) {
				skipValue();
			}
		}break;

		case ValueType::Member: {
			next();
			skipValue();
		}break;

		default: {
			next();
		}break;
	}
}

bool axJsonReader::next() {
	auto lastValueType = _valueType;
	auto lastTokenType = _token.type;

	bool b = _nextToken();
	if (!b) return false;

	if (_currentLevelType() == Level::Type::Object) {
		if (lastValueType != ValueType::Member) {
			if (_token.type == TokenType::EndObject) {
				_levels.popBack();
				_valueType = ValueType::EndObject;
				return true;
			}

			if (lastTokenType != TokenType::BeginObject) {
				if (_token.type == TokenType::Comma) {
					_nextToken();
				} else {
					error("comma[,] expected between members");
				}
			}

			if (_token.type == TokenType::Identifier) {
				error("Quote [\"] is missing for member name [{?}]", _token.str);

			} else if (_token.type != TokenType::String) {
				error("Member name must be string, token={?}", _token.str);
			}

			_valueType = ValueType::Member;
			return true;
		}

		if (_token.type != TokenType::Colon) {
			error("colon[:] expected after member name");
		}
		_nextToken();

	} else if (_currentLevelType() == Level::Type::Array) {
		if (_token.type == TokenType::EndArray) {
			_levels.popBack();
			_valueType = ValueType::EndArray;
			return true;
		}

		if (lastTokenType != TokenType::BeginArray) {
			if (_token.type == TokenType::Comma) {
				_nextToken();
			} else {
				error("comma[,] expected between elements");
			}
		}
	}

	switch (_token.type) {
		case TokenType::BeginObject:
		{
			_levels.emplaceBack(Level::Type::Object, _r);
			_valueType = ValueType::BeginObject;
			return true;
		}break;

		case TokenType::BeginArray:
		{
			_levels.emplaceBack(Level::Type::Array, _r);
			_valueType = ValueType::BeginArray;
			return true;
		}break;

		case TokenType::Null:	 _valueType = ValueType::Null;		break;
		case TokenType::Bool:	 _valueType = ValueType::Bool;		break;
		case TokenType::Number:	 _valueType = ValueType::Number;	break;
		case TokenType::String:	 _valueType = ValueType::String;	break;
		default: error("Error invalid type in next()");
	}
	return true;
}

void axJsonReader::dumpToken() {
	switch (_token.type) {
		case TokenType::Invalid:		AX_LOG("Token[Invalid]"); break;
		case TokenType::BeginObject:	AX_LOG("Token({)"); break;
		case TokenType::EndObject:		AX_LOG("Token(})"); break;
		case TokenType::BeginArray:		AX_LOG("Token([)"); break;
		case TokenType::EndArray:		AX_LOG("Token(])"); break;
		case TokenType::Null:			AX_LOG("Token(null)"); break;
		case TokenType::Number:			AX_LOG("Token(", _token.str, ")"); break;
		case TokenType::Bool:			AX_LOG("Token({?}", _token.boolValue); break;
		case TokenType::String:			AX_LOG("Token(str=", _token.str, ")"); break;
		case TokenType::Comma:			AX_LOG("Token(,)"); break;
		case TokenType::Colon:			AX_LOG("Token(:)"); break;
		default:
			error("Unknown token type");
	}
}

void axJsonReader::dumpValue() {
	switch (_valueType) {
		case ValueType::Invalid:		AX_LOG("JsValue[Invalid]"); break;
		case ValueType::BeginObject:	AX_LOG("JsValue({)"); break;
		case ValueType::EndObject:		AX_LOG("JsValue(})"); break;
		case ValueType::BeginArray:		AX_LOG("JsValue([)"); break;
		case ValueType::EndArray:		AX_LOG("JsValue(])"); break;
		case ValueType::Null:			AX_LOG("JsValue(null)"); break;
		case ValueType::Number:			AX_LOG("JsValue(", _token.str, ")"); break;
		case ValueType::Bool:			AX_LOG("JsValue({?})", _token.boolValue); break;
		case ValueType::String:			AX_LOG("JsValue(str=", _token.str, ")"); break;
		case ValueType::Member:			AX_LOG("JsValue(member=", _token.str, ")"); break;
		default: error("Unknown value type");
	}
}

bool axJsonReader::_nextToken() {
	_valueType = ValueType::Invalid;
	_token.type = TokenType::Invalid;
	_token.str.clear();

	for (;;) {
		//trim space
		while (_ch == ' ' || _ch == '\t' || _ch == '\n' || _ch == '\r') {
			_nextChar();
		}

		if (_ch == '/') {
			_nextChar();
			switch (_ch) {
				case '/': { 
					//trim inline comment
					_nextChar();
					while (_ch != '\n' && _ch != 0) {
						_nextChar();
					}
				}continue; //back to trim space again

				case '*': {
					//trim block comment
					_nextChar();
					for (;;) {
						if (_ch == 0) {
							error("Unexpected end of file in comment block");
						}

						if (_ch == '*') {
							_nextChar();
							if (_ch == '/') {
								_nextChar();
								break;
							}
						}else{
							_nextChar();
						}
					}
				}continue;

				default: error("Invalid token after '/'");
			}
		}

		break;
	}

	switch (_ch) {
		case 0: return false;
		case '{': _token.type = TokenType::BeginObject;	_nextChar(); return true;
		case '}': _token.type = TokenType::EndObject;	_nextChar(); return true;
		case '[': _token.type = TokenType::BeginArray;	_nextChar(); return true;
		case ']': _token.type = TokenType::EndArray;	_nextChar(); return true;
		case ',': _token.type = TokenType::Comma;		_nextChar(); return true;
		case ':': _token.type = TokenType::Colon;		_nextChar(); return true;
		case '"':
		{
			_token.type = TokenType::String;
			_parseStringToken();
			return true;
		};
	}

	for (;;) {
		if (isdigit(_ch) || isalpha(_ch) || _ch == '+' || _ch == '-' || _ch == '.') {
			_token.str += _ch;
			_nextChar();
		} else {
			break;
		}
	}

	if (_token.str == "null") {
		_token.type = TokenType::Null;
		return true;
	}

	if (_token.str == "true") {
		_token.type = TokenType::Bool;
		_token.boolValue = true;
		return true;
	}

	if (_token.str == "false") {
		_token.type = TokenType::Bool;
		_token.boolValue = false;
		return true;
	}

	if (_token.str.size() > 0) {
		auto c = _token.str[0];
		if (isdigit(c) || c == '+' || c == '-' || c == '.') {
			_token.type = TokenType::Number;
			return true;
		}

		if (isalpha(c) || c == '_') {
			_token.type = TokenType::Identifier;
			return true;
		}
	}

	error("error token=[{?}]", _token.str);
	return false;
}

void axJsonReader::_parseStringToken() {
	for (;;) {
		_nextChar();
		if (_ch == 0) {
			throw axJsonReaderError_UnexpectedEndOfJson();
		}
		switch (_ch) {
			case '"': _nextChar(); return;
				//escape char
			case '\\':
			{
				_nextChar();
				switch (_ch) {
					case '\\':
					case '/':
					case '"':
						_token.str += _ch;
						break;
					case 'b': _token.str += '\b'; break;
					case 'f': _token.str += '\f'; break;
					case 'n': _token.str += '\n'; break;
					case 'r': _token.str += '\r'; break;
					case 't': _token.str += '\t'; break;
					default:
						throw axJsonReaderError_UnexpectedStringEscapeChar(_ch);
				}
			}break;

			default:
			{
				_token.str += _ch;
			}break;
		}
	}
}

void axJsonReader::getLocationString(axIString& s, int maxNumLines) {
	if (!_start) return;

	s.clear();

	const char* lineStart  = nullptr;
	const char* lineEnd    = nullptr;
	const char* blockStart = nullptr;

	if (!_findPrevNewlineLocation(lineStart, 1)) {
		lineStart = _start;
	}

	if (!_findPrevNewlineLocation(blockStart, maxNumLines)) {
		blockStart = _start;
	}

	if (!_findNextNewlineLocation(lineEnd, 1)) {
		//throw Error("cannot find line end");
		lineEnd = _end;
	}

	s.appendFormat("\n{?}:{?},{?}\n", _filenameForErrorMessage, _lineNumber, _column);
	s.append(axStrView8(blockStart, lineEnd - blockStart));

	s.append('\n');

	for (auto* p = lineStart; p < _r; p++) {
		switch (*p) {
			case '\t': s.append('\t'); break;
			default:   s.append(' '); break;
		}
	}
	
	s.append("^~~~~");
}

int axJsonReader::_findPrevNewlineLocation(const char* &out_pos, int maxNumLines) {
	if (!_start || !_r) return 0;
	int found = 0;
	auto* p = _r - 1;
	for (; p >= _start; p--) {
		if (*p != '\n') continue;
		found++;
		out_pos = p;
		if (found >= maxNumLines) break;
	}
	return found;
}

int axJsonReader::_findNextNewlineLocation(const char* &out_pos, int maxNumLines) {
	if (!_start || !_r) return 0;
	int found = 0;
	auto* p = _r;
	for (; p < _end; p++) {
		if (*p != '\n') continue;
		found++;
		out_pos = p;
		if (found >= maxNumLines) break;
	}
	return found;
}

void axJsonReader::_nextChar() {
	if (_r >= _end) {
		_ch = 0;
		return;
	}
	_ch = *_r;
	_r++;

	if (_ch == '\n') {
		_lineNumber++;
		_column = 0;
	} else {
		_column++;
	}
}

void axJsonReader::logWithParamList(axLog::Level lv, axStrView8 fmt, const axSpan<axFormatParam>& paramList) {
	axString jsonLoc;
	getLocationString(jsonLoc, 2);

	axString s;
	s.appendFormatWithParamList(fmt, paramList);
	s.append(jsonLoc);

	axLog::log(AX_LOC, lv, "{?}", s);

	if (lv == axLog::Level::Error) {
		throw axJsonReaderError();
	}
}
