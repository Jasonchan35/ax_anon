#pragma once

#include "AnonObjectManager.h"

namespace Anon {

class DiaSystem;
class GEO_System;
class SCN_System;

#define AnonContextChangedFlags_EnumList(E) \
	E(None,			= 0) \
	E(NewObject,	= 1 << 0) \
	E(DeleteObject,	= 1 << 1) \
	E(RenameObject,	= 1 << 2) \
	E(ComputeNeeded,= 1 << 3) \
//----
ax_ENUM_FLAGS_CLASS(AnonContextChangedFlags, axUInt64)

class AnonContext : public axTypedBase {
	AX_RTTI_CLASS(AnonContext, axTypedBase);
public:
	AnonContext();
	~AnonContext();

	static AnonContext* s_instance();

	struct CreateDesc {
		bool inEditor = false;
	};

	void create(CreateDesc& desc);

	AnonObjectManager objectManager;

	bool inEditor() const { return _inEditor; }

	using ChangedFlags = AnonContextChangedFlags;

	void markChangedFlags(ChangedFlags f);
	ChangedFlags changedFlags() { return _changedFlags; }

	void onHandleChangedFlags();
	void bindChangedFlagsEvent(axObject* obj, axFunc<void(const ChangedFlags& flags)> func);

	axEvent<void ()> evRequestCustomAppEvent;
	void onHandleCustomAppEvent();

	void setComputeNeeded() { markChangedFlags(ChangedFlags::ComputeNeeded); }

private:
	bool _inEditor = false;

	void _markChangedFlags(ChangedFlags f);
	ChangedFlags _changedFlags = ChangedFlags::None;
	axEvent<void (const ChangedFlags& flags)>	_evChangedFlags;

	DiaSystem*	_diaSystem = nullptr;
	GEO_System*	_geoSystem = nullptr;
	SCN_System*	_scnSystem = nullptr;

	struct RegHandler;
};

inline
void AnonContext::markChangedFlags(ChangedFlags f) {
	if (ax_enum(_changedFlags).hasFlags(f))
		return;
	_markChangedFlags(f);
}

} //namespace
