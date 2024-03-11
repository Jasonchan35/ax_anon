#pragma once

#include "axJsonReader.h"
#include <ax_core/file/axFile.h>

class axJsonDeserializer : public axNonCopyable {
public:
	axJsonDeserializer(axStrView8 json, axStrView filenameForErrorMessage) 
		: reader(json, filenameForErrorMessage) 
	{
	}

	void io(bool&	   value) { reader.readValue(value); }

	void io(axInt8&    value) { reader.readValue(value); }
	void io(axInt16&   value) { reader.readValue(value); }
	void io(axInt32&   value) { reader.readValue(value); }
	void io(axInt64&   value) { reader.readValue(value); }

	void io(axUInt8&   value) { reader.readValue(value); }
	void io(axUInt16&  value) { reader.readValue(value); }
	void io(axUInt32&  value) { reader.readValue(value); }
	void io(axUInt64&  value) { reader.readValue(value); }

	void io(axFloat32& value) { reader.readValue(value); }
	void io(axFloat64& value) { reader.readValue(value); }

	void io_fixed(axInt8&    value) { reader.readValue(value); }
	void io_fixed(axInt16&   value) { reader.readValue(value); }
	void io_fixed(axInt32&   value) { reader.readValue(value); }
	void io_fixed(axInt64&   value) { reader.readValue(value); }

	void io_fixed(axUInt8&   value) { reader.readValue(value); }
	void io_fixed(axUInt16&  value) { reader.readValue(value); }
	void io_fixed(axUInt32&  value) { reader.readValue(value); }
	void io_fixed(axUInt64&  value) { reader.readValue(value); }

	void io_fixed(axFloat32&  value) { reader.readValue(value); }
	void io_fixed(axFloat64&  value) { reader.readValue(value); }

	void io(axIString8&  value) { reader.readValue(value); }

	template<class T>
	void io(T& value) { axJsonDeserializer_handler(*this, value); }

	template<class T>
	void named_io(axStrView name, T& value) {
		if (reader.isMember(name)) {
			io(value);
		}
	}

	template<class T> void named_io_fixed(axStrView name, T& value) { named_io(name, value); }
	template<class T> void named_io_pkey(axStrView name, T& value, bool autoPkeypkeyAutoInc) { named_io(name, value); }

	axJsonReader reader;
};

//-------------------

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, T& value) {
	se.reader.beginObject();
	while (!se.reader.endObject()) {
		value.onSerialize(se);
	}
}

template<class T, axInt N> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axStringT<T, N>& value) { se.io(value.asIString()); }

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axIArray<T>& value) {
	value.clear();
	se.reader.beginArray();
	while (!se.reader.endArray()) {
		auto& elem = value.emplaceBack();
		se.io(elem);
	}
}

template<class T, axInt N> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axArray<T, N>& value) { se.io(value.asIArray()); }

template<class T> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axMutSpan<T> value) {
	se.reader.beginArray();
	axInt i = 0;
	while (!se.reader.endArray()) {
		if (i >= value.size()) throw axError_Undefined(AX_LOC);
		se.io(value[i]);
		i++;
	}
	if (i != value.size()) throw axError_Undefined(AX_LOC);
}

template<class T, axInt N> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axFixedArray<T,N> & value) {
	se.reader.beginArray();
	auto* p = value.data();
	auto* end = p + value.size();
	while (!se.reader.endArray()) {
		if (p >= end) throw axError_Undefined(AX_LOC);
		se.io(*p);
		p++;
	}
}

template<class CLOCK> inline
void axJsonDeserializer_handler(axJsonDeserializer& se, axTimestampT<CLOCK>& value) {
	axInt64 tmp;
	se.io(tmp);
	value.setInt(tmp);
}
