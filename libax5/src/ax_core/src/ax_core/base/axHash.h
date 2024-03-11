#pragma once

class axHash {
public:
	using Value = axUInt64;
	Value value;

	axHash() = default;
	explicit axHash(Value v) : value(v) {};

	bool operator==(const axHash& rhs) const { return value == rhs.value; }
	bool operator!=(const axHash& rhs) const { return value != rhs.value; }
	bool operator< (const axHash& rhs) const { return value <  rhs.value; }
	bool operator> (const axHash& rhs) const { return value >  rhs.value; }
	bool operator<=(const axHash& rhs) const { return value <= rhs.value; }
	bool operator>=(const axHash& rhs) const { return value >= rhs.value; }

//	void onFormat(axFormat& f) const; // implement in axFormat.h
};

AX_STATIC_ASSERT(axTypeTraits::isPod<axHash>::value);

#define AX_MACRO_OP(T) \
	AX_INLINE axHash axHash_get(T v) { return axHash(static_cast<axHash::Value>(v)); } \
//----
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_CHAR(AX_MACRO_OP)
#undef AX_MACRO_OP

AX_INLINE axHash axHash_get(const axType* p)	{ return axHash_get(axPtrUtil::toUInt(p)); }
AX_INLINE axHash axHash_get(void* const   p)	{ return axHash_get(axPtrUtil::toUInt(p)); }
AX_INLINE axHash axHash_get(bool b)			{ return axHash(b ? 0U : 1U); }

template< class T > AX_INLINE
axHash axHash_get( const T & v ) {
	return v.onGetHash();
}

template<class T>
class axHashData {
public:
	using Data = T;

	template<class... ARGS>
	axHashData(ARGS&&... args) : _data(ax_forward(args)...) {}

	const T&	data		() const & { return  _data; }
	const T&	operator*	() const & { return *_data; }
	const T*	operator->	() const & { return  _data; }

	bool equals(const axHash& h, const T& d) const {
		if (&_data == d) return true;
		if (hash() != h) return false;
		return _data == d;
	}

	bool operator==(const axHashData& r) const {
		if (this == &r) return true;
		if (hash() != r.hash()) return false;
		return _data == r._data;
	}

	bool operator!=(const axHashData& r) const {
		return !operator==(r);
	}

	axHash hash() const {
		if (_hashDirty) {
			_hash = axHash_get(_data);
			_hashDirty = false;
		}
		return _hash;
	}

	void onFormat(axFormat& f) const; // implement in axFormat.h

	Data& edit() & {
		_hashDirty = true;
		return _data;
	}

private:
	Data _data;

	mutable axHash	_hash {0};
	mutable bool	_hashDirty = true;
};
