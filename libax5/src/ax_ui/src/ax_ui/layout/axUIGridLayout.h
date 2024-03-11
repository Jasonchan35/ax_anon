#pragma once

#include "axUILayout.h"

class axUIGridLayoutHandler : public axUILayoutHandler {
	AX_RTTI_CLASS(axUIGridLayoutHandler, axUILayoutHandler)
public:
	class TrackList;
	class Track {
	public:
		float grow;
		float minSize;
		float maxSize;

		Track();

		float outStart() const { return _outStart; }
		float outEnd  () const { return _outStart + _outSize; }
		float outSize () const { return _outSize;  }

	friend class TrackList;
	protected:
		float _outStart = 0;
		float _outSize  = 0;

	};

	class TrackList : public axNonCopyable {
	public:
		void onUpdateLayout(float width);
		Track* add(float grow, float minSize = 0, float maxSize = ax_inf);

		float outTotalSize() const { return _outTotalSize; }
		float spacing = 0;

		axArray<Track> tracks;

	protected:
		float _outTotalSize = 0;
	};

	virtual	void onUpdateLayout(axUIView* view) override;
	virtual void onMeasureLayout(axUIView* view, axUIMeasureRequest& req) override;

	TrackList rows;
	TrackList cols;
};

// Wrap axUILayoutHandler into view
class axUIGridLayoutView : public axUILayoutView {
	AX_RTTI_CLASS(axUIGridLayoutView, axUILayoutView)
public:
	axUIGridLayoutView(axUIView* parent) : Base(parent) { setLayoutHandler(&gridLayout); }
	axUIGridLayoutHandler gridLayout;
};