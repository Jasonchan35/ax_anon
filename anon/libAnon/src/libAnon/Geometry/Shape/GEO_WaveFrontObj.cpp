#include "GEO_WaveFrontObj.h"
#include "../Core/GEO_GeoData_RenderMeshHelper.h"
#include <libAnon/Importer/WaveFrontObjImporter.h>

namespace Anon {

void GEO_WaveFrontObj::onCompute() {
	Base::onCompute();

	if (_loadedFilename != _filename) {
		_loadedFilename = _filename;
		_editableMesh.clear();

		AX_DUMP_VAR(axPath::currentDir());

		if (axFile::exists(_filename)) {
			WaveFrontObjImporter importer;
			importer.loadFile(_editableMesh, _filename);
		}
	}

	GEO_GeoData_RenderMeshHelper::createGeoData(_output, _editableMesh);
}

}