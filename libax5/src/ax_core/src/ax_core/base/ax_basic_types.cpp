#include "ax_basic_types.h"

void axSourceLoc::onJsonWriter(axJsonWriter& wr) const {
	wr.writeMember("func", axStrView_c_str(func));
	wr.writeMember("file", axStrView_c_str(file));
	wr.writeMember("line", line);
}

void axSourceLoc::onSerialize(axBinSerializer& se) {
	se.io(axStrView_c_str(func));
	se.io(axStrView_c_str(file));
	se.io(line);
}

void axSourceLoc::onFormat(axFormat& f) const {
	f.format("func:{?}\nfile:{?}:{?}", axStrView_c_str(func), axStrView_c_str(file), line);
}
