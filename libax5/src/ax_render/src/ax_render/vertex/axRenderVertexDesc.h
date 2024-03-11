#pragma once

#include "axRenderVertexType.h"

class axRenderVertexDescData : public axNonCopyable {
	using This = axRenderVertexDescData;
public:
	using AttrId = axRenderVertexAttrId;
	using DataType = axRenderDataType;
	using VertexType = axRenderVertexType;

	class Attr {
	public:
		Attr() : active(false) {}
		axInt16		offset   = 0;
		axInt16		sizeInBytes = 0;
		AttrId		attrId   = AttrId::Unknown;
		DataType	dataType = DataType::Unknown;
		bool		active : 1;

		void onFormat(axFormat& f) const;
	};

	axRenderVertexDescData();

	template<class Obj, class T>
	void addAttr(AttrId attrId, T Obj::*member);

	void addAttr(AttrId attrId, axInt offset, DataType dataType_);

	template<class Obj, class T, axInt N>
	void addAttrElement(AttrId attrId, axFixedArray<T, N> Obj::*member, axInt elementIndex);

	bool hasPositions	() const { return attr(AttrId::POSITION0).active != 0; }
	bool hasNormals		() const { return attr(AttrId::NORMAL0  ).active != 0; } 
	bool hasTangents	() const { return attr(AttrId::TANGENT0 ).active != 0; } 
	bool hasUvSet		(axInt i) const { return attr(AttrId::TEXCOORD0 + static_cast<axEnum<AttrId>::IntType>(i)).active != 0; }
	bool hasColorSet	(axInt i) const { return attr(AttrId::COLOR0    + static_cast<axEnum<AttrId>::IntType>(i)).active != 0; }
	bool hasCustomSet	(axInt i) const { return attr(AttrId::CUSTOM0   + static_cast<axEnum<AttrId>::IntType>(i)).active != 0; }

	axInt16		strideBytes = 0;
	VertexType	vertexType = VertexType::Unknown;
	
	axArray<    Attr*, ax_enum_int(AttrId::_END)> activeAttrs;
	axFixedArray<Attr, ax_enum_int(AttrId::_END)> allAttrs;

	const Attr& attr(AttrId i) const	{ return allAttrs[ax_enum_int(i)]; }
	axStrLiteral name;

	bool hasAllAttrs(const This& r) const {
		for (auto& ra : r.activeAttrs) {
			if (!allAttrs[ax_enum_int(ra->attrId)].active) return false;
		}
		return true;
	}

	bool operator==(const This& r) const { return vertexType == r.vertexType; }
	bool operator!=(const This& r) const { return vertexType != r.vertexType; }

	void onFormat(axFormat& f) const;
};

class axRenderVertexDesc {
	using This = axRenderVertexDesc;
public:
	using Data = const axRenderVertexDescData*;
	using Attr = axRenderVertexDescData::Attr;

	axRenderVertexDesc(Data data) : _data(data) {}

	Data operator->() const { return _data; }
	Data operator*() const  { return _data; }

	Data ptr() const { return _data; }

	bool operator==(const This& r) const {
		if (_data == r._data) return true;
		if (_data == nullptr || r._data == nullptr) return false;
		return *_data == *r._data;
	}
	bool operator!=(const This& r) const { return !operator==(r); }

	bool hasAllAttrs(const This& r) const {
		if (!_data || !r._data) return false;
		return _data->hasAllAttrs(*r._data);
	}

	explicit operator bool() const { return _data != nullptr; }

	void onFormat(axFormat& f) const { 
		if (_data) {
			f << *_data; 
		}else {
			f << "null";
		}
	}

private:
	Data _data;
};

template<class VERTEX>
class axRenderVertexDescMakeT : public axRenderVertexDescData {
public:
	using Vertex = VERTEX;
	using AttrId = axRenderVertexAttrId;
	using DataType = axRenderDataType;
	using VertexType = axRenderVertexType;
	using NormalType = axRenderNormalType;

