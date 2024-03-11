#pragma once

#include "ModelImporter.h"

namespace Anon {

class WaveFrontObjImporter : public ModelImporter {
public:
	void loadFile(axEditableMesh& outMesh, axStrView filename);
	void loadMem (axEditableMesh& outMesh, axByteSpan data);

private:
	void _readLine();
	void _readToken();

	void _error(axStrView msg);
	void _parseLine();

	axStrView	_source;
	axStrView	_remainLines;
	axStrView	_currentLine;
	axStrView	_remainTokens;
	axStrView	_token;
	axInt		_lineNumber = 0;

	axEditableMesh* _outMesh = nullptr;
	axArray<axVec2d> _tmpUv;
	axArray<axVec3d> _tmpNormal;
};

} //namespace
