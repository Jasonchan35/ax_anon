#pragma once
#include "axUITextBox.h"

template<class T>
class axUINumberBox : public axUITextBoxBase {
	AX_RTTI_CLASS(axUINumberBox<T>, axUITextBoxBase)
public:
	axUINumberBox(axUIView* parent);
	axUINumberBox(axUIView* parent, const T& value);

	const T& value			() const { return _value; }
	const T& editingValue	() const { return _editingValue; }

	void setValue			(const T& newValue, bool fireEvent);
	void setEditingValue	(const T& newValue, bool fireEvent);

	virtual void onSetValue			(T& newValue) { evSetValue.emit(newValue);     }
	virtual void onSetEditingValue	(T& newValue) { evSetEditingValue.emit(newValue); }

	axDelegate<void (T& newValue)>	evSetValue;
	axDelegate<void (T& newValue)>	evSetEditingValue;

protected:
	virtual bool onSetValueByText(axStrViewU text, bool fireEvent) override;
	virtual void onMouseEvent(axUIMouseEvent& ev) override;

private:
	T _value = 0;
	T _editingValue = 0;
	double _mouseDragSensitive = 1;

	axEnterOnce	_re_setValue;
	axEnterOnce	_re_setEditingValue;
};

using axUIIntNumberBox    = axUINumberBox<axInt>;
using axUIFloatNumberBox  = axUINumberBox<float>;
using axUIDoubleNumberBox = axUINumberBox<double>;
