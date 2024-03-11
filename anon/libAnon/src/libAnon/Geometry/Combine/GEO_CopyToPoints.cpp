#include "GEO_CopyToPoints.h"

namespace Anon {

void GEO_CopyToPoints::onCompute() {
	Base::onCompute();
	if (bypass()) return;

	// auto srcSpan = input().readPoints();
	// auto inputPointSpan = _inputPoints.readPoints();

	// auto totalPoints = srcSpan.size() * inputPointSpan.size();
	// auto outPoints = _output.editPoints(totalPoints);
}

}