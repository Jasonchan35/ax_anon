#pragma once

namespace axNodeGen {

inline
void convertToDisplayName(axIString& outStr, axStrView s) {
	outStr.clear();
	outStr.reserve(s.size());

	axChar last = 0;
	for (auto ch : s) {
		if (axCStr::isLower(last) && axCStr::isUpper(ch)) {
			outStr.append(' ');
		}
		if (ch == '_')
			ch = ' ';
		outStr.append(ch);

		last = ch;
	}

	if (outStr) {
		outStr[0] = axCStr::toUpper(outStr[0]);
	}
}

class Attribute {
public:
	axString	name;
	axString	defaultValue;
};

enum class PropType {
	Unknown,
	Normal,
	DiaInput,
	DiaOutput,
};

class PropInfo : public axNonCopyable {
public:
	PropType propType = PropType::Unknown;
	axString name;
	axString displayName;
	axString varName;
	axString typeName;
	axString defaultValue;

	struct HasAttr {
		bool dontSave = false;
	};
	HasAttr		hasAttr;
	axArray<Attribute>	attributes;
};

class TypeInfo : public axNonCopyable {
public:
	axString name;
	axString baseName;
	axString fullname;
	axString openNamespaceScope;
	axString closeNamespaceScope;
	axDict<axString, PropInfo>	props;
};

class TypeDB : public axNonCopyable {
public:
	TypeDB();
	~TypeDB();

	axDict<axString, TypeInfo>	types;
};


} //namespace axNodeGen