#include "axUIGridLayout.h"

void axUIGridLayoutHandler::onUpdateLayout(axUIView* view) {
	auto rc = view->contentRect();
	cols.onUpdateLayout(rc.w);
	rows.onUpdateLayout(rc.h);

	auto numCols = cols.tracks.size();
	auto numRows = rows.tracks.size();

	for (auto& v : view->children()) {
		auto startCol = ax_max(axInt(0), v.layout.cellX());
		auto startRow = ax_max(axInt(0), v.layout.cellY());

		auto endCol   = ax_min(numCols, startCol + v.layout.cellSpanX()) - 1;
		auto endRow   = ax_min(numRows, startRow + v.layout.cellSpanY()) - 1;

		if (endRow < startRow) endRow = startRow;
		if (endCol < startCol) endCol = startCol;

		axRect2f result = v.rect();
		if (numCols) {
			result.x = cols.tracks[startCol].outStart();
			result.w = cols.tracks[endCol  ].outEnd() - result.x;
		}

		if (numRows) {
			result.y = rows.tracks[startRow].outStart();
			result.h = rows.tracks[endRow  ].outEnd() - result.y;
		}

		v.setLayoutResultRect(result);
	}
}

void axUIGridLayoutHandler::onMeasureLayout(axUIView* view, axUIMeasureRequest& req) {
	AX_ASSERT(false); // TODO
}

void axUIGridLayoutHandler::TrackList::onUpdateLayout(float width) {
	float totalMinSize = 0;
	float totalGrow = 0;

	for (auto& c : tracks) {
		totalMinSize += c.minSize;
		if (c.grow < 0) {
			c.grow = 0;
		}
		totalGrow    += c.grow;
	}

	float remain = width - totalMinSize;
	if (remain < 0) {		
		remain = 0;
	}

	for (auto& c : tracks) {
		c._outSize = c.minSize;
	}

	axInt iterCount = 0;
	while (remain > 0 && iterCount < 16) {
		float nextRemain = remain;
		float nextTotalGrow = 0;

		for (auto& c : tracks) {
			float newSize = c._outSize;
			if (totalGrow != 0) {			
				newSize += ax_ceil(c.grow * remain / totalGrow);
			}
			newSize = ax_clamp(newSize, c.minSize, c.maxSize);
			auto diff = newSize - c._outSize;
			if (diff > 0) {
				nextTotalGrow += c.grow;
				nextRemain -= diff;
				c._outSize = newSize;
			}
		}

		if (nextRemain == remain)
			break;

		remain = nextRemain;
		totalGrow = nextTotalGrow;
		iterCount++;
	}

	_outTotalSize = 0;
	for (auto& c : tracks) {
		c._outStart = _outTotalSize;
		_outTotalSize += c._outSize;
	}
}

axUIGridLayoutHandler::Track* axUIGridLayoutHandler::TrackList::add(float grow, float minSize, float maxSize) {
	auto& p = tracks.emplaceBack();
	p.grow = grow;
	p.minSize = minSize;
	p.maxSize = maxSize;
	return &p;
}

axUIGridLayoutHandler::Track::Track()
	: grow(1)
	, minSize(0)
	, maxSize(ax_inf)
{

}
