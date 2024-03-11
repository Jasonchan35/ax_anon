#pragma once

#include "axRectBinPacker.h"

// http://clb.demon.fi/files/RectangleBinPack.pdf
// The Skyline Algorithms
	
class axRectBinPacker_Skyline : public axRectBinPacker {
public:
	axRectBinPacker_Skyline();
	void create(axInt width, axInt height);

	virtual Result addRect(const axVec2i& reqSize);

	axSpan<axVec2i>	skyline() const { return *_skyline; }

private:
	void _updateSkyline(axInt idx, const axRect2i& rect);
	void _addPointToNewSkyline(const axVec2i& pt);

	axInt	_width;
	axInt	_height;

	axArray<axVec2i>*	_skyline     = nullptr;
	axArray<axVec2i>*	_skylineBack = nullptr;
	axArray<axVec2i>	_skylineBuf[2];
};