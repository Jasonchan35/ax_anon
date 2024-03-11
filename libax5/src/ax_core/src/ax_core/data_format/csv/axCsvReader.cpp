#include "axCsvReader.h"

axCsvReader::axCsvReader(axStrView data, axStrView filenameForErrorMessage) 
	: _data(data)
	, _filename(filenameForErrorMessage)
	, _cur(data.data())
	, _end(data.end())
{
}

bool axCsvReader::nextRow() {
	_row.cells.clear();
	if (!_cur || _cur >= _end) return false;

	for (;;) {
		auto& cell = _row.cells.emplaceBack();
		if (!_readCell(cell))
			break;
	}

	return true;
}

bool axCsvReader::_readCell(Cell& cell) {
	if (_cur >= _end) return false;

	for ( ; _cur < _end ; _cur++) {
		auto c = *_cur;
		if (c == '\"') {
			_cur++;
			_readString(cell);
			continue;
		}

		if (c == ',') {
			_cur++;
			return true;
		}

		if (c == '\r') {
			_cur++;
			if (*_cur == '\n') _cur++;
			return false;
		}

		if (c == '\n') {
			_cur++;
			return false;
		}

		cell.append(c);
	}

	return false;
}

void axCsvReader::_readString(Cell& cell) {
	for (;; _cur++) {
		if (_cur >= _end)
			throw axError_Undefined(AX_LOC); // End of file within string

		auto c = *_cur;

		if (c == '\"') {
			return;
		}

		cell.append(c);
	}
}
