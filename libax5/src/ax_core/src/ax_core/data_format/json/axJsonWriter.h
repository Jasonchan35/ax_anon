#pragma once

#include <ax_core/string/axString.h>

class axJsonWriterError : public axError {};

class axJsonWriterError_EndObject : public axJsonWriterError {};
class axJsonWriterError_EndArray : public axJsonWriterError {};
class axJsonWriterError_MemberOutsideOfObject : public axJsonWriterError {};
class axJsonWriterError_MemberNameRequired : public axJsonWriterError {};

class axJsonWriter;

template<class T> void axJsonWriter_handler(axJsonWriter& wr, const T& value);

class axJsonWriter {
public:
	axJsonWriter(axIString & outJson);
	~axJsonWriter();

	class ObjectScope : public axNonCopyable {
	public:
		ObjectScope(axJsonWriter* p);
		ObjectScope(ObjectScope && rhs) noexcept;
		~ObjectScope();

	private:
		axJsonWriter* _p;
	};

	class ArrayScope : public axNonCopyable {
	public:
		ArrayScope(axJsonWriter* p);
		ArrayScope(ArrayScope && rhs) noexcept;
		~ArrayScope();
	private:
		axJsonWriter* _p;
	};

	AX_NODISCARD ObjectScope object(axStrView name)		{ beginObject(name); return ObjectScope(this); }
	AX_NODISCARD ObjectScope object()					{ beginObject();	 return ObjectScope(this); }

	ArrayScope	array(axStrView name)		{ beginArray(name); return ArrayScope(this); }
	ArrayScope	array()						{ beginArray();		return ArrayScope(this); }

	void beginObject(axStrView name);
	void beginObject();
	void endObject();

	void beginArray(axStrView name);
	void beginArray();
	void endArray();

	void writeMemberName(axStrView name);

	template<class VALUE>
	void writeMember(axStrView name, const VALUE& value) {
		writeMemberName(name);
		writeValue(value);
	}

	void writeNull();

	void writeValue(axInt8		value);
	void writeValue(axInt16		value);
	void writeValue(axInt32		value);
	void writeValue(axInt64		value);

	void writeValue(axUInt8		value);
	void writeValue(axUInt16	value);
	void writeValue(axUInt32	value);
	void writeValue(axUInt64	value);

	void writeValue(axFloat32	value);
	void writeValue(axFloat64	value);
	void writeValue(axFloat128	value);

	void writeValue(bool		value);
	void writeValue(axStrView	value);

	template<axInt N>
	void writeValue(const axChar (&sz)[N])	{ writeValue(axStrView(sz)); }

	template<class R>
	void writeValue(const axIStringT<R>& value) { writeValue(value.view()); }

	template<class R, axInt N>
	void writeValue(const axStringT<R, N>& value) { writeValue(value.view()); }

	template<class R>
	void writeValue(const R& v) {
		axJsonWriter_handler(*this, v);
	}

	template<class T>
	void reflection(const T& obj) {
		using R = axReflection<T>;
		ReflectionForEachHandler<T> h {*this, obj};
		axReflection<R>::Fields::s_forEachType(h);
	}

	axIString& json() { return *_json; }

	void preWriteValue();
	void newline(axInt offset = 0);
private:
	template<class T>
	struct ReflectionForEachHandler {
		using R = axReflection<T>;

		axJsonWriter& wr;
		const T& obj;

		template<axInt index, class Field> AX_INLINE
		void handle() {
			auto& v = Field::s_value(&obj);
			wr.writeMember(Field::s_name(), v);
		}
	};


	void quoteString(axStrView v);

	enum class LevelType {
		Object,
		Array,
		Member,
	};

	axIString*	_json;
	axArray<LevelType, 16> _level;

	bool _commaNeeded	{false};
	bool _newlineNeeded {true};
};

template<class T> inline
void axJsonWriter_handler(axJsonWriter& wr, const T& value) {
	auto obj = wr.object();
	value.onJsonWriter(wr);
}

template<> inline
void axJsonWriter_handler(axJsonWriter& wr, const axStrView8& value) {
	wr.writeValue(value);
}

template<class T> inline
void axJsonWriter_handler(axJsonWriter& wr, const axStrViewT<T>& value) {
	axTempString8 tmp;
	tmp.setUtf(value);
	wr.writeValue(tmp.view());
}

template<class T> inline
void axJsonWriter_handler(axJsonWriter& wr, const axStrLiteralT<T>& value) {
	wr.writeValue(value.asStrView());
}

template<class T> 
void axJsonWriter_handler(axJsonWriter& wr, const axSpan<T>& value) {
	auto arr = wr.array();
	for (auto& e : value) {
		wr.writeValue(e);
	}
}

template<class T, axDeleteFunc<T> del, class LIST_NODE>
void axJsonWriter_handler(axJsonWriter& wr, const axDList<T, del, LIST_NODE>& value) {
	auto arr = wr.array();
	for (auto& e : value) {
		wr.writeValue(e);
	}
}

template<class T> inline
void axJsonWriter_VoidPtrWrapper(axJsonWriter& wr, const void* data) {
	auto* p = reinterpret_cast<const T*>(data);
	wr.writeValue(*p);
}
