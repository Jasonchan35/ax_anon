#pragma once

#include "AppCommand.h"

namespace Anon { namespace Editor {

class SelectionItem {
public:
	SelectionItem() = default;

	SelectionItem(const AnonObjectId & objectId_)
		: objectId(objectId_)
	{}

	AnonObjectId objectId;
	AnonObject* getObject() const { return objectId.getObject(); }

	template<class T>
	T* get() const { return ax_type_cast<T>(getObject()); }

	void onFormat(axFormat& f) const;
};

class Selection : public axNonCopyable {
public:
	using Item = SelectionItem;

	void select(AnonObject* obj);
	void select(axSpan<AnonObject*> entities);
	void select(axSpan<Item> items) { _select(items); }

	bool has(AnonObject* obj);

	axSpan<Item> current() const { return _current; }

	axEvent<void ()> evChanged;

private:
	void _select(axSpan<Item> items);

	class SelectCommand : public AppCommand {
		AX_RTTI_CLASS(SelectCommand, AppCommand)
	public:
		SelectCommand(axSpan<Item> before_) {
			before = before_;
		}

		axArray<Item> before;
		axArray<Item> after;

		virtual void onUndo() override;
		virtual void onRedo() override;
		
		virtual void onFormat(axFormat& f) const override;
	};

friend class UndoSelect;
protected:
	axArray<Item>	_current;
	axEnterOnce		_enterOnce;
};

}} //namespace