	template<class Vertex, class Pos>
	struct AddPosAttrHelper {
		static void add(axRenderVertexDescData& desc) {
			desc.addAttr<Vertex, Pos>(AttrId::POSITION0, &VERTEX::pos);
		}
	};
	template<class Vertex> struct AddPosAttrHelper<Vertex, void> { static void add(axRenderVertexDescData& desc) {} };

	template<class Vertex, class Uv>
	struct AddUvAttrHelper {
		static void add(axRenderVertexDescData& desc) {
			for (axUInt8 i = 0; i < Vertex::kUvCount; i++) {
				desc.addAttrElement<Vertex, Uv, Vertex::kUvCount>(AttrId::TEXCOORD0 + i, &Vertex::uv, i);
			}
		}
	};
	template<class Vertex> struct AddUvAttrHelper<Vertex, void> { static void add(axRenderVertexDescData& desc) {} };

	template<class Vertex, class Color>
	struct AddColorAttrHelper {
		static void add(axRenderVertexDescData& desc) {
			for (axUInt8 i = 0; i < Vertex::kColorCount; i++) {
				desc.addAttrElement<Vertex, Color, Vertex::kColorCount>(AttrId::COLOR0 + i, &Vertex::color, i);
			}
		}
	};
	template<class Vertex> struct AddColorAttrHelper<Vertex, void> { static void add(axRenderVertexDescData& desc) {} };

	template<class Vertex, NormalType normalType>
	struct AddNormalAttrHelper;

	template<class Vertex>
	struct AddNormalAttrHelper<Vertex, NormalType::None> {
		static void add(axRenderVertexDescData& desc) {}
	};

	template<class Vertex>
	struct AddNormalAttrHelper<Vertex, NormalType::Normal> {
		static void add(axRenderVertexDescData& desc) {
			desc.addAttr<Vertex, decltype(Vertex::normal)>(AttrId::NORMAL0, &Vertex::normal);
		}
	};

	template<class Vertex>
	struct AddNormalAttrHelper<Vertex, NormalType::NormalTangent> {
		static void add(axRenderVertexDescData& desc) {
			desc.addAttr<Vertex, decltype(Vertex::normal )>(AttrId::NORMAL0, &Vertex::normal);
			desc.addAttr<Vertex, decltype(Vertex::tangent)>(AttrId::TANGENT0, &Vertex::tangent);
		}
	};

	template<class Vertex>
	struct AddNormalAttrHelper<Vertex, NormalType::NormalTangentBinormal> {
		static void add(axRenderVertexDescData& desc) {
			desc.addAttr<Vertex, decltype(Vertex::normal  )>(AttrId::NORMAL0, &Vertex::normal);
			desc.addAttr<Vertex, decltype(Vertex::tangent )>(AttrId::TANGENT0, &Vertex::tangent);
			desc.addAttr<Vertex, decltype(Vertex::binormal)>(AttrId::BINORMAL0, &Vertex::binormal);
		}
	};

	axRenderVertexDescMakeT() {
		vertexType = VERTEX::kType;
		AddPosAttrHelper<   Vertex, typename Vertex::Pos  >::add(*this);
		AddUvAttrHelper<    Vertex, typename Vertex::Uv   >::add(*this);
		AddColorAttrHelper< Vertex, typename Vertex::Color>::add(*this);
		AddNormalAttrHelper<Vertex, Vertex::kNormalType>::add(*this);
		strideBytes = sizeof(Vertex);
	}
};

template<class Obj, class T> inline
void axRenderVertexDescData::addAttr(AttrId attrId, T Obj::*member) {
	auto offset = axPtrUtil::memberOffset(member);
	addAttr(attrId, offset, axRenderDataTypeUtil::typeOf<T>());
}

template<class Obj, class T, axInt N> inline
void axRenderVertexDescData::addAttrElement(AttrId attrId, axFixedArray<T, N> Obj::*member, axInt elementIndex) {
	auto offset = axPtrUtil::memberOffset(member, elementIndex);
	addAttr(attrId, offset, axRenderDataTypeUtil::typeOf<T>());
}
