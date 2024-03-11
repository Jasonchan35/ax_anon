#pragma once

class axFormat;

template<class First, class Second>
class axPair {
public:
	axPair() = default;
	axPair(const First& first_, const Second& second_)
		: first(first_)
		, second(second_)
	{}

	inline void onFormat(axFormat& f) const {
		//f << '[' << first << ":" << second << ']';
	}

	First first;
	Second second;
};

template<class First, class Second> inline
axPair<First, Second> axPair_make(const First& first, const Second& second) { return axPair<First, Second>(first, second); }
