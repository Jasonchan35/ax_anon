#pragma once

#include <ax_render.h>

class InfoParser : public axNonCopyable {
public:
	using DepthTestOp = axRenderState_DepthTestOp;
	using BlendFunc   = axRenderState::BlendFunc;
	using BlendFactor = axRenderState_BlendFactor;
	using BlendOp     = axRenderState_BlendOp;

	void readFile(axStrView outdir, axStrView filename);

	bool nextToken();
	bool nextChar();

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

	axShaderInfo info;

private:
	void _trimSpaces();
	bool _nextToken();

	template<class Format, class... Args>
	void log(const Format& fmt, const Args&... args) {
		axTempString tmp;
		tmp.format(fmt, args...);
		AX_LOG("{?}\n{?}:{?}", tmp, _filename, _lineNumber);
	}

	template<class Format, class... Args>
	void error(const Format& fmt, const Args&... args) {
		log(fmt, args...);
		throw axError_Undefined(AX_LOC);
	}

	void errorUnexpectedChar();
	void errorUnexpectedToken();

	bool _parseIdentifier();
	bool _parseNumber();
	bool _parseString();
	void _parseCommentBlock();
	void _parseCommentInline();

	void expectOp(axStrView s);

	void readShader();
	void readProperties();
	void readProperty();
	void readPass();
	void readBlendFunc(BlendFunc& f);

	template<class T>
	void readEnum(T& v) {
		if (!_token.isIdentifier()) {
			errorUnexpectedToken();
			return;
		}

		if (!_token.str.tryParse(v)) {
			AX_LOG("Error: parse enum {?}", _token.str);
			throw axError_Undefined(AX_LOC);
		}
		nextToken();
	}
	void readIdentifier(axIString& s);
	void readString(axIString& s);
	void readBool(bool& v);

	Token			_token;
	axString		_source;
	axString		_filename;
	axInt			_lineNumber = 1;
	axChar			_ch = 0;
	const axChar*	_cur = nullptr;
};