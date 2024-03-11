#pragma once

#include <ax_core/base/ax_bitwise.h>
#include <ax_core/data_format/json/axJsonUtil.h>
#include <ax_core/data_format/axBinSerializer.h>
#include <ax_core/string/axStrLiteral.h>
#include <ax_core/string/axFormatParam.h>

template<class T> class axEnum;

template<class T>
class axEnumEntry {
public:
	axEnumEntry() = default;
	axEnumEntry(T value_, axStrLiteral name_) : value(value_), name(name_) {}
	void onFormat(axFormat& f) const { f << name << " = " << ax_enum_int(value); }

	T value;
	axStrLiteral name;
};

#define ax_SIMPLE_ENUM(T) \
	inline void axFormatParam_handler		(axFormat& f,     const T& v) { f << ax_enum_int(v); } \
	inline void axBinSerializer_handler		(axBinSerializer& se,   T& v) { se.io(ax_enum_int_ref(v)); } \
	inline void axBinDeserializer_handler	(axBinDeserializer& se, T& v) { se.io(ax_enum_int_ref(v)); } \
	inline axHash axHash_get		(const T & v) { return axHash_get(ax_enum_int(v)); } \
	inline void axDBInParamList_io	(axDBInParamList  & list, const T& v) { list.io(ax_enum_int_ref(v)); } \
	inline void axDBOutParamList_io	(axDBOutParamList & list,       T& v) { list.io(ax_enum_int_ref(v)); } \
	inline void axDBColumnList_io	(axDBColumnList& se, T& v, axStrView name) { se.named_io(name, ax_enum_int_ref(v)); } \
	AX_ENUM_OPERATORS(T) \
//----


#define ax_ENUM_ITEM_ENUM(       NAME, VALUE, ...)	NAME VALUE,
#define ax_ENUM_ITEM_ENTRY(      NAME, VALUE, ...)	Entry(Value::NAME, #NAME),
#define ax_ENUM_ITEM_NAME_CASE(  NAME, VALUE, ...)	case Value::NAME: return #NAME;

#define ax_ENUM_ITEM_CONST(      NAME, VALUE, ...)	inline static constexpr Value NAME	= Value::NAME;
#define ax_ENUM_ITEM_IS_VALUE(   NAME, VALUE, ...)	inline constexpr bool is_##NAME() const			{ return value == Value::NAME; }
#define ax_ENUM_ITEM_HAS_FLAG(   NAME, VALUE, ...)	inline constexpr bool hasFlag_##NAME() const	{ return hasFlags(Value::NAME); }
#define ax_ENUM_ITEM_SET_VALUE(  NAME, VALUE, ...)	inline constexpr void setTo_##NAME()			{ value = Value::NAME; }
#define ax_ENUM_ITEM_SET_FLAG(   NAME, VALUE, ...)	inline constexpr void setFlag_##NAME()			{ setFlags(Value::NAME); }
#define ax_ENUM_ITEM_UNSET_FLAG( NAME, VALUE, ...)	inline constexpr void unsetFlag_##NAME()		{ unsetFlags(Value::NAME); }
#define ax_ENUM_ITEM_TOGGLE_FLAG(NAME, VALUE, ...)	inline constexpr void toggleFlag_##NAME()		{ toggleFlags(Value::NAME); }

#define ax_ENUM_ITEM_LOWEST( NAME, VALUE, ...) if (Value::NAME < v) v = Value::NAME;
#define ax_ENUM_ITEM_HIGHEST(NAME, VALUE, ...) if (Value::NAME > v) v = Value::NAME;

#define ax_ENUM_CLASS(      ENUM_TYPE, INT_TYPE) ax_ENUM_CLASS_IMPL(ENUM_TYPE, INT_TYPE, false)
#define ax_ENUM_FLAGS_CLASS(ENUM_TYPE, INT_TYPE) ax_ENUM_CLASS_IMPL(ENUM_TYPE, INT_TYPE, true )

