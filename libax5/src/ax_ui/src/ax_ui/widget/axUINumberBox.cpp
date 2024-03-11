#include "axUINumberBox.h"

template<class T>
axUINumberBox<T>::axUINumberBox(axUIView* parent)
	: Base(parent)
	, _mouseDragSensitive(0.2)
{
	layout.setMinW(50);
	layout.setFitContentW().setFitContentH();
}

template<class T>
axUINumberBox<T>::axUINumberBox(axUIView* parent, const T& value)
	: axUINumberBox(parent)
{
	setValue(value, false);
}

template<class T>
void axUINumberBox<T>::setValue(const T& newValue, bool fireEvent) {
	T tmp = newValue;
	if (auto re = _re_setValue.enter()) {
		if (fireEvent) onSetValue(tmp);
	}
	_value = tmp;
	_editingValue = tmp;
	auto text = axTempStringU::s_convert(_value);
	setBaseText(text);
}

template<class T>
void axUINumberBox<T>::setEditingValue(const T& newValue, bool fireEvent) {
	T tmp = newValue;
	if (auto re = _re_setEditingValue.enter()) {
		if (fireEvent) onSetEditingValue(tmp);
	}

	_editingValue = tmp;
	auto text = axTempStringU::s_convert(_editingValue);
	setBaseText(text);
}

template<class T>
bool axUINumberBox<T>::onSetValueByText(axStrViewU text, bool fireEvent) {
	T tmp = _value;
	axTempString str;
	str.setUtf(text);

	if (!str.tryParse(tmp))
		return false;

	setValue(tmp, fireEvent);
	return true;
}

template<class T>
void axUINumberBox<T>::onMouseEvent(axUIMouseEvent& ev) {
	using Type		= axUIMouseEvent::Type;
	using Modifier	= axUIMouseEvent::Modifier;
	using Button	= axUIMouseEvent::Button;

	switch (ev.type) {
		case Type::DragBegin: {
			_editingValue = _value;
		} break;

		case Type::Dragging: {
			if (ev.pressedButtons == Button::Middle) {
				auto s = _mouseDragSensitive;
				if (ax_enum(ev.modifier).hasFlags(Modifier::Ctrl)) {
					s *= 0.1;
				} else if (ax_enum(ev.modifier).hasFlags(Modifier::Shift)) {
					s *= 10;
				}

				auto v = _editingValue + ev.deltaPos.x * s;
				setEditingValue(static_cast<T>(v), true);
			}
		} break;

		case Type::DragEnd: {
			setValue(_editingValue, true);
		} break;
	}

	Base::onMouseEvent(ev);
}

#define	AX_MACRO_OP(T) \
	template class axUINumberBox<T>; \
//-------------
	AX_TYPE_LIST_SINT(AX_MACRO_OP)
	AX_TYPE_LIST_UINT(AX_MACRO_OP)
	AX_TYPE_LIST_FLOAT(AX_MACRO_OP)
#undef AX_MACRO_OP
