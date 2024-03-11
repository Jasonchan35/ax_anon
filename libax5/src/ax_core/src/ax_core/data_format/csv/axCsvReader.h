#pragma once

#include "../../string/axString.h"

class axCsvReader {
public:
	using Cell = axString;

	class Row {
	public:
		axArray<Cell, 64>	cells;

		void onFormat(axFormat& f) const {
			f << cells;
		}
	};

	axCsvReader(axStrView data, axStrView filenameForErrorMessage);

	bool nextRow();

	axStrView	cell(axInt index) { return _row.cells.inBound(index) ? _row.cells[index] : axStrView(); }
	const Row&	row() const { return _row; }

private:

	bool		_readCell(Cell& cell);
	void		_readString(Cell& str);

	axStrView		_data;
	axStrView		_filename;
	const axChar*	_cur = nullptr;
	const axChar*	_end = nullptr;
	Row				_row;
};