#define ax_ENUM_CLASS_IMPL(ENUM_TYPE, INT_TYPE, IS_FLAGS) \
	enum class ENUM_TYPE : INT_TYPE { \
		ENUM_TYPE##_EnumList(ax_ENUM_ITEM_ENUM) \
	}; \
	AX_ENUM_OPERATORS(ENUM_TYPE) \
	\
	class ENUM_TYPE##_ValueTraits { \
	public: \
		using Value = ENUM_TYPE; \
		using Entry = axEnumEntry<ENUM_TYPE>; \
		constexpr static bool kIsFlags = IS_FLAGS; \
		inline static axStrLiteral toName(ENUM_TYPE value) { \
			switch (value) { \
				ENUM_TYPE##_EnumList(ax_ENUM_ITEM_NAME_CASE) \
				default: return axStrLiteral(); \
			} \
		} \
		inline static axSpan<Entry> entries() { \
			static const Entry table[] = { ENUM_TYPE##_EnumList(ax_ENUM_ITEM_ENTRY) }; \
			return table; \
		} \
		static constexpr Value kLowest() { \
			auto v = static_cast<Value>(0); \
			ENUM_TYPE##_EnumList(ax_ENUM_ITEM_LOWEST) \
			return v; \
		} \
		static constexpr Value kHighest() { \
			auto v = static_cast<Value>(0); \
			ENUM_TYPE##_EnumList(ax_ENUM_ITEM_HIGHEST) \
			return v; \
		} \
	}; \
	\
	inline ENUM_TYPE##_ValueTraits ax_enum_traits(ENUM_TYPE v) { return ENUM_TYPE##_ValueTraits(); }; \
	inline void axFormatParam_handler		(axFormat& f, const ENUM_TYPE& value) { ax_enum(value).onFormat(f); } \
	inline bool axStrView_tryParse			(axStrView s,            ENUM_TYPE& value)	{ return ax_enum(value).tryParse(s); } \
	inline void axJsonWriter_handler		(axJsonWriter& wr, const ENUM_TYPE& value)	{ return ax_enum(value).onJsonWriter(wr); } \
	inline void axJsonReader_handler		(axJsonReader& rd,       ENUM_TYPE& value)	{ return ax_enum(value).onJsonReader(rd); } \
	inline void axJsonSerializer_handler	(  axJsonSerializer& se, ENUM_TYPE& value)	{ return ax_enum(value).onSerialize(se); } \
	inline void axJsonDeserializer_handler	(axJsonDeserializer& se, ENUM_TYPE& value)	{ return ax_enum(value).onSerialize(se); } \
	inline void axBinSerializer_handler		(   axBinSerializer& se, ENUM_TYPE& value)	{ return ax_enum(value).onSerialize(se); } \
	inline void axBinDeserializer_handler	( axBinDeserializer& se, ENUM_TYPE& value)	{ return ax_enum(value).onSerialize(se); } \
//---

template<class V> inline axEnum<V>&			ax_enum(      V& v) { return *reinterpret_cast<      axEnum<V>*>(&v); }
template<class V> inline const axEnum<V>&	ax_enum(const V& v) { return *reinterpret_cast<const axEnum<V>*>(&v); }

template<class V> inline axStrLiteral	ax_enum_name(const V& v) { return axEnum<V>(v).toName(); }

template<class T>
class axEnum {
	AX_RTTI_STRUCT(axEnum, axNoBase)
public:
	using IntType = typename axTypeTraits::underlying<T>::Type;
	using Value = T;
	using Entry = axEnumEntry<Value>;
	using EnumTraits = decltype(ax_enum_traits(T()));

	static constexpr axInt kBitCount = sizeof(T) * 8;
	static constexpr T kZero   = static_cast<T>(0);
	static constexpr T kOne    = static_cast<T>(1);
	static constexpr T kAllOne = static_cast<T>(~0);

	static constexpr This kLowest () { return EnumTraits::kLowest(); }
	static constexpr This kHighest() { return EnumTraits::kHighest(); }

			 constexpr axEnum(Value   v = kZero) : value(v) {}
	explicit constexpr axEnum(IntType v) : value(static_cast<T>(v)) {}

	constexpr operator T() const { return value; }
	constexpr void operator=(T v) { value = v; }

