#include "App.h"
#include "Generator.h"

namespace axNodeGen {

int axNodeGen::App::onRun() {
	axString currentDir;
	axPath::getCurrentDir(currentDir);
	AX_DUMP_VAR(currentDir);


	auto args = commandArguments();

	axString inputPath;

	for (axInt i = 1; i < args.size(); i++) {
		auto& a = args[i];
		if (a.startsWith("-")) {
			AX_LOG("unknown option {?}", a);
			return -1;
		} else {
			inputPath = a;
		}
	}

	axArray<axString> inputFiles;
	axTempString tmp;

	tmp.set(inputPath, "/**/*.h");
	AX_DUMP_VAR(tmp);

	axString	_outGenTypeHeaders("#pragma once\n\n");
	axString	_outGenTypes("template<class HANDLER> inline\n"
							 "static void generated_node_types(HANDLER& handler) {\n");

	axPath::glob(inputFiles, tmp, false, true, false);
	inputFiles.sort();

	for (auto& f : inputFiles) {
		Generator g;
		g.gen(f);

		if (!g._typeDB.types.size())
			continue;

		axPath::getRelative(tmp, f, inputPath);
		_outGenTypeHeaders.appendFormat("#include \"{?}\"\n", tmp);

		for (auto& t : g._typeDB.types.values()) {
			_outGenTypes.appendFormat("\t""handler.template addType< {?,40} >();\n", t.fullname);
		}
	}

	_outGenTypes.append("}\n\n");

	{
		axTempString txt(_outGenTypeHeaders, "\n\n", _outGenTypes);
		axTempString outFilename(inputPath, "/NodeGenTypes._impl.h");
		axFile::writeFileIfChanged(outFilename, txt, true, false);
	}

	return 0;
}


} //namespace axNodeGen