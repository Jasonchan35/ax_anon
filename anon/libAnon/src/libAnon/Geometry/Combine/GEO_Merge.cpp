#include "GEO_Merge.h"

namespace Anon {

void GEO_Merge::onCompute() {
	Base::onCompute();
	if (bypass()) return;
	_output.combine(_input2);
}

}