	AX_INLINE constexpr This operator+ (IntType v) const { return value +  v; }
	AX_INLINE constexpr This operator- (IntType v) const { return value -  v; }
	AX_INLINE constexpr This operator| (IntType v) const { return value |  v; }
	AX_INLINE constexpr This operator& (IntType v) const { return value &  v; }
	AX_INLINE constexpr This operator^ (IntType v) const { return value ^  v; }
	AX_INLINE constexpr This operator<<(axInt   v) const { return value << v; }
	AX_INLINE constexpr This operator>>(axInt   v) const { return value >> v; }

	AX_INLINE constexpr This operator+ (T v) const { return value +  v; }
	AX_INLINE constexpr This operator- (T v) const { return value -  v; }
	AX_INLINE constexpr This operator| (T v) const { return value |  v; }
	AX_INLINE constexpr This operator& (T v) const { return value &  v; }
	AX_INLINE constexpr This operator^ (T v) const { return value ^  v; }
	AX_INLINE constexpr This operator<<(T v) const { return value << v; }
	AX_INLINE constexpr This operator>>(T v) const { return value >> v; }

	AX_INLINE constexpr This operator~() const { return ~value; }

	AX_INLINE constexpr void operator +=(IntType v) { value  += v; }
	AX_INLINE constexpr void operator -=(IntType v) { value  -= v; }
	AX_INLINE constexpr void operator |=(IntType v) { value  |= v; }
	AX_INLINE constexpr void operator &=(IntType v) { value  &= v; }
	AX_INLINE constexpr void operator ^=(IntType v) { value  ^= v; }
	AX_INLINE constexpr void operator<<=(axInt   v) { value <<= v; }
	AX_INLINE constexpr void operator>>=(axInt   v) { value >>= v; }

	AX_INLINE constexpr void operator +=(T v) { value  += v; }
	AX_INLINE constexpr void operator -=(T v) { value  -= v; }
	AX_INLINE constexpr void operator |=(T v) { value  |= v; }
	AX_INLINE constexpr void operator &=(T v) { value  &= v; }
	AX_INLINE constexpr void operator ^=(T v) { value  ^= v; }
	AX_INLINE constexpr void operator<<=(T v) { value <<= v; }
	AX_INLINE constexpr void operator>>=(T v) { value >>= v; }

	AX_INLINE constexpr This operator++()	{ return ++value; }
	AX_INLINE constexpr This operator--()	{ return --value; }

	AX_INLINE constexpr This operator++(int)	{ return value++; }
	AX_INLINE constexpr This operator--(int)	{ return value--; }

	AX_INLINE constexpr bool operator==(T r) const { return value == r; }
	AX_INLINE constexpr bool operator!=(T r) const { return value != r; }

	AX_INLINE constexpr bool isZero		() const			{ return value == T(0); }
	AX_INLINE constexpr void setAllZero	()					{ value = T(0); }

	AX_INLINE constexpr void setFlags		(T bits, bool b)	{ ax_bitwise::set   (value, bits, b); }
	AX_INLINE constexpr void setFlags		(T bits)			{ ax_bitwise::set   (value, bits); }
	AX_INLINE constexpr void unsetFlags	(T bits)			{ ax_bitwise::unset (value, bits); }
	AX_INLINE constexpr void toggleFlags	(T bits)			{ ax_bitwise::toggle(value, bits); }

	AX_INLINE constexpr bool hasAnyFlag	(T bits) const		{ return ax_bitwise::hasAny	(value, bits); }
	AX_INLINE constexpr bool hasFlags		(T bits) const		{ return ax_bitwise::has	(value, bits); }

	AX_INLINE constexpr axInt count1Bits	() const			{ return ax_bitwise::count1 (value); }
	AX_INLINE constexpr axInt count0Bits	() const			{ return ax_bitwise::count0 (value); }
	AX_INLINE constexpr axInt highestBit	() const			{ return ax_bitwise::highest(value); }
	AX_INLINE constexpr axInt lowestBit	() const			{ return ax_bitwise::lowest (value); }

	AX_INLINE constexpr void parse			(axStrView s)		{ if (!tryParse(s)) throw axError_Undefined(AX_LOC); }
	AX_INLINE constexpr bool tryParse		(axStrView s)		{ return _tryParse(s); }
				   constexpr void onFormat		(axFormat& f) const { _onFormat(f); }

