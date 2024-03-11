#pragma once

#include "../pointer/axUPtr.h"
#include "../container/axDict.h"
#include "../rtti/axType.h"

template<class KEY, class OBJECT, class... CREATE_ARGS>
class axFactoryT;

template<class KEY, class OBJ_BASE, class... CREATE_ARGS>
class axFactoryT : public axNonCopyable {
public:
	using ObjBase = OBJ_BASE;
	using Creator = axUPtr<ObjBase> (*)(const axSourceLoc& loc, axAllocator* allocator, CREATE_ARGS... args);
	struct Entry {
		Creator creator;
	};

	using Key = KEY;
	using Dict = axDict<KEY, Entry>;
	using InKey = typename Dict::InKey;

	template<class T>
	void addEntry(const InKey& key) {
		AX_STATIC_ASSERT(axTypeTraits::isBaseOf<ObjBase, T>::value);
		auto& entry = _dict.add(key);
		entry.creator = &CreatorT<T>;
	}

	axUPtr<ObjBase> _create(const axSourceLoc& loc, axAllocator* allocator, const InKey& key, CREATE_ARGS... args) {
		auto* entry = _dict.find(key);
		if (!entry || !entry->creator) return nullptr;
		return entry->creator(loc, allocator, ax_forward(args)...);
	}

private:
	template<class T>
	static axUPtr<ObjBase> CreatorT(const axSourceLoc& loc, axAllocator* allocator, CREATE_ARGS... args) {
		AX_STATIC_ASSERT(axTypeTraits::isBaseOf<ObjBase, T>::value);
		return axUPtr_make(new(loc, allocator) T(ax_forward(args)...));
	};

	Dict _dict;
};

// ! create OBJ_BASE by axType or typeName
template<class OBJ_BASE, class... CREATE_ARGS>
class axTypeFactoryT : public axNonCopyable {
public:
	template<class IN_TYPE>
	void addTypeName() {
		auto* t = ax_typeof<IN_TYPE>();
		auto& e = _typeNameDict.add(t->name());
		e.type = t;
		_entries.emplaceBack(t);
	}

	template<class T, class OBJ_TO_CREATE>
	void addEntry() {
		addTypeName<T>();
		_impl.template addEntry<OBJ_TO_CREATE>(ax_typeof<T>());
	}

	template<class T>
	void addEntry() { addEntry<T, T>(); }

	axSpan<const axType*>	entries() const { return _entries; }

	axUPtr<OBJ_BASE> createByName(const axSourceLoc& loc, axAllocator* allocator, axStrView typeName, CREATE_ARGS... args) {
		auto* e = _typeNameDict.find(typeName);
		if (!e) return nullptr;
		return create(loc, allocator, e->type, ax_forward(args)...);
	}

	template<class T>
	axUPtr<OBJ_BASE> create(const axSourceLoc& loc, axAllocator* allocator, CREATE_ARGS... args) {
		return create(loc, allocator, ax_typeof<T>(), ax_forward(args)...);
	}

	axUPtr<OBJ_BASE> create(const axSourceLoc& loc, axAllocator* allocator, const axType* type, CREATE_ARGS... args) {
		auto* t = type;
		while (t) {
			if (auto o = _impl._create(loc, allocator, t, ax_forward(args)...))
				return o;
			t = t->baseClass();
		}
		return nullptr;
	}

private:
	struct TypeNameEntry {
		const axType* type;
	};

	axDict<axString, TypeNameEntry>	_typeNameDict;
	axArray<const axType*>	_entries;

	using FactoryImpl = axFactoryT<const axType*, OBJ_BASE, CREATE_ARGS...>;
	FactoryImpl _impl;
};
