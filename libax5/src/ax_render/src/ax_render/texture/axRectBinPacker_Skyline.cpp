#include <ax_render/texture/axRectBinPacker_Skyline.h>

// +: skyline points
//
// +----------------------------------------+
// |                                        |
// |                                        |
// |                                        |
// |                                        |
// |                                        |
// |          +------+                      |
// |          |      |          +------+    |
// |+----+    |      |          |      |    |
// |     |    |      +----------+      |    |
// |     +----+                        +----|
// |                                        |
// +----------------------------------------+

axRectBinPacker_Skyline::axRectBinPacker_Skyline() {
	_skyline     = &_skylineBuf[0];
	_skylineBack = &_skylineBuf[1];
}

void axRectBinPacker_Skyline::create(axInt width, axInt height) {
	_width  = width;
	_height = height;

	auto& skyline = *_skyline;

	skyline.resize(2);
	skyline.at(0) = axVec2i(0,	 0);
	skyline.at(1) = axVec2i(width, 0);
}

axRectBinPacker::Result axRectBinPacker_Skyline::addRect(const axVec2i& reqSize) {
	Result result;

	auto& skyline = *_skyline;
	if (!skyline.size()) {
		throw axError_Undefined(AX_LOC);
	}

	axInt n = skyline.size();
	axInt minY = _height;
	axInt idx = -1;
	for (axInt i = 0; i < n-1; i++) {
		auto s = skyline[i];

		auto tmpX = s.x + reqSize.x;
		if (tmpX > _width) break;

		//find max y in overlapped skyline segments in x axis
		auto tmpY = s.y;
		for (axInt j = i+1; j < n; j++) {
			auto q = skyline[j];
			if (q.x >= tmpX) break;
			if (q.y > tmpY) {
				tmpY = q.y;
			}
		}

		if (tmpY + reqSize.y > _height) break;

		if (tmpY < minY) {
			idx = i;
			minY = tmpY;
		}
	}	

	if (idx < 0) {
		result.rect.set(0, 0, 0, 0);
	}else{
		axRect2i rect(skyline[idx].x, minY, reqSize.x, reqSize.y);
		_updateSkyline(idx, rect);
		//AX_LOG("--");
		//AX_DUMP_VAR(rect);
		//AX_DUMP_VAR(*_skyline);

		result.rect = rect;
	}

	return result;
}

void axRectBinPacker_Skyline::_updateSkyline(axInt idx, const axRect2i& rect) {
	auto& skyline    = *_skyline;
	auto& newSkyline = *_skylineBack;

	axInt n = skyline.size();
	newSkyline.clear();
	newSkyline.reserve(n + 1);

	axInt xMax = rect.xMax();
	axInt yMax = rect.yMax();

	// copy all before idx
	for (axInt j = 0; j < idx; j++) {
		auto q = skyline[j];
		_addPointToNewSkyline(q);
	}

	// add new point
	{
		auto q = axVec2i(rect.x, yMax);
		_addPointToNewSkyline(q);
	}

	// bypass all overlapped points
	for (axInt j = idx; j < n-1; j++) {
		auto s = skyline[j];
		if (s.x >= xMax) break;

		idx = j;
	}

	// add new point
	{
		auto q = axVec2i(xMax, rect.y);
		_addPointToNewSkyline(q);
	}

	idx++;

	for (axInt j = idx; j < n-1; j++) {
		auto q = skyline[j];
		_addPointToNewSkyline(q);
	}

	//always add last one
	newSkyline.append(skyline.back());

	ax_swap(_skyline, _skylineBack);
}

void axRectBinPacker_Skyline::_addPointToNewSkyline(const axVec2i& q) {
	auto& newSkyline = *_skylineBack;

	if (newSkyline.size() > 0) {
		if (newSkyline.back().y == q.y) {
			return; //skip if y as same as last one
		}
	}

	newSkyline.append( q );
}