	static constexpr axSpan<Entry>	s_entries() { return EnumTraits::entries(); }
	static           axSpan<Entry>	s_flags	 () { static FlagsTable table; return table.entries(); }

	AX_INLINE constexpr void	setInt	(IntType i)	{ value = static_cast<T>(i); }

	AX_INLINE constexpr axStrLiteral	toName	() const { return EnumTraits::toName(value); }
	AX_INLINE constexpr       IntType	toInt	() const { return axTypeTraits::castUnderlying(value); }
	AX_INLINE constexpr const IntType&	asInt	() const { return *axTypeTraits::castUnderlyingPtr(&value); }
	AX_INLINE constexpr       IntType&	asInt	()       { return *axTypeTraits::castUnderlyingPtr(&value); }

	AX_INLINE void onJsonReader(axJsonReader&       rd) 		{ axTempString tmp; rd.readValue(tmp); parse(tmp); }
	AX_INLINE void onJsonWriter(axJsonWriter&       wr) const 	{ axTempString tmp; tmp.convert(*this); wr.writeValue(tmp); }
	AX_INLINE void onSerialize	(axJsonSerializer&   se) { onJsonWriter(se.writer); }
	AX_INLINE void onSerialize	(axJsonDeserializer& se) { onJsonReader(se.reader); }
	AX_INLINE void onSerialize	(axBinSerializer&    se) { se.io(asInt()); }
	AX_INLINE void onSerialize	(axBinDeserializer&  se) { se.io(asInt()); }

	void setToDefaultValue() { value = kZero; }

	T value;

private:
	class FlagsTable {
	public:
		FlagsTable() {
			for (axInt i = 0; i < kBitCount; i++) {
				auto v = static_cast<IntType>(1) << i;
				auto flag  = static_cast<T>(v);
				auto name = EnumTraits::toName(flag);
				if (!name) continue; // no such entry
				_entries.emplaceBack(flag, name);
			}
		}
		axSpan<Entry> entries() const { return _entries; }
	private:
		axArray<Entry, kBitCount> _entries;
	};

	AX_INLINE constexpr bool s_tryParse(Value& outValue, axStrView s) {
		for (const auto& e : s_entries()) {
			if (e.name == s) {
				outValue = e.value;
				return true;
			}
		}
		return false;
	}

	AX_INLINE constexpr bool _tryParse(axStrView s) {
		AX_IF_CONSTEXPR(EnumTraits::kIsFlags) {
			value = kZero;
			axArray<axStrView, kBitCount> tokens;
			s.getTokens(tokens, '|');

			T tmp;
			for (auto& tok : tokens) {
				tok = tok.trimCharsFrontAndBack(" \t");
				if (!s_tryParse(tmp, tok)) {
					AX_LOG("axEnumFlags::tryParse() unknown flag {?}", tok);
					return false;
				}
				value |= tmp;
			}
			return true;
		} else {
			return s_tryParse(value, s);
		}
	}

	constexpr void _onFormat(axFormat& f) const {
		AX_IF_CONSTEXPR(EnumTraits::kIsFlags) {
			axInt i = 0;
			for (auto& e : s_flags()) {
				if ((value & e.value) == kZero) continue;
				if (i > 0) f << " | ";
				f << e.name;
				i++;
			}
			if (i == 0) f << "None";
		} else {
			if (auto name = toName()) {
				f << name;
			} else {
				f << ax_enum_int(value);
			}
		}
	}
};

template<class T> inline void axJsonSerializer_handler  (axJsonSerializer&   se, axEnum<T> & value) { value.onSerialize(se); }
template<class T> inline void axJsonDeserializer_handler(axJsonDeserializer& se, axEnum<T> & value) { value.onSerialize(se); }

// template<class T> axEnum<T> axEnum_make(T v) { return axEnum<T>(v); }

//	template<class T>
//	class axEnumTraits_IgnoreAssign {
//	public:
//		constexpr axEnumTraits_IgnoreAssign(T v) : _value(v) {}
//		constexpr operator T() const { return _value; }
//
//		template<class R>
//		constexpr T operator=(const R&) { return _value; }
//	private:
//		T _value;
//	};
//
//	#define ax_ENUM_CLASS_VALUE(         NAME, VALUE, ...)	((IgnoreAssign)EnumType::NAME)
