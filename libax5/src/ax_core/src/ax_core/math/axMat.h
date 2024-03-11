#pragma once

#include "../container/axArray.h"

template<class T>
class axMat {
public:
	using Element = T;

	axMat() = default;
	axMat(const std::initializer_list< std::initializer_list<T> >& values);

	void create(axInt colCount, axInt rowCount);

	void resize (axInt colCount, axInt rowCount);
	void reserve(axInt colCount, axInt rowCount) { _data.reserve(colCount * rowCount); }

	void clear()		{ _data.clear(); }

	axInt colCount() const	{ return _colCount; }
	axInt rowCount() const	{ return _rowCount; }

			T* data()			{ return _data.data(); }
	const	T* data() const		{ return _data.data(); }

	axMutSpan<T>	row(axInt i)		{ return axMutSpan<T>(&_data[i * _colCount], _colCount); }
	   axSpan<T>	row(axInt i) const	{ return    axSpan<T>(&_data[i * _colCount], _colCount); }

	      T& at(axInt colIndex, axInt rowIndex)			{ return row(rowIndex)[colIndex]; }
	const T& at(axInt colIndex, axInt rowIndex) const	{ return row(rowIndex)[colIndex]; }

	void fillValues(const T& value) { _data.fillValues(value); }

	void setValues(const std::initializer_list< std::initializer_list<T> >& values);

	void mul(const axMat& a, const axMat& b);

	void onFormat(axFormat& f) const;

	bool operator==(const axMat& s) const;

private:
	axInt _colCount = 0;
	axInt _rowCount = 0;

	axArray<T>	_data;
};

template<class T> inline
axMat<T>::axMat(const std::initializer_list< std::initializer_list<T> >& values) {
	setValues(values);
}


