#pragma once

#include "../rtti/axTypeManager.h"
#include "../pointer/axLink.h"
#include "../pointer/axWeak.h"

#define AX_CLASS(...)
#define AX_PROP(...)
#define AX_EVENT(...)

#define AX_CLASS_SPEC(T, BASE) \
	private: \
		AX_RTTI_CLASS_CUSTOM_INIT(T, BASE) \
	protected: \
		struct Spec; \
		virtual void onInitProps() override; \
		virtual bool onReadJsonProp (axJsonReader& rd_) override; \
		virtual void onWriteJsonProp(axJsonWriter& wr_) const override; \
	public: \
		static  T*   s_find(AnonObjectId id) { return s_find_<T>(id); } \
	private: \
//-----


class axObject : public axLinkable, public axWeakRefBase, public axTypedBase {
	AX_RTTI_CLASS(axObject, axTypedBase);
public:

	void			setDebugName(axStrLiteral name)	{ _debugName = name; }
	axStrLiteral	debugName	() const			{ return _debugName; }

	virtual void onLinkedObjectDestroy(axObject* obj) {}

	template<class T>
	T* findLinkedOwner() {
		for (auto& link : this->Linkable_each()) {
			if (auto* p = ax_type_cast<T>(link.owner())) {
				return p;
			}
		}
	}

private:
	axStrLiteral _debugName;

};
