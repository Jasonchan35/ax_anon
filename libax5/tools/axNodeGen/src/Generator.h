#pragma once

#include "Parser.h"

namespace axNodeGen {

class Generator : public axNonCopyable {
public:
	Generator();
	void gen(axStrView filename);
	void gen_type(TypeInfo& type);

	Parser	_parser;
	TypeDB	_typeDB;

	axString	_outImpl;
};

} //namespace