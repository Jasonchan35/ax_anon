#pragma once

#include "../libanon_config.h"
#include "AnonAttribute.h"

namespace Anon {

class AnonObject;
class AnonObjectManager;

class AnonObjectId {
public:
	using Value = axUInt64;

	bool operator==(const AnonObjectId& r) const { return _v == r._v; }
	bool operator!=(const AnonObjectId& r) const { return _v != r._v; }

	axHash onGetHash() const { return axHash_get(_v); }
	void onFormat(axFormat& f) const;

	AnonObject* getObject() const;

friend class AnonObjectManager;
protected:
	Value _v = 0;
};

class AnonFieldPath {
public:
	const axFieldInfo*	fieldInfo = nullptr;
	axIntArray_<4>		pathIndex;
	explicit operator bool() const { return fieldInfo != nullptr; }

	void onFormat(axFormat& f) const {
		f << pathIndex;
	}

	void toString(axIString& outStr, const AnonObject* obj) const;

	bool operator==(const AnonFieldPath& r) const {
		return fieldInfo == r.fieldInfo && pathIndex == r.pathIndex;
	}
};

struct AnonObjectSpec {
	using OwnFields = axTuple<>;
	using Fields = axTuple<>;

	AnonObjectSpec(axType* type_) : type(type_) {}
	const axType* type = nullptr;
};

using AnonFieldSpec = axFieldInfo;

struct AnonFieldValuePtr {
	AnonFieldValuePtr(std::nullptr_t) {}
	AnonFieldValuePtr(const axFieldInfo* fieldInfo_, void* value_) : fieldInfo(fieldInfo_), value(value_) {}

	explicit operator bool() const { return fieldInfo != nullptr; }

	const axFieldInfo* fieldInfo = nullptr;
	void* value = nullptr;
};

template<class DATA>
class AnonDataSPtr {
	using This = AnonDataSPtr;
public:
	using Data = DATA;
	using DataSPtr = axSPtr<Data>;

	AnonDataSPtr() = default;
	AnonDataSPtr(const This& r) { setData(r._data); }
	void operator=(const This& r) { setData(r._data); }

	void setData(const DataSPtr& data) { _data = data; if (_data) _data->setOwner(_owner); }

	void setOwner(AnonObject* v) { _owner = v; }

	operator       Data*()       { return _data.ptr(); }
	operator const Data*() const { return _data.ptr(); }

	      Data* ptr()       { return _data.ptr(); }
	const Data* ptr() const { return _data.ptr(); }

	      Data*	operator->()       { return _data.ptr(); }
	const Data*	operator->() const { return _data.ptr(); }

	explicit operator bool() const { return _data.ptr(); }

private:
	DataSPtr	_data;
	AnonObject*	_owner = nullptr;
};

class AnonObject : public axObject, public axMapNode<AnonObjectId, AnonObject> {
	AX_RTTI_CLASS(AnonObject, axObject);
public:
	AnonObject();
	~AnonObject();

	using Spec = AnonObjectSpec;

	struct OwnFieldInfo {};
	struct FieldInfo : public OwnFieldInfo {};

	AnonObjectId	objectId() const { return _objectId; }

	static	AnonObject*	s_find(AnonObjectId objectId);

	template<class T>
	static	T*	s_find_(AnonObjectId objectId) {
		return ax_type_cast<T>(s_find(objectId));
	}

	AnonFieldValuePtr getFieldValuePtr(const AnonFieldPath& fieldPath);

	AnonFieldValuePtr getFieldValuePtr(axStrView fieldPath);

	template<class VALUE>
	VALUE*	getValuePtr(axStrView fieldPath) {
		auto r = getFieldValuePtr(fieldPath);
		if (!r.value) return nullptr;
		if (!r.fieldInfo) return nullptr;
		if (r.fieldInfo->type() != ax_typeof<VALUE>()) {
			AX_ASSERT(false);
			return nullptr;
		}
		return reinterpret_cast<VALUE*>(r.value);
	}

	template<class VALUE>
	bool setValue(axStrView fieldPath, const VALUE& newValue) {
		auto* p = getValuePtr<VALUE>(fieldPath);
		if (!p) return false;
		*p = newValue;
		setComputeNeeded();
		return true;
	}

	AnonFieldPath getFieldPath(axStrView name);

	void setFieldValue(axStrView fieldPath, axStrView newValue);
	void setToDefaultValue();

			void getObjectInfo(axIString& info)		{ onGetObjectInfo(info); }
	virtual void onGetObjectInfo(axIString& info)	{}

	void initProps() { onInitProps(); setToDefaultValue(); }
	virtual void onInitProps() {}

	template<class FIELD_SPEC, class T>
	void _initProp(const FIELD_SPEC& spec, T& prop) {}

	void onJsonReader(axJsonReader& rd);
	void onJsonWriter(axJsonWriter& wr) const;

	bool readJsonProps(axJsonReader& rd);
	void writeJsonProps(axJsonWriter& wr) const;

	virtual bool onReadJsonProp (axJsonReader& rd);
	virtual void onWriteJsonProp(axJsonWriter& wr) const;

	AX_INLINE void setComputeNeeded();
	AX_INLINE void compute();

	virtual void onSetComputeNeeded();
	virtual void onCompute() { _calledBaseOnCompute = true; }

	axEvent<void()>	evValueChanged;

	const axNameId& name() const		{ return _name; }
	void setName(axStrView s)			{ _name = axNameId::s_make(s); }
	void setName(const axNameId& r)		{ _name = r; }

	// TODO: move to Editor ?
			void drawGizmo	(axRenderRequest& req)	{ onDrawGizmo(req); }
	virtual void onDrawGizmo(axRenderRequest& req) {}

friend class AnonObjectManager;
protected:
	axNameId	_name;
	AnonObjectId _objectId;
	axEnterOnce	_re_compute;
	bool _computeNeeded : 1;
	bool _calledBaseOnCompute : 1;
	bool _calledBaseComputeNeeded : 1;
};

inline void AnonObject::setComputeNeeded() {
	if (_computeNeeded) return;
	_computeNeeded = true;

	_calledBaseComputeNeeded = false;
	onSetComputeNeeded();
	if (!_calledBaseComputeNeeded) {
		AX_ASSERT(false); // forgot to call Base::onSetComputeNeeded() ?
	}
}

inline void AnonObject::compute() {
	if (!_computeNeeded) return;
	_computeNeeded = false;

	auto re = _re_compute.enter();
	if (!re) {
		AX_ASSERT(false); // cycling compute chain ?
		return;
	}

	_calledBaseOnCompute = false;
	onCompute();
	if (!_calledBaseOnCompute) {
		AX_ASSERT(false); // forgot to call Base::onCompute() ?
	}
}

} //namespace

