#include "InfoParser.h"

void InfoParser::readFile(axStrView outdir, axStrView filename) {
	axFile::readUtf8(filename, _source);
	_cur = _source.data();

	nextChar();

	nextToken();
	if (_token.isIdentifier("Shader")) { 
		readShader();
	} else {
		errorUnexpectedToken();
		return;
	}

	{
		axString outFilename;
		outFilename.set(outdir, "/info.json");

		axJsonUtil::writeFileIfChanged(outFilename, info, true);
	}

	{
		axString outFilename;
		outFilename.set(outdir, "/source.hlsl");

		axInt offset = _cur - _source.data();
		for (auto& c : _source.slice(0, offset)) {
			if (c != '\n') {
				c = ' ';
			}
		}

		axFile::writeFileIfChanged(outFilename, _source, false);
	}
}

void InfoParser::readShader() {
	nextToken();
	expectOp("{");

	for(;;) {
		if (_token.isOp("}"))	{ nextToken(); break; }
		if (_token.isNewline()) { nextToken(); continue; }
		if (_token.isIdentifier("Properties")) { readProperties(); continue; }
		if (_token.isIdentifier("Pass")) { readPass(); continue; }
		return errorUnexpectedToken();
	}
}

void InfoParser::readProperties() {
	nextToken();
	expectOp("{");

	for(;;) {
		if (_token.isOp("}"))	{ nextToken(); break; }
		if (_token.isNewline()) { nextToken(); continue; }
		if (_token.isIdentifier()) {
			readProperty();
			continue;
		}
		return errorUnexpectedToken();
	}
}

void InfoParser::readProperty() {
	// prop type
	if (!_token.isIdentifier()) {
		errorUnexpectedToken();
		return; 
	}
	auto& o = info.props.emplaceBack();

	readEnum(o.propType);

	// prop name
	if (!_token.isIdentifier()) {
		errorUnexpectedToken();
		return; 
	}
	o.name = _token.str;
	nextToken();

	// optional defaultValue
	if (_token.isOp("(")) {
		nextToken();
		for(;;) {
			if (_token.isOp(")"))	{ nextToken(); break; }
			if (_token.isNewline()) { nextToken(); continue; }
			o.defaultValue += _token.str;
			nextToken();
		}
	}

	// optional displayName
	if (_token.isString()) readString(o.displayName);

	if (!_token.isNewline()) {
		errorUnexpectedToken();
	}
	nextToken();
}

void InfoParser::readBlendFunc(BlendFunc& f) {
	nextToken();			
	readEnum(f.op);
	readEnum(f.srcFactor);
	readEnum(f.dstFactor);
}

void InfoParser::readPass() {
	nextToken();
	auto& o = info.passes.emplaceBack();
	readString(o.name);
	expectOp("{");

	for(;;) {
		if (_token.isOp("}"))	{ nextToken(); break; }
		if (_token.isNewline()) { nextToken(); continue; }
		if (_token.isIdentifier("Cull")) {
			nextToken();
			readEnum(o.renderState.cull);
			continue;
		}
		if (_token.isIdentifier("AntialiasedLine")) {
			nextToken();
			readBool(o.renderState.antialiasedLine);
			continue;
		}
		if (_token.isIdentifier("Wireframe")) {
			nextToken();
			readBool(o.renderState.wireframe);
			continue;
		}
		if (_token.isIdentifier("DepthTest") ) { 
			nextToken();
			DepthTestOp v;
			readEnum(v);
			o.renderState.depthTest.op = v;
			continue; 
		}
		if (_token.isIdentifier("DepthWrite")) {
			nextToken();
			bool b;
			readBool(b);
			o.renderState.depthTest.writeMask = b;
			continue;
		}
		if (_token.isIdentifier("BlendRGB")) {
			readBlendFunc(o.renderState.blend.rgb);
			continue;
		}
		if (_token.isIdentifier("BlendAlpha")) {
			readBlendFunc(o.renderState.blend.alpha);
			continue;
		}

		if (_token.isIdentifier("VsFunc")) { nextToken(); readIdentifier(o.vsFunc); continue; }
		if (_token.isIdentifier("PsFunc")) { nextToken(); readIdentifier(o.psFunc); continue; }
		if (_token.isIdentifier("CsFunc")) { nextToken(); readIdentifier(o.csFunc); continue; }
		if (_token.isIdentifier("RayTracingFunc")) { nextToken(); readIdentifier(o.rayTracingFunc); continue; }
		return errorUnexpectedToken();
	}
}


bool InfoParser::nextToken() {
	if (!_nextToken()) return false;
//	AX_DUMP_VAR(_token.str);
	return true;
}

bool InfoParser::_nextToken() {
	_token.type = TokenType::Unknown;
	_token.str.clear();

	for (;;) {
		_trimSpaces();
		if (!_ch) return false;

		if (_ch == '_' || axCStr::isAlpha(_ch)) {
			return _parseIdentifier();
		}

		if (axCStr::isDigit(_ch)) return _parseNumber();
		if (_ch == '\"') return _parseString();

		if (_ch == '\n') {
			_token.type = TokenType::Newline;
			_token.str += "<newline>";
			nextChar();
			return true;
		}

		if (_ch == '/') {
			nextChar();			
			if (_ch == '*') { 
				_parseCommentBlock();
				continue;
			}

			if (_ch == '/') { 
				_parseCommentInline();
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

void InfoParser::errorUnexpectedChar() {
	error("Unexpected character [{?}]", _ch);
}

void InfoParser::errorUnexpectedToken() {
	error("Unexpected token [{?}]", _token.str);
}

bool InfoParser::_parseIdentifier() {
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

bool InfoParser::_parseNumber() {
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

bool InfoParser::_parseString() {
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

void InfoParser::_parseCommentBlock() {
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

void InfoParser::_parseCommentInline() {
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

void InfoParser::expectOp(axStrView s) {
	if (_token.isOp() && _token.str == s) {
		nextToken();
		return;
	}
	error("expected token {?}", s);
}

void InfoParser::readIdentifier(axIString& s) {
	if (!_token.isIdentifier()) errorUnexpectedToken();
	s = _token.str;
	nextToken();
}

void InfoParser::readString(axIString& s) {
	if (!_token.isString()) errorUnexpectedToken();
	s = _token.str;
	nextToken();
}

void InfoParser::readBool(bool& v) {
	if (!_token.isIdentifier()) errorUnexpectedToken();
	if (_token.str == "true") {
		v = true;
	}else if(_token.str == "false") {
		v = false;
	}else{
		errorUnexpectedToken();
	}
	nextToken();
}

bool InfoParser::nextChar() {
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

void InfoParser::_trimSpaces() {
	for (;;) {
		if (_ch == ' '	|| _ch == '\t' || _ch == '\r') {
			nextChar();
		}else{
			break;
		}
	}
}

