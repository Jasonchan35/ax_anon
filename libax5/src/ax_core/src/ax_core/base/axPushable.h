#pragma once

template<class T>
class axScopedPush {
	using This = axScopedPush<T>;
public:
	explicit axScopedPush(T& v)
		: _v(&v)
		, _oldValue(v)
	{}

	explicit axScopedPush(T& v, const T& newValue)
		: _v(&v)
		, _oldValue(v)
	{
		v = newValue;
	}

	axScopedPush(This && r) {
		_v = r._v;
		_oldValue = r._oldValue;
		r._v = nullptr;
	}

	~axScopedPush() {
		if (!_v) return;
		*_v = _oldValue;
	}

	T& value()			{ return *_v; }

	T* ptr()			{ return _v; }
	T* operator->()		{ return _v; }

	template<class R> void operator =  (const R& r) { *_v =  r; }

	template<class R> void operator += (const R& r) { *_v += r; }
	template<class R> void operator -= (const R& r) { *_v -= r; }
	template<class R> void operator *= (const R& r) { *_v *= r; }
	template<class R> void operator /= (const R& r) { *_v /= r; }

	template<class R> void operator |= (const R& r) { *_v |= r; }
	template<class R> void operator &= (const R& r) { *_v &= r; }
	template<class R> void operator ^= (const R& r) { *_v /= r; }

private:
	T* _v = nullptr;
	T _oldValue;
};

template<class T> AX_NODISCARD axScopedPush<T> ax_push(T& v) { return axScopedPush<T>(v); }
template<class T> AX_NODISCARD axScopedPush<T> ax_push(T& v, const T& newValue) { return axScopedPush<T>(v, newValue); }

template<class T>
class axPushable {
	using This = axPushable;
public:
	template<class... ARGS>
	axPushable(const ARGS&... args) : _data(ax_forward(args)...) {}

	axScopedPush<T> push() {
		return axScopedPush<T>(_data);
	}

	axScopedPush<T> push(const T& value) {
		return axScopedPush<T>(_data, value);
	}

	const T& get() const & { return _data; }
	operator const T& () const & { return _data; }
	const T* operator->() const & { return &_data; }

	template<class R> auto operator+ (const R& r) const { return _data * r; }
	template<class R> auto operator- (const R& r) const { return _data * r; }
	template<class R> auto operator* (const R& r) const { return _data * r; }
	template<class R> auto operator/ (const R& r) const { return _data * r; }
	template<class R> auto operator| (const R& r) const { return _data * r; }
	template<class R> auto operator& (const R& r) const { return _data * r; }

	template<class R> bool operator> (const R& r) const { return _data >  r; }
	template<class R> bool operator< (const R& r) const { return _data <  r; }
	template<class R> bool operator>=(const R& r) const { return _data >= r; }
	template<class R> bool operator<=(const R& r) const { return _data <= r; }

	template<class R> bool operator==(const R& r) const { return _data == r; }
	template<class R> bool operator!=(const R& r) const { return _data != r; }

	T& edit() & { return _data; }

private:
	T	_data;
};