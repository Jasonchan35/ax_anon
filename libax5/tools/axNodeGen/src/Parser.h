#pragma once

#include "TypeInfo.h"

namespace axNodeGen {

class Parser : public axNonCopyable {
public:
	void readFile(axStrView filename, TypeDB& typeDB);

	void parseNamespace();
	void parseClass();
	void parseProp(TypeInfo& outType);
	
	void skipBlock(axIString* appendToStr = nullptr);

	void readExpectIdentifer(axStrView s);
	void readExpectOp(axStrView s);
	void readIdentifer(axIString & s, axStrView msg);
	void readValueString(axIString& s, axStrView msg);
	void readDefaultValue(axIString & s, axStrView msg);
	void readAppendTemplateParam(axIString & outStr);

	bool nextToken();
	bool nextChar();

	void errorUnexpectedEndOfFile(axStrView from);
	void errorUnexpectedChar();
	void errorUnexpectedToken();

	template<class Format, class... Args>
	void log(const Format& fmt, const Args&... args) {
		axTempString tmp;
		tmp.format(fmt, args...);
		AX_LOG("{?}\n{?}:{?}\n_token=[{?}]", tmp, _filename, _lineNumber, _token.str);
	}

	template<class Format, class... Args>
	void error(const Format& fmt, const Args&... args) {
		log(fmt, args...);
		throw axError_Undefined(AX_LOC);
	}

	enum class TokenType {
		Unknown,
		Identifier,
		Number,
		String,
		Op,
		Newline,
	};

	struct Token {
		TokenType	type;
		axString	str;

		explicit operator bool() const			{ return type != TokenType::Unknown; }
		bool isIdentifier() const				{ return type == TokenType::Identifier; }
		bool isIdentifier(axStrView s) const	{ return type == TokenType::Identifier && str == s; }
		bool isOp() const						{ return type == TokenType::Op; }
		bool isOp(axStrView s) const			{ return type == TokenType::Op && str == s; }
		bool isString() const					{ return type == TokenType::String; }
		bool isString(axStrView s) const		{ return type == TokenType::String && str == s; }
		bool isNewline() const					{ return type == TokenType::Newline; }
	};

	Token& token() { return _token; }

private:
	bool _nextToken();
	void _trimSpaces();
	bool _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	void _parseMultiLineComment();
	void _parseSingleLineComment();
	void appendQuotedString(axIString& outStr, axStrView inStr);

	void getNamespaceString(axIString& outStr, axIArray<axString>& ns);

	void getOpenNamespaceScope (axIString& outStr, axIArray<axString>& ns);
	void getCloseNamespaceScope(axIString& outStr, axIArray<axString>& ns);

	axArray<axString> _namespaces;

	Token			_token;
	axString		_source;
	axString		_filename;
	axInt			_lineNumber = 1;
	axChar			_ch = 0;
	const axChar*	_cur = nullptr;

	TypeDB*			_typeDB = nullptr;
};


} //namespace