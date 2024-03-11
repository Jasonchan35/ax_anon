#pragma once

#include <random>

class axRandom {
public:
	void seed(axInt s) { _gen.seed(s); }

	axUInt64 getUInt64() { return _gen(); }
	
	// exclude max
	template<class T>
	T getInt(T max) {
		auto r = getUInt64();
		auto o = r % max;
		return static_cast<T>(o);
	}

	axInt8 		range(axInt8    min, axInt8    max) { return intRange(min, max); }
	axInt16		range(axInt16   min, axInt16   max) { return intRange(min, max); }
	axInt32		range(axInt32   min, axInt32   max) { return intRange(min, max); }
	axInt64		range(axInt64   min, axInt64   max) { return intRange(min, max); }

	axUInt8 	range(axUInt8   min, axUInt8   max) { return intRange(min, max); }
	axUInt16	range(axUInt16  min, axUInt16  max) { return intRange(min, max); }
	axUInt32	range(axUInt32  min, axUInt32  max) { return intRange(min, max); }
	axUInt64	range(axUInt64  min, axUInt64  max) { return intRange(min, max); }

	axFloat32 	range(axFloat32 min, axFloat32 max) { return floatRange(min, max); }
	axFloat64 	range(axFloat64 min, axFloat64 max) { return floatRange(min, max); }

	template<class T>
	T intRange(T min, T max) {
		auto r = getUInt64();
		return static_cast<T>(r % (max - min + 1)) + min;
	}

	template<class T>
	T floatRange(T min, T max) {
		std::uniform_real_distribution<T> dis(min, max);
		return dis(_gen);
	}

	template<class T> axVec2<T> onUnitCircle()		{ return Helper<T>::onUnitCircle(*this); }
	template<class T> axVec3<T> onUnitSphere()		{ return Helper<T>::onUnitSphere(*this); }

	template<class T> axVec2<T> insideUnitCircle()	{ return Helper<T>::insideUnitCircle(*this); }
	template<class T> axVec3<T> insideUnitSphere()	{ return Helper<T>::insideUnitSphere(*this); }

private:
	template<class T>
	class Helper {
	public:
		axVec2<T> onUnitCircle		(axRandom& rand);
		axVec3<T> onUnitSphere		(axRandom& rand);

		axVec2<T> insideUnitCircle	(axRandom& rand);
		axVec3<T> insideUnitSphere	(axRandom& rand);
	};

	std::random_device	_randDev;
	std::mt19937_64		_gen {_randDev()};
};
