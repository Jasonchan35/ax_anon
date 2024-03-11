#pragma once

#include "../libanonEditor_config.h"

namespace Anon { namespace Editor {

class AppCommandManager;

class AppCommand : public axObject, public axDListNode<AppCommand> {
	AX_RTTI_CLASS(AppCommand, axObject)
public:
	void undo() { onUndo(); }
	void redo() { onRedo(); }

	virtual void onFormat(axFormat& f) const;

	bool isPreview = false;

protected:
	virtual void onUndo() = 0;
	virtual void onRedo() = 0;
};

class AppCommandGroup : public axObject, public axDListNode<AppCommandGroup> {
	AX_RTTI_CLASS(AppCommandGroup, axObject)
public:
	void undo();
	void redo();

	void onFormat(axFormat& f) const;

	bool isPreview = false;

friend class AppCommandManager;
protected:
	axDList<AppCommand> _cmdList;
	axString _name;
};

class AppCommandManager : public axObject {
public:
	AppCommandManager();
	static AppCommandManager* s_instance();

	void newGroup(axStrView name);
	void addCommand(AppCommand* cmd, bool isPreview = false);

	bool undoPreview();
	bool undo();

	bool redo();
	void dumpUndoList();
	void clearUndoList();

	axEvent<void (const AppCommand& cmd, bool undo)> evCommand;

private:
	bool 	_debugDump = false;
	axInt	_maxUndoCount = 100;

	axDList<AppCommandGroup> _undoList;
	axDList<AppCommandGroup> _redoList;
};

class SetValueCommand : public AppCommand {
public:
	axArray<AnonObjectId, 4>	objects;
	axString					fieldPath;
	axString					newValue;
	axString					oldValue;

	void onUndo() override {
		for (auto& i : objects) {
			if (auto* o = AnonObject::s_find(i)) {
				o->setFieldValue(fieldPath, oldValue);
			}
		}
	}
	void onRedo() override {
		for (auto& i : objects) {
			if (auto* o = AnonObject::s_find(i)) {
				o->setFieldValue(fieldPath, newValue);
			}
		}
	}

	void onFormat(axFormat& f) const override {
		f.format("SetValueCommand Redo [{?}] [{?}]", fieldPath, newValue);
	}
};


}} //namespace
