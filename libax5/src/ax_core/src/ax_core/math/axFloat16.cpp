#include "axFloat16.h"
#include "../string/axFormat.h"

//axReflection_initRtti(      , axFloat16)
//axReflection_onFormat(    , axFloat16)
//axReflection_onJsonReader(, axFloat16)
//axReflection_onJsonWriter(, axFloat16)

void axFloat16::onFormat(axFormat & f) const { f << toFloat32(); }

bool axStrView_tryParse(axStrView s, axFloat16& value) {
	float tmp;
	if (!s.tryParse(tmp)) return false;
	value = tmp;
	return true;
}
