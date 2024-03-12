#include "axMat.h"

template<class T>
void axMat<T>::setValues(const std::initializer_list< std::initializer_list<T> >& values) {
	axInt colCount = 0;
	axInt rowCount = static_cast<axInt>(values.size());

	for (const auto& it : values) {
		ax_max_it(colCount, static_cast<axInt>(it.size()));
	}

	create(colCount, rowCount);

	for (axInt r = 0 ; r < rowCount; r++) {
		auto* s = values.begin() + r;
		row(r).setValues(*s);
	}
}

template<class T>
void axMat<T>::create(axInt colCount, axInt rowCount) {
	clear();
	_colCount = colCount;
	_rowCount = rowCount;
	_data.resize(colCount * rowCount);
}

template<class T>
void axMat<T>::resize(axInt colCount, axInt rowCount) {
	if (_colCount == colCount && _rowCount == rowCount) return;

	axMat tmp;
	tmp.create(colCount, rowCount);

	axInt nc = ax_min(_colCount, colCount);
	axInt nr = ax_min(_rowCount, rowCount);

	for (axInt r = 0; r < nr; r++) {
		auto* src = row(r).data();
		auto* dst = tmp.row(r).data();
		auto* end = src + nc;

		for (; src < end; src++, dst++) {
			*dst = *src;
		}
	}

	_data = ax_move(tmp._data);
}

template<class T>
bool axMat<T>::operator==(const axMat& s) const {
	if (_colCount != s._colCount) return false;
	if (_rowCount != s._rowCount) return false;

	for (axInt r = 0; r < _rowCount; r++) {
		if (row(r) != s.row(r)) return false;
	}

	return true;
}

template<class T>
void axMat<T>::onFormat(axFormat& f) const {
	for (axInt r = 0; r < _rowCount; r++) {
		f << "\n  " << row(r);
	}
}

template<class T>
void axMat<T>::mul(const axMat& a, const axMat& b) {
	if (a._colCount != b._rowCount) throw axError_Undefined(AX_LOC);

	auto ocol = ax_min(a._colCount, b._colCount);
	auto orow = ax_min(a._rowCount, b._rowCount);
	create(ocol, orow);

	auto n = a._colCount;

	for (axInt r = 0; r < orow; r++) {
		for (axInt c = 0; c < ocol; c++) {
			T v = 0;
			for (axInt i = 0; i < n; i++) {
				auto av = a.at(i, r);
				auto bv = b.at(c, i);
				v += av * bv;
			}
			at(c, r) = v;
		}
	}
}

template class axMat<axFloat32>;
template class axMat<axFloat64>;