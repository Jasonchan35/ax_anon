#include "AnonContext.h"

#include "../Scene/SCN_System.h"
#include "../Geometry/GEO_System.h"
#include "../Diagram/DiaSystem.h"

#include "../NodeGenTypes._impl.h"

namespace Anon {

AnonContext* AnonContext_instance;

AnonContext* AnonContext::s_instance() {
	return AnonContext_instance;
}

AnonContext::AnonContext() {
	AX_ASSERT(AnonContext_instance == nullptr);
	AnonContext_instance = this;
}

AnonContext::~AnonContext() {
	ax_delete(_diaSystem);
	ax_delete(_geoSystem);
	ax_delete(_scnSystem);

	AX_ASSERT(AnonContext_instance == this);
	AnonContext_instance = nullptr;
}

struct AnonContext::RegHandler {
	template<class T> void addType() {
		Helper<SCN_System, SCN_Component>::addNode<T>();
		Helper<GEO_System, GEO_Node>::addNode<T>();
		Helper<DiaSystem,  DiaNode >::addNode<T>();
	}

	RegHandler(AnonContext* anon) : _anon(anon) {}

	AnonContext* anon() { return _anon; }

private:

	template<class SYSTEM, class NODE>
	struct Helper {
		template<class T, typename std::enable_if_t< axTypeTraits::isBaseOf<NODE, T>::value, int> = 0>
		static void addNode() { SYSTEM::s_instance()->template addNodeType<T>(); }
		template<class T, typename std::enable_if_t<!axTypeTraits::isBaseOf<NODE, T>::value, int> = 0>
		static void addNode() {}
	};

	AnonContext* _anon = nullptr;
};

void AnonContext::create(CreateDesc& desc) {
	_inEditor = desc.inEditor;

	_diaSystem = ax_new DiaSystem;
	_diaSystem->create();

	_geoSystem = ax_new GEO_System;
	_geoSystem->create(_diaSystem);

	_scnSystem = ax_new SCN_System;
	_scnSystem->create();

	RegHandler h(this);
	generated_node_types(h);
}

void AnonContext::onHandleCustomAppEvent() {
	onHandleChangedFlags();
}

void AnonContext::_markChangedFlags(ChangedFlags f) {
	evRequestCustomAppEvent.emit();
	_changedFlags |= f;
}

void AnonContext::onHandleChangedFlags() {
	if (_changedFlags == ChangedFlags::None)
		return;

	_evChangedFlags.emit(_changedFlags);
	_changedFlags = ChangedFlags::None;
}

void AnonContext::bindChangedFlagsEvent(axObject* obj, axFunc<void(const ChangedFlags& flags)> func) {
	_evChangedFlags.bind(obj, func);
}

} //namespace
