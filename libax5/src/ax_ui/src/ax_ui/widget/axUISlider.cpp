#include "axUISlider.h"

template<class T>
axUISlider<T>::axUISlider(axUIView* parent) 
	: Base(parent)
	, _numBox(this)
	, _slider(this)
{
	Base::layout.setMinW(50);

	_numBox.evSetValue.bind(this, [this](auto newValue) {
		evSetValue.emit(newValue);
	});

	_numBox.evSetEditingValue.bind(this, [this](auto newValue) {
		evSetEditingValue.emit(newValue);
	});
}

template<class T>
axUISlider<T>::Slider::Slider(axUISlider<T>* parent)
	: Base(parent)
	, _owner(parent)
{
	setPadding({0, 8});
}

template<class T>
void axUISlider<T>::Slider::onRender(axRenderRequest& req) {
	Base::onRender(req);

	auto rc = contentRect();
	req.ui.drawRect(AX_LOC, rc, Color(0,0,0));

	auto nl = _owner->normalizedEditingValue();
	rc.w = ax_ceil(rc.w * static_cast<float>(nl));

	req.ui.drawRect(AX_LOC, rc, Color(.1f, .35f, .8f));
}

template<class T>
void axUISlider<T>::Slider::onMouseEvent(axUIMouseEvent& ev) {
	if (ev.isDragging()) {
		auto rc = contentRect();
		auto x = ev.pos.x - rc.x;
		auto v = x / rc.w;
		_owner->setNormalizedEditingValue(v, true);
	}
	Base::onMouseEvent(ev);
}

template<class T>
axUISlider<T>::NumBox::NumBox(axUISlider<T>* parent)
	: Base(parent)
	, _owner(parent)
{
	this->layout.setFixedW();
	this->setWidth(80);
}

template<class T>
void axUISlider<T>::setValue(const T& newValue, bool fireEvent) {
	_numBox.setValue(newValue, fireEvent);
}

template<class T>
void axUISlider<T>::setEditingValue(const T& newValue, bool fireEvent) {
	_numBox.setEditingValue(newValue, fireEvent);
}

template<class T>
void axUISlider<T>::NumBox::onSetValue(T& newValue) {
	newValue = ax_clamp(newValue, _owner->min(), _owner->max());
	Base::onSetValue(newValue);
}

template<class T>
void axUISlider<T>::NumBox::onSetEditingValue(T& newValue) {
	newValue = ax_clamp(newValue, _owner->min(), _owner->max());
	Base::onSetEditingValue(newValue);
}


template<class T> void axUISlider<T>::setMin(const T& v) { _min = v; setNeedToRender(); }
template<class T> void axUISlider<T>::setMax(const T& v) { _max = v; setNeedToRender(); }

template<class T>
double axUISlider<T>::toNormalizedValue(T v) const {
	auto dis = _max - _min;
	if (dis <= 0) return 0;

	double nl = static_cast<double>(v - min()) / static_cast<double>(dis);
	return ax_clamp01(nl);
}

template<class T>
T axUISlider<T>::fromNormalizedValue(double n) const {
	auto dis = _max - _min;
	if (dis <= 0) return _min;

	T o = static_cast<T>(dis * n) + _min;
	return ax_clamp(o, _min, _max);
}

template<class T> void axUISlider<T>::setNoValue()			{ _numBox.setNoValue(); }
template<class T> void axUISlider<T>::setIsMultipleValue()	{ _numBox.setIsMultipleValue(); }


#define	AX_MACRO_OP(T) \
	template class axUISlider<T>; \
//-------------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP
