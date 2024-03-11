#pragma once

#include "axUINumberBox.h"
#include "../layout/axUIBoxLayout.h"

template<class T>
class axUISlider : public axUIHBoxView {
	AX_RTTI_CLASS(axUISlider, axUIHBoxView)
public:
	axUISlider(axUIView* parent);

	void setNoValue	();
	void setIsMultipleValue();

	void setValue		(const T& newValue, bool fireEvent);
	void setEditingValue(const T& newValue, bool fireEvent);

	void setNormalizedValue			(double v, bool fireEvent)	{ setValue       (fromNormalizedValue(v), fireEvent); }
	void setNormalizedEditingValue	(double v, bool fireEvent)	{ setEditingValue(fromNormalizedValue(v), fireEvent); }

	const T& value() const			{ return _numBox.value(); }
	const T& editingValue() const	{ return _numBox.editingValue(); }

	double normalizedValue() const			{ return toNormalizedValue(value()); }
	double normalizedEditingValue() const	{ return toNormalizedValue(editingValue()); }

	void setMin(const T& v);
	void setMax(const T& v);

	const T& min() const { return _min; }
	const T& max() const { return _max; }

	axDelegate<void (T& newValue)>	evSetValue;
	axDelegate<void (T& newValue)>	evSetEditingValue;

	double	toNormalizedValue(T v) const;
	T		fromNormalizedValue(double n) const;
private:

	class Slider : public axUIView {
		AX_RTTI_CLASS(Slider, axUIView)
	public:
		Slider(axUISlider* parent);

		void onRender(axRenderRequest& req) override;
		void onMouseEvent(axUIMouseEvent& ev) override;

		axUISlider* _owner;
	};

	class NumBox : public axUINumberBox<T> {
		AX_RTTI_CLASS(NumBox, axUINumberBox<T>)
	public:
		NumBox(axUISlider* parent);

		void onSetValue(T& newValue) override;
		void onSetEditingValue(T& newValue) override;

		axUISlider* _owner;
	};

	NumBox		_numBox;
	Slider		_slider;

	T _min = 0;
	T _max = 100;
};
