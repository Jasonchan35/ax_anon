#include "axFieldInfo.h"

void axFieldInfo::onFormat(axFormat& f) const {
	f.format("{?} : {?}, offset = {?}", _name, _type->name(), _offset);
}
