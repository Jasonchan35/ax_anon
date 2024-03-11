#include "Parser.h"

namespace axNodeGen {

void Parser::readFile(axStrView filename, TypeDB& typeDB) {
	_filename = filename;
	_typeDB = &typeDB;

	axFile::readUtf8(filename, _source);
	_cur = _source.data();

	nextChar();
	nextToken();
	
	parseNamespace();
}

void Parser::parseNamespace() {
	do {
		if (_token.isOp("}")) break;

		if (_token.isNewline()) return;

		if (_token.isIdentifier("AX_CLASS")) {
			parseClass();
			continue;
		}
		if (_token.isIdentifier("namespace")) {
			nextToken();

			axTempString tmp;
			readIdentifer(tmp, "namespace name");

			_namespaces.emplaceBack(tmp);

			readExpectOp("{");

			parseNamespace();
			continue;
		}
	} while (nextToken());

	if (_namespaces.size()) {
		_namespaces.popBack();
	}
}

void Parser::parseClass() {
	nextToken();
	readExpectOp("(");
	while (!_token.isOp(")")) {
		if (!nextToken()) {
			errorUnexpectedEndOfFile("CLASS attributes");
		}
	}
	nextToken();
	readExpectIdentifer("class");

	axTempString tmp;
	readIdentifer(tmp, "class name");

	if (_typeDB->types.find(tmp)) {
		error("class {?} already exists", tmp);
	}
	auto& outType = _typeDB->types.add(tmp);
	outType.name = tmp;

	//--- namespace scope ---
	getOpenNamespaceScope (outType.openNamespaceScope,  _namespaces);
	getCloseNamespaceScope(outType.closeNamespaceScope, _namespaces);

	//--- fullname ---
	getNamespaceString(outType.fullname, _namespaces);
	if (outType.fullname) {
		outType.fullname.append("::");
	}
	outType.fullname.append(tmp);
	//-------

	if (_token.isOp(":")) {
		nextToken();
		readExpectIdentifer("public");	
		readIdentifer(tmp, "base class");
		outType.baseName = tmp;		
	}

	if (_token.isOp(",")) {
		nextToken();

		if (_token.isIdentifier("public") || _token.isIdentifier("protected") || _token.isIdentifier("private")) {
			nextToken();
		}
		readIdentifer(tmp, "other base class");

		if (_token.isOp("<")) {
			skipBlock();
		}
	}

// class Body
	readExpectOp("{");
	while (!_token.isOp("}")) {
		if (_token.isIdentifier("AX_PROP")) {
			parseProp(outType);
			continue;
		}

		if (_token.isOp("{")) {
			skipBlock();
			continue;
		}

		if (!nextToken()) {
			errorUnexpectedEndOfFile("CLASS body");
		}
	}
}

void Parser::parseProp(TypeInfo& outType) {
	PropInfo::HasAttr	hasAttr;
	axArray<Attribute>	attributes;

	nextToken();
	readExpectOp("(");
	for (;;) {
		if (_token.isOp(")")) break;

		auto& attr = attributes.emplaceBack();
		readIdentifer(attr.name, "read prop attribute");

		if (_token.isOp("(") || _token.isOp("{")) {
			readValueString(attr.defaultValue, "read prop attribute default value");
		}

		if (attr.name == "DontSave") {
			hasAttr.dontSave = true;
		}

		if (_token.isOp(",")) {
			nextToken();
		}
	}
	nextToken();

	for(;;) {
		if (_token.isIdentifier("static")) {
			error("doesn't support static property");
		}
		if (_token.isIdentifier("const")) {
			error("doesn't support const property");
		}
		break;
	}

	axTempString typeName;
	readIdentifer(typeName, "property typename");

	if (_token.isOp("<")) {
		readAppendTemplateParam(typeName);
	}

	if (_token.isOp("*")) {
		error("doesn't support pointer property");
	}

	axTempString varName;
	readIdentifer(varName, "property name");

	axTempString defaultValue;
	readDefaultValue(defaultValue, "property defaultValue");

	axTempString propName;
	axTempString displayName;
	{
		auto tmp = varName.view();
		if (tmp.startsWith("_")) {
			tmp = tmp.sliceFrom(1);
		}
		propName = tmp;
		convertToDisplayName(displayName, tmp);
	}

	if (outType.props.find(propName)) {
		error("property [{?}.{?}] already exists", outType.name, displayName);
	}

	auto& prop = outType.props.add(propName);
	prop.name			= propName;
	prop.varName		= varName;
	prop.displayName	= displayName;
	prop.typeName		= typeName;
	prop.propType		= PropType::Normal;
	prop.defaultValue	= defaultValue;
	prop.hasAttr		= hasAttr;
	prop.attributes		= attributes;

	while (!_token.isOp(";")) {
		if (!nextToken()) {
			errorUnexpectedEndOfFile("Property ';'");
		}		
	}
}

void Parser::readAppendTemplateParam(axIString & outStr) {
	outStr.append("<");
	readExpectOp("<");
	while (!_token.isOp(">")) {
		if (_token.isOp("<")) {
			readAppendTemplateParam(outStr);
			continue;
		}
		outStr.append(_token.str);

		if (!nextToken()) {
			errorUnexpectedEndOfFile("AX_PROP typename");
		}
	}
	outStr.append(">");
	nextToken();
}

void Parser::skipBlock(axIString* appendToStr) {
	axStrLiteral start;
	axStrLiteral end;
	if (_token.str == "{") {
		start = "{";
		end = "}";
	}else if (_token.str == "(") {
		start = "(";
		end = ")";
	}else if (_token.str == "[") {
		start = "[";
		end = "]";
	}else if (_token.str == "<") {
		start = "<";
		end = ">";
	}else{
		error("unknown block begin {?}", _token.str);
	}

	if (appendToStr) {
		appendToStr->append(_token.str);
	}

	nextToken();
	for(;;) {
		if (appendToStr) {
			appendToStr->append(_token.str);
		}

		if (_token.isOp(start)) {
			skipBlock(appendToStr);
			continue;
		}

		if (_token.isOp(end)) {
			nextToken();
			return;
		}

		if (!nextToken()) {
			axTempString msg("skipBlock ", start);
			errorUnexpectedEndOfFile(msg);
		}
	}	
}

void Parser::readExpectIdentifer(axStrView s) {
	if (!_token.isIdentifier(s)) {
		error("token [{?}] expected", s);
	}
	nextToken();
}

void Parser::readExpectOp(axStrView s) {
	if (!_token.isOp(s)) {
		error("token [{?}] expected", s);
	}
	nextToken();
}

void Parser::readIdentifer(axIString & s, axStrView msg) {
	if (!_token.isIdentifier()) {
		error("error read {?}", msg);
	}
	s = _token.str;
	nextToken();
}

void Parser::readValueString(axIString & s, axStrView msg) {
	s.clear();

	axStrLiteral	beginOp = "";
	axStrLiteral	endOp   = "";

	if (_token.isOp("{")) {
		beginOp = "{";
		endOp   = "}";
	} else if (_token.isOp("(")) {
		beginOp = "(";
		endOp   = ")";
	}

	if (beginOp) {
		s.append(_token.str);
		nextToken();

		axInt lv = 1;
		for (;;) {
			if (_token.isOp(beginOp)) {
				s.append(_token.str);
				nextToken();
				lv++;
				continue;
			}

			if (_token.isOp(endOp)) {
				s.append(_token.str);
				nextToken();
				lv--;
				if (lv <= 0)
					break;
				continue;
			}

			s.append(_token.str);
			nextToken();
		}		
	} else {
		s.append(_token.str);
		nextToken();
	}
}

void Parser::readDefaultValue(axIString & s, axStrView msg) {
	s.clear();
	if (_token.isOp(";")) {
		return;
	}

	if (_token.isOp("=")) {
		nextToken();
		for (;;) {
			if (_token.isOp(";")) break;
			s.append(_token.str);
			nextToken();
		}

	} else if (_token.isOp("{")) {
		readValueString(s, msg);
	}
}

bool Parser::nextToken() {
	if (!_nextToken()) return false;
	//AX_DUMP_VAR(_token.str);
	return true;
}

bool Parser::_nextToken() {
	_token.type = TokenType::Unknown;
	_token.str.clear();

	for (;;) {
		_trimSpaces();
		if (!_ch) return false;

		if (_ch == '_' || axCStr::isAlpha(_ch)) {
			return _parseIdentifier();
		}

		if (_ch == '+' || _ch == '-' || axCStr::isDigit(_ch)) {
			return _parseNumber();
		}

		if (_ch == '\"') return _parseString();
		if (_ch == '/') {
			nextChar();			
			if (_ch == '*') { 
				_parseMultiLineComment();
				continue;
			}

			if (_ch == '/') { 
				_parseSingleLineComment();
				continue;
			}

			_token.type = TokenType::Op;
			_token.str += _ch;
			return true;
		}


		_token.type = TokenType::Op;
		_token.str += _ch;
		nextChar();
		return true;
	}
}

void Parser::errorUnexpectedChar() {
	error("Unexpected character [{?}]", _ch);
}

void Parser::errorUnexpectedToken() {
	error("Unexpected token [{?}]", _token.str);
}

bool Parser::_parseIdentifier() {
	_token.type = TokenType::Identifier;

	_token.str += _ch;
	nextChar();

	while (_ch) {
		if (_ch == '_' || axCStr::isAlpha(_ch) || axCStr::isDigit(_ch)) {
			_token.str += _ch;
			nextChar();
		}else{
			break;
		}
	}
	return true;
}

bool Parser::_parseNumber() {
	_token.type = TokenType::Number;
	_token.str += _ch;
	nextChar();

	bool hasDot = false;

	while(_ch) {
		if (_ch == '.') {
			if (hasDot) {
				errorUnexpectedChar();
			}
			hasDot = true;
			_token.str += _ch;
			nextChar();
		}else if (axCStr::isDigit(_ch)) {
			_token.str += _ch;
			nextChar();
		}else{
			break;
		}
	}
	
	return true;
}

bool Parser::_parseString() {
	_token.type = TokenType::String;

	for (;;) {
		nextChar();
		if (_ch == '\\') {
			nextChar();
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
					throw axError_Undefined(AX_LOC);
			}
		}else if (_ch == '\"') {
			nextChar();
			break;
		}else{
			_token.str += _ch;
		}
	}
	return true;
}

