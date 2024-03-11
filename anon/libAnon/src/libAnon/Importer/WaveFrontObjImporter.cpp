#include "WaveFrontObjImporter.h"

namespace Anon {

void WaveFrontObjImporter::loadFile(axEditableMesh& outMesh, axStrView filename) {
	axMemMap mm;
	mm.openFile(filename);
	loadMem(outMesh, mm);
}

void WaveFrontObjImporter::loadMem(axEditableMesh& outMesh, axByteSpan data) {
	_outMesh = &outMesh;
	_outMesh->clear();

	const axInt reserveSize = 512;
	_tmpUv.reserve(reserveSize);
	_tmpNormal.reserve(reserveSize);

	_source = axStrView_make(data);
	_remainLines = _source;
	_lineNumber = 0;

	for(;;) {
		_readLine();
		if (_currentLine) {
			_remainTokens = _currentLine;
			_readToken();
			_parseLine();
		}
		if (!_remainLines) break;
	}
}

void WaveFrontObjImporter::_parseLine() {
	if (!_token) return;
	if (_token.startsWith("#")) return; //skip comment

	if (_token == "v") {
		axVec4d value(0,0,0,1);

		for (axInt i=0; i<4; i++) {
			_readToken();
			if (!_token) break;
			if (!_token.tryParse(value.data()[i])) {
				_error("error read v");
			}
		}

		auto v = value.homogenize() * option.scale;
		_outMesh->addPoint(v);
		return;
	}

	if (_token == "vt") {
		axVec2d value(0,0);

		for (axInt i=0; i<2; i++) {
			_readToken();
			if (!_token) break;
			if (!_token.tryParse(value.data()[i])) {
				_error("error read vt");
			}
		}
		_tmpUv.append(value);
		return;
	}


	if (_token == "vn") {
		axVec3d value(0,0,0);

		for (axInt i=0; i<3; i++) {
			_readToken();
			if (!_token) break;
			if (!_token.tryParse(value.data()[i])) {
				_error("error read vn");
			}
		}
		_tmpNormal.append(value);
		return;
	}

	if (_token == "f") {
		axArray<axInt,64>	face_vi;
		axArray<axInt,64>	face_vt;
		axArray<axInt,64>	face_vn;

		for (;;) {
			_readToken();
			if (!_token) break;

			axInt vi = 0, vt = 0, vn = 0;

			auto pair = _token.splitByChar('/');
			if (!pair.first.tryParse(vi)) {
				_error("error read f vi");
			}

			//vt
			pair = pair.second.splitByChar('/');
			if (pair.first) {
				if (!pair.first.tryParse(vt)) {
					_error("error read f vt");
				}
			}
			//vn
			pair = pair.second.splitByChar('/');
			if (pair.first) {
				if (!pair.first.tryParse(vn)) {
					_error("error read f vn");
				}
			}

			// index start from 1 and Negative values indicate relative vertex numbers
			if (vi > 0) { vi--; } else if (vi < 0) { vi = _outMesh->points().size() + vi; }
			if (vt > 0) { vt--; } else if (vt < 0) { vt = _tmpUv.size()     + vt; }
			if (vn > 0) { vn--; } else if (vn < 0) { vn = _tmpNormal.size() + vn; }

			face_vi.append(vi);
			face_vt.append(vt);
			face_vn.append(vn);

			if (!_remainTokens) break;
		}
		_outMesh->addFace(face_vi);
	}
}

void WaveFrontObjImporter::_readLine() {
	auto s = _remainLines.splitByChar('\n');
	_currentLine = s.first.trimCharsFrontAndBack(" \t\r");
//	AX_DUMP_VAR(_currentLine);
	_remainLines = s.second;
	_lineNumber++;
}

void WaveFrontObjImporter::_readToken() {
	auto s = _remainTokens.splitByChars(" \t");
	_token = s.first;
//	AX_DUMP_VAR(_token);
	_remainTokens = s.second.trimChars(" \t");
}

void WaveFrontObjImporter::_error(axStrView msg) {
	AX_LOG("Error Read WaveFrontOBJ line:{?}: {?}", _lineNumber, msg);
}

} //namespace