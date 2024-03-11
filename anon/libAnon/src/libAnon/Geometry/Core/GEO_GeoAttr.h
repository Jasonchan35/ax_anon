#pragma once
#include "../../Core/AnonObject.h"

namespace Anon {

#define GEO_PrimitiveType_EnumList(E) \
	E(Unknown,) \
	E(Line,) \
	E(OpenCurve, ) \
	E(CloseCurve, ) \
	E(Polygon,) \
//----
ax_ENUM_CLASS(GEO_PrimitiveType, axUInt8)

#define GEO_ComponentType_EnumList(E) \
	E(Unknown,) \
	E(Point,) \
	E(Edge,) \
	E(Vertex, ) \
	E(Primitive, ) \
	E(Pixels,) \
	E(Voxels,) \
	E(Object,) \
//----
ax_ENUM_CLASS(GEO_ComponentType, axUInt8)


class GEO_Point;
class GEO_Edge;
class GEO_Vertex;
class GEO_Primitive;
class GEO_GeoData;
class GEO_Component;

class GEO_Point {
	AX_RTTI_STRUCT(GEO_Point, axNoBase);
public:
	Vec3d pos;
	axInt firstEdge = -1;

	template<class FUNC> // bool (*FUNC)(axInt id, const GEO_Edge& e)
	AX_INLINE void foreachEdges(axSpan<GEO_Edge> edges, axInt pointId, FUNC func) const;

	AX_INLINE void set(const GEO_Point& src, axInt vertexOffset, axInt edgeOffset) {
		pos = src.pos;
		firstEdge = src.firstEdge + edgeOffset;
	}
};

class GEO_Vertex {
	AX_RTTI_STRUCT(GEO_Vertex, axNoBase);
public:
	axInt	point;

	axInt	primitive;
	axInt	edge;
	axInt	nextEdgeVertex;

	AX_INLINE void set(const GEO_Vertex& src, axInt pointOffset, axInt primitiveOffset, axInt edgeOffset) {
		point = src.point + pointOffset;
		edge  = src.edge + edgeOffset;
		primitive = src.primitive + primitiveOffset;
	}
};

class GEO_Edge {
	AX_RTTI_STRUCT(GEO_Edge, axNoBase);
public:
	axInt	point0;
	axInt	point1;

	axInt	nextEdge0; // next edge for point 0
	axInt	nextEdge1;

	axInt	firstVertex;

	void set(const GEO_Edge& src, axInt pointOffset, axInt vertexOffset, axInt edgeOffset) {
		point0 = src.point0;
		point1 = src.point1;
		nextEdge0 = src.nextEdge0 + edgeOffset;
		nextEdge1 = src.nextEdge1 + edgeOffset;
		firstVertex = src.firstVertex + vertexOffset;
	}

	template<class FUNC> // bool (*FUNC)(axInt id, const GEO_Vertex& v)
	AX_INLINE void foreachVertex(axSpan<GEO_Vertex> vertices, FUNC func) const {
		auto vi = firstVertex;
		if (vi < 0) return;
		do {
			auto& v = vertices[vi];
			if (!func(vi, v)) return;
			vi = v.nextEdgeVertex;
		} while (vi != firstVertex);
	}

	AX_INLINE bool hasPoints(axInt p0, axInt p1) const {
		return (point0 == p0 && point1 == p1)
			|| (point0 == p1 && point1 == p0);
	}

	AX_INLINE axInt oppositePoint(axInt pt) const {
		if (pt == point0) {
			return point1;
		} else {
			AX_ASSERT(pt == point1);
			return point0;
		}
	}

	AX_INLINE axInt nextEdge(axInt pt) const {
		if (pt == point0) {
			return nextEdge0;
		} else {
			AX_ASSERT(pt == point1);
			return nextEdge1;
		}
	}

	void setNextEdge(axInt next, axInt pt) {
		if (pt == point0) {
			nextEdge0 = next;
		} else {
			AX_ASSERT(pt == point1);
			nextEdge1 = next;
		}
	}

	void onFormat(axFormat& f) const;
};

class GEO_Primitive {
	AX_RTTI_STRUCT(GEO_Primitive, axNoBase);
public:

	using Type = GEO_PrimitiveType;
	Type		type = Type::Unknown;
	axIntRange	vertexRange{0,0};

	AX_INLINE void set(const GEO_Primitive& src, axInt vertexOffset) {
		type = src.type;
		vertexRange.set(src.vertexRange.start + vertexOffset, src.vertexRange.size);
	}
};

class GEO_AttrArrayBase : public axRefCounted, public axTypedBase {
	AX_RTTI_CLASS(GEO_AttrArrayBase, axTypedBase)
public:
	GEO_AttrArrayBase(GEO_Component* comp, const axType* type, axNameId name) 
		: _owner(comp), _type(type), _name(name) {}

	axNameId name() const { return _name; }
	const axType* type() const { return _type; }

	AX_INLINE axInt size() const;

			axSPtr<GEO_AttrArrayBase> baseClone(GEO_Component* comp) { return onBaseClone(comp); }
	virtual axSPtr<GEO_AttrArrayBase> onBaseClone(GEO_Component* comp) = 0;

	void copyTo(GEO_Component& target, axIntRange range, axInt offset) const	{ onCopyTo(target, range, offset); }
	void copyTo(GEO_Component& target, axInt offset = 0) const					{ onCopyTo(target, axIntRange(0, size()), offset); }

	void copyPointAttrToVertex		(axNameId name, GEO_GeoData& geo) const { onCopyPointAttrToVertex		(name, geo); }
	void copyPointAttrToPrimitive	(axNameId name, GEO_GeoData& geo) const { onCopyPointAttrToPrimitive	(name, geo); }

	void copyVertexAttrToPoint		(axNameId name, GEO_GeoData& geo) const { onCopyVertexAttrToPoint		(name, geo); }
	void copyVertexAttrToPrimitive	(axNameId name, GEO_GeoData& geo) const { onCopyVertexAttrToPrimitive	(name, geo); }

	void copyPrimitiveAttrToPoint	(axNameId name, GEO_GeoData& geo) const { onCopyPrimitiveAttrToPoint	(name, geo); }
	void copyPrimitiveAttrToVertex	(axNameId name, GEO_GeoData& geo) const { onCopyPrimitiveAttrToVertex	(name, geo); }

	GEO_Component*	owner() const { return _owner; }

friend class GEO_Component;
protected:
	void resize(GEO_Component* comp, axInt n) { onResize(comp, n); }
	virtual void onResize(GEO_Component* comp, axInt n) = 0;

	virtual void onCopyTo(GEO_Component& target, axIntRange range, axInt offset) const = 0;

	virtual void onCopyPointAttrToVertex		(axNameId name, GEO_GeoData& geo) const = 0;
	virtual void onCopyPointAttrToPrimitive		(axNameId name, GEO_GeoData& geo) const = 0;

	virtual void onCopyVertexAttrToPoint		(axNameId name, GEO_GeoData& geo) const = 0;
	virtual void onCopyVertexAttrToPrimitive	(axNameId name, GEO_GeoData& geo) const = 0;

	virtual void onCopyPrimitiveAttrToPoint		(axNameId name, GEO_GeoData& geo) const = 0;
	virtual void onCopyPrimitiveAttrToVertex	(axNameId name, GEO_GeoData& geo) const = 0;

private:
	GEO_Component* _owner = nullptr;
	const axType* _type = nullptr;
	axNameId	_name;
};

template<class T>
class GEO_AttrArray : public GEO_AttrArrayBase {
	AX_RTTI_CLASS(GEO_AttrArray<T>, GEO_AttrArrayBase)
public:
	GEO_AttrArray(GEO_Component* owner, axNameId name) : Base(owner, ax_typeof<T>(), name) {}

	   axSpan<T>	span() const	{ return _data.span(); }

	axMutSpan<T>	edit(GEO_Component* comp)	{
		if (comp != owner()) {
			throw axError_Undefined(AX_LOC);
		}
		return _data.span();
	}

	axSPtr<GEO_AttrArray> clone(GEO_Component* owner)	{
		auto p = axSPtr_new(GEO_AttrArray(owner, name()));
		p->_data = _data;
		return p;
	}

protected:
	axSPtr<GEO_AttrArrayBase> onBaseClone(GEO_Component* comp) override {
		auto o = clone(comp);
		return o.ptr();
	}

	virtual void onResize(GEO_Component* comp, axInt n) override {
		if (comp != owner()) {
			throw axError_Undefined(AX_LOC);
		}
		_data.resize(n);
	}

	virtual void onCopyTo(GEO_Component& target, axIntRange range, axInt offset) const override;  // implement in GEO_GeoData.h

