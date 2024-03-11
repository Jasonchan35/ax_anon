#include "axUICommon.h"

void axUIMeasured::onFormat(axFormat& f) const {
	f << "min=" << min << ", max=" << max;
}

axVec2f axUIAlign::posInRect(const axRect2f& rect, const axVec2f& contentSize) const {
	auto diff = rect.size - contentSize;
	ax_clamp_it(diff, axVec2f(0), rect.size);

	axVec2f outPos{0,0};

	switch (x) {
		case X::Left:	outPos.x = rect.pos.x;	break;
		case X::Center:	outPos.x = diff.x / 2;	break;
		case X::Right:	outPos.x = diff.x;		break;
	}

	switch (y) {
		case Y::Top:	outPos.y = rect.pos.y;	break;
		case Y::Center:	outPos.y = diff.y / 2;	break;
		case Y::Bottom:	outPos.y = diff.y;		break;
	}

	return outPos;
}