void Parser::appendQuotedString(axIString& outStr, axStrView inStr) {
	outStr.append('"');
	for (auto& ch : inStr) {
		switch (ch) {
			case '\\':	outStr.append("\\\\"); break;
			case '\"':	outStr.append("\\\""); break;
			case '\b':	outStr.append("\\b"); break;
			case '\f':	outStr.append("\\f"); break;
			case '\n':	outStr.append("\\n"); break;
			case '\r':	outStr.append("\\r"); break;
			case '\t':	outStr.append("\\t"); break;
			default:	outStr.append(ch); break;
		}		
	}
	outStr.append('"');
}

void Parser::getOpenNamespaceScope(axIString& outStr, axIArray<axString>& ns) {
	outStr.clear();
	if (ns.size()) {
		for (auto& e : ns) {
			outStr.append("namespace ", e, "{ ");
		}
		outStr.append("\n\n");
	}
}

void Parser::getCloseNamespaceScope(axIString& outStr, axIArray<axString>& ns) {
	outStr.clear();
	if (ns.size()) {
		for (auto& e : ns) {
			AX_UNUSED(e);
			outStr.append("}");
		}
		outStr.append(" // namespace \n");
	}
}

void Parser::getNamespaceString(axIString& outStr, axIArray<axString>& ns) {
	outStr.clear();
	for (auto& e : ns) {
		if (outStr.size()) {
			outStr.append("::");
		}
		outStr.append(e);
	}
}

void Parser::_parseMultiLineComment() {
	nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '*') {
			nextChar();
			if (_ch == '/') {
				nextChar();
				return;
			}
		}else{
			nextChar();
		}
	}
}

void Parser::_parseSingleLineComment() {
	nextChar();			
	for(;;) {
		if (!_ch) return;
		if (_ch == '\n') {
			nextChar();
			return;
		}else{
			nextChar();
		}
	}
}

bool Parser::nextChar() {
	_ch = 0;
	if (!_cur) return false;
	if (_cur >= _source.end()) return false;
	_ch = *_cur;
	_cur++;

	if (_ch == '\n') {
		_lineNumber++;
	}

	return true;
}

void Parser::errorUnexpectedEndOfFile(axStrView from) {
	error("unexpected end of file for {?}", from);
}

void Parser::_trimSpaces() {
	for (;;) {
		if (_ch == ' '	|| _ch == '\t' || _ch == '\r' || _ch == '\n') {
			nextChar();
		}else{
			break;
		}
	}
}

} //namespace