	virtual void onCopyPointAttrToVertex		(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h
	virtual void onCopyPointAttrToPrimitive		(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h

	virtual void onCopyVertexAttrToPoint		(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h
	virtual void onCopyVertexAttrToPrimitive	(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h

	virtual void onCopyPrimitiveAttrToPoint		(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h
	virtual void onCopyPrimitiveAttrToVertex	(axNameId name, GEO_GeoData& geo) const override; // implement in GEO_GeoData.h

private:
	axArray<T>	_data;
};

class GEO_Component : public axTypedBase {
	AX_RTTI_CLASS(GEO_Component, axTypedBase)
public:
	using Type = GEO_ComponentType;

	GEO_Component(Type type) : _type(type) {}

	template<class T>
	axSpan<T> readAttr(axNameId name) const {
		if (auto* a = _findAttr<T>(name)) {
			return a->span();
		}
		return axSpan<T>();
	}

	template<class T>
	axMutSpan<T> newAttr(axNameId name) & {
		auto o = axSPtr_new(GEO_AttrArray<T>(this, name));
		o->resize(this, _size);
		_addAttr(o.ptr());
		return o->edit(this);
	}

	template<class T>
	axMutSpan<T> editAttr(axNameId name) {
		if (auto* a = _findAttr<T>(name)) {
			if (a->owner() == this) {
				return a->edit(this);
			}
			auto newCopy = a->clone(this);
			_addAttr(newCopy);
			return newCopy->edit(this);
		}		
		return newAttr<T>(name);
	}

	template<class T>
	axMutSpan<T> editAttr(axNameId name, axInt size) {
		resize(size);
		return editAttr<T>(name);
	}

	axInt size() const { return _size; }
	axInt attrCount() const { return _attrs.size(); }

	void clear();
	void resize(axInt n);
	void incSize(axInt n) { resize(_size + n); }

	axSpan<axSPtr<GEO_AttrArrayBase>> attrs() const { return _attrs.span(); }
	void onGetObjectInfo(axIString& info);

protected:

	void _copyAttrsTo(axNameId exceptName, GEO_Component& target, axIntRange range, axInt offset) const {
		for (auto& a : attrs()) {
			if (!a) continue;
			if (a->name() == exceptName) continue;
			a->copyTo(target, range, offset);
		}
	}

	void _addAttr(GEO_AttrArrayBase* newAttr);

	      GEO_AttrArrayBase* _findAttrBase(axNameId name) &;
	const GEO_AttrArrayBase* _findAttrBase(axNameId name) const & { return ax_const_cast(this)->_findAttrBase(name); }

	template<class T>       GEO_AttrArray<T>* _findAttr(axNameId name)       { return ax_type_cast<GEO_AttrArray<T>>(_findAttrBase(name)); }
	template<class T> const GEO_AttrArray<T>* _findAttr(axNameId name) const { return ax_type_cast<GEO_AttrArray<T>>(_findAttrBase(name)); }

	Type _type = Type::Unknown;
	axArray<axSPtr<GEO_AttrArrayBase>> _attrs;
	axInt _size = 0;
};

inline axInt GEO_AttrArrayBase::size() const { return _owner->size(); }

class GEO_PointComponent : public GEO_Component {
	AX_RTTI_CLASS(GEO_PointComponent, GEO_Component)
public:
	GEO_PointComponent() : GEO_Component(Type::Point) {}

	bool copyAttrToVertex(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyPointAttrToVertex(name, geo);
		return true;
	}

	bool copyAttrToPrimitive(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyPointAttrToPrimitive(name, geo);
		return true;
	}

	void copyAttrsTo(GEO_PointComponent& target, axIntRange range, axInt offset) const {
		_copyAttrsTo(AnonNames::point(), target, range, offset);
	}
	void copyAttrsTo(GEO_PointComponent& target, axInt offset = 0) const {
		copyAttrsTo(target, axIntRange(0, size()), offset);
	}
};

class GEO_VertexComponent : public GEO_Component {
	AX_RTTI_CLASS(GEO_VertexComponent, GEO_Component)
public:
	GEO_VertexComponent() : GEO_Component(Type::Vertex) {}

	bool copyAttrToPoint(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyVertexAttrToPoint(name, geo);
		return true;
	}

	bool copyAttrToPrimitive(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyVertexAttrToPrimitive(name, geo);
		return true;
	}

	void copyAttrsTo(GEO_VertexComponent& target, axIntRange range, axInt offset) const {
		_copyAttrsTo(AnonNames::vertex(), target, range, offset);
	}
	void copyAttrsTo(GEO_VertexComponent& target, axInt offset = 0) const {
		copyAttrsTo(target, axIntRange(0, size()), offset);
	}
};

class GEO_EdgeComponent : public GEO_Component {
	AX_RTTI_CLASS(GEO_EdgeComponent, GEO_Component)
public:
	GEO_EdgeComponent() : GEO_Component(Type::Edge) {}

	void copyAttrsTo(GEO_EdgeComponent& target, axIntRange range, axInt offset) const {
		_copyAttrsTo(AnonNames::edge(), target, range, offset);
	}
	void copyAttrsTo(GEO_EdgeComponent& target, axInt offset = 0) const {
		copyAttrsTo(target, axIntRange(0, size()), offset);
	}
};

class GEO_PrimitiveComponent : public GEO_Component {
	AX_RTTI_CLASS(GEO_PrimitiveComponent, GEO_Component)
public:
	GEO_PrimitiveComponent() : GEO_Component(Type::Primitive) {}

	bool copyAttrToPoint(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyPrimitiveAttrToPoint(name, geo);
		return true;
	}

	bool copyAttrToVertex(axNameId name, GEO_GeoData& geo) {
		auto* attr = _findAttrBase(name);
		if (!attr) return false;
		attr->copyPrimitiveAttrToVertex(name, geo);
		return true;
	}

	void copyAttrsTo(GEO_PrimitiveComponent& target, axIntRange range, axInt offset) const {
		_copyAttrsTo(AnonNames::primitive(), target, range, offset);
	}
	void copyAttrsTo(GEO_PrimitiveComponent& target, axInt offset = 0) const {
		copyAttrsTo(target, axIntRange(0, size()), offset);
	}
};

} //namespace