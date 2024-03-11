#include "GEO_Modifier.h"

namespace Anon {

void GEO_Modifier::setBypass(bool b) {
	if (_bypass == b) return;
	_bypass = b;
	setComputeNeeded();
}

void GEO_Modifier::onCompute() {
	Base::onCompute();
	_output = _input;
}

}