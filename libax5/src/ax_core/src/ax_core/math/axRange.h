#pragma once

template<class T>
class axRange_ {
	using This = axRange_;
public:
	T	start, size;

	axRange_() = default;
	axRange_(const T& start_, const T& size_) { set(start_, size_); }

	T	 end() const { return start + size; }

	void set(const T& newStart, const T& newSize) {
		start = newStart;
		size   = newSize;
	}

	void setStartKeepEnd(const T& newStart) { auto e = end(); start = newStart; setEnd(e); };
	void setEnd(const T& newEnd) { size = newEnd - start; };

	AX_INLINE This operator| (const This& r) const	{ return _unionWith(r); }
	AX_INLINE This operator& (const This& r) const	{ return _intersects(r); }

	AX_INLINE void operator|=(const This& r)		{ *this = *this | r; }
	AX_INLINE void operator&=(const This& r)		{ *this = *this & r; }

	AX_NODISCARD axRange_	operator* (const T& m)	{ return axRange_(start * m, size * m); }
	AX_NODISCARD axRange_	operator/ (const T& m)	{ return axRange_(start / m, size / m); }

	void onFormat(axFormat& f) const; // implement in axFormat.h

private:
	This _unionWith(const This& r) const {
		auto newStart = ax_min(start, r.start);
		auto newEnd    = ax_max(end(),  r.end());
		if (newEnd <= newStart)
			return This(0, 0);
		return This(newStart, newEnd - newStart);
	}

	This _intersects(const This& r) const {
		auto newStart = ax_max(start, r.start);
		auto newEnd    = ax_min(end(),  r.end());
		if (newEnd <= newStart)
			return This(0, 0);
		return This(newStart, newEnd - newStart);
	}
};

using axIntRange = axRange_<axInt>;
