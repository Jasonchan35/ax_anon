#pragma once

class axAttribute : public axTypedBase {
	AX_RTTI_CLASS(axAttribute, axNoBase)
public:
};

template<class ATTR>
const ATTR* axFieldInfo::findAttr() const {
	for (auto* a : _attributes) {
		if (a->isKindOf<ATTR>()) {
			return static_cast<ATTR*>(a);
		}
	}
	return nullptr;
};

