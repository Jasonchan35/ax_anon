#pragma once

#include "../App/AppCommand.h"

namespace Anon { namespace Editor {

class AnonObjectBinding : public axObject {
	AX_RTTI_CLASS(AnonObjectBinding, axObject)
public:
	virtual ~AnonObjectBinding();

	axArray<AnonObjectId, 4>	objects;

	bool addObject(AnonObject* obj);
	void clearObjects();

	template<class VALUE>
	struct GetValueResult {
		GetValueResult(VALUE* value_, bool isMultipleValue_)
			: value(value_)
			, isMultipleValue(isMultipleValue_)
		{}
		VALUE* value = nullptr;
		bool isMultipleValue = false;
	};

	template<class VALUE>
	GetValueResult<VALUE> getValue(axStrView fieldPath) {
		using Result = GetValueResult<VALUE>;
		VALUE* value = nullptr;

		for (auto& objId : objects) {
			auto* obj = AnonObject::s_find(objId);
			if (!obj) continue;
			auto* v = obj->getValuePtr<VALUE>(fieldPath);
			if (!v) return Result(nullptr, false); // error get value
			if (value && *value != *v) {
				return Result(nullptr, true);
			}
			value = v;
		}
		return Result(value, false);
	}

	template<class VALUE>
	void setValue(axStrView fieldPath, const VALUE& newValue, bool isPreview) {
		auto* mgr = AppCommandManager::s_instance();
		mgr->undoPreview();

		auto cmd = axUPtr_new(SetValueCommand());
		cmd->objects = objects;
		cmd->fieldPath = fieldPath;
		
		axJsonUtil::stringify(cmd->newValue, newValue);

		auto result = getValue<VALUE>(fieldPath);
		if (result.value) {
			axJsonUtil::stringify(cmd->oldValue, *result.value);
		}

		mgr->addCommand(cmd.detach(), isPreview);

		evValueChanged.emit();
	}

	void onGetObjectInfo(axIString& info);

	axEvent<void ()>	evValueChanged;

	void setToDefaultValue();

private:
	void _setComputeNeeded();
};


}}
