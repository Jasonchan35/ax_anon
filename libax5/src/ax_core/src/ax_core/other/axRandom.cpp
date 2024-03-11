#include "axRandom.h"

template<class T>
axVec2<T> axRandom::Helper<T>::onUnitCircle(axRandom& rand) {
	auto theta = rand.floatRange<T>(0, 1) * 2 * ax_pi_<T>();
	axVec2<T> v;
	ax_sincos(theta, v.x, v.y);
	return v;
}

template<class T>
axVec2<T> axRandom::Helper<T>::insideUnitCircle(axRandom& rand) {
	auto v = onUnitCircle(rand);
	auto r = rand.floatRange<T>(0, 1);
	return v * ax_sqrt(r); // distribution in squre root along radius
}

template<class T>
axVec3<T> axRandom::Helper<T>::onUnitSphere(axRandom& rand) {
	auto longitude = rand.floatRange<T>(0, 1) * 2 * ax_pi_<T>();
	auto latitude  = ax_acos(rand.floatRange<T>(0, 1) * 2 - 1);

	T sinLongitude, cosLongitude;
	T sinLatitude,  cosLatitude;

	ax_sincos(longitude, sinLongitude, cosLongitude);
	ax_sincos(latitude,  sinLatitude,  cosLatitude);

	axVec3<T> v(sinLatitude * cosLongitude,
				sinLatitude * sinLongitude,
				cosLatitude);
	return v;
}

template<class T>
axVec3<T> axRandom::Helper<T>::insideUnitSphere(axRandom& rand) {
	auto v = onUnitSphere(rand);
	auto r = rand.floatRange<T>(0, 1);
	return v * ax_cbrt(r);
}

template class axRandom::Helper<axFloat32>;
template class axRandom::Helper<axFloat64>;
