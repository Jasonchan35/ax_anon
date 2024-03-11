#pragma once

class axRectBinPacker : public axNonCopyable {
public:

	class Result {
	public:
		axRect2i	rect;
		bool		rotated = false;
	};

	virtual Result addRect(const axVec2i& reqSize) = 0;

};