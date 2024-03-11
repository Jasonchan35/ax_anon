#pragma once

#include "axPersistString.h"
#include "../data_format/json/axJsonDeserializer.h"

template<class CH, class ID>
class axNameIdT { // copyable
public:
	using This		= axNameIdT;
	using Name		= axPersistStringT<CH>;
	using StrView	= axStrViewT<CH>;
	using Id		= ID;

	axNameIdT() = default;
	axNameIdT(const Name& name, const Id& id) : _name(name), _id(id) {}

	Name	name() const	{ return _name; }
	Id		id() const		{ return _id;   }

	static This	s_make(StrView str);

	bool operator==(const This& r) const { return _name == r._name && _id == r._id; }
	bool operator!=(const This& r) const { return _name != r._name || _id != r._id; }

	void onFormat(axFormat& f) const {
		f << _name;
		if (_id >= 0) f << _id;
	}

	void getString(axIStringT<CH>& outStr) const {
		outStr.convert(*this);
	}

	void appendToString(axIStringT<CH>& outStr) const {
		outStr.appendConvert(*this);
	}

	axTempString	toString() const { return axTempString::s_convert(*this); }

	bool hasNoId() const { return _id == kNoId; }

	void setId(const ID& v) { _id = v; }

	const static Id kNoId = -1;

private:
	Name	_name;
	Id		_id = kNoId;
};

template<class CH, class ID> inline
void axJsonWriter_handler(axJsonWriter& wr, const axNameIdT<CH, ID>& value) {
	wr.writeValue(axTempString::s_convert(value));
}

template<class CH, class ID> inline
void axJsonReader_handler(axJsonReader& rd, axNameIdT<CH, ID>& value) {
	axTempString tmp;
	rd.readValue(tmp);
	value = axNameIdT<CH, ID>::s_make(tmp);
}

template<class CH, class ID>
axNameIdT<CH, ID> axNameIdT<CH, ID>::s_make(StrView str) {
	if (!str) return This();

	auto* s = str.begin();
	auto* e = str.end();
	auto* p = e - 1;
	for (; p >= s; p--) {
		if (!axCStr::isDigit(*p)) {
			p++;
			break;
		}
	}
		
	ID outId = kNoId;
	if (p != e) {
		auto len = e - p;
		if (!axStrView(p, len).tryParse(outId)) {
			throw axError_Undefined(AX_LOC);
		}
	}

	auto len = p - s;
	axStrView namePart(s, len);
	auto outName = Name::s_make(namePart);
		
	return This(outName, outId);
}

using axNameId = axNameIdT<axChar, axInt>;
