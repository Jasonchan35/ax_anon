#pragma once

#include "axJsonWriter.h"
#include "axJsonDeserializer.h"

class axJsonSerializer : public axNonCopyable {
public:
	axJsonSerializer(axIString& outJson) 
		: writer(outJson) 
	{
	}

	void io(bool&      value) { writer.writeValue(value); }

	void io(axInt8&    value) { writer.writeValue(value); }
	void io(axInt16&   value) { writer.writeValue(value); }
	void io(axInt32&   value) { writer.writeValue(value); }
	void io(axInt64&   value) { writer.writeValue(value); }

	void io(axUInt8&   value) { writer.writeValue(value); }
	void io(axUInt16&  value) { writer.writeValue(value); }
	void io(axUInt32&  value) { writer.writeValue(value); }
	void io(axUInt64&  value) { writer.writeValue(value); }

	void io(axFloat32& value) { writer.writeValue(value); }
	void io(axFloat64& value) { writer.writeValue(value); }

	void io(axIString8& value) { writer.writeValue(value); }

	template<class T>
	void io(axMutSpan<T> value) {
		auto array_scope = writer.array();
		for (auto& e : value) {
			io(e);
		}
	}

	template<class T>
	void io(T& value) { axJsonSerializer_handler(*this, value); }

	template<class T>
	void named_io(axStrView name, T& value) {
		writer.writeMemberName(name);
		io(value);
	}

	template<class T> void named_io_fixed(axStrView name, T& value) { named_io(name, value); }
	template<class T> void named_io_pkey(axStrView name, T& value, bool autoPkeypkeyAutoInc) { named_io(name, value); }

	axJsonWriter writer;
};

template<class T> inline
void ax_dump_json_impl(const axSourceLoc& loc, T& value) {
	axTempString8 json;
	axJsonSerializer se(json);
	se.io(value);
	AX_LOG("ax_dump_json: {?}", json);
}

#define ax_dump_json(value) ax_dump_json_impl(AX_LOC, value);

//------------------

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, T& value) {
	auto object_scope = se.writer.object();
	value.onSerialize(se);
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axIArray<T> & value) {
	se.io(value.span());
}

template<class T, axInt N> inline
void axJsonSerializer_handler(axJsonSerializer& se, axArray<T, N> & value) {
	se.io(value.span());
}

template<class T, axInt N> inline
void axJsonSerializer_handler(axJsonSerializer& se, axFixedArray<T,N> & value) {
	se.io(value.span());
}

template<class T> inline
void axJsonSerializer_handler(axJsonSerializer& se, axIStringT<T>& value) {
	se.writer.writeValue(value);
}

template<class T, axInt N> inline
void axJsonSerializer_handler(axJsonSerializer& se, axStringT<T, N>& value) {
	se.writer.writeValue(value);
}

template<class CLOCK> inline
void axJsonSerializer_handler(axJsonSerializer& se, axTimestampT<CLOCK>& value) {
	se.writer.writeValue(value.toInt());
}
