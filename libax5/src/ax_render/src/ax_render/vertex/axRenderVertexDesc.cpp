#include "axRenderVertexDesc.h"

axRenderVertexDescData::axRenderVertexDescData() {
	for (axInt i = 0; i < allAttrs.size(); i++) {
		allAttrs[i].attrId = AttrId(static_cast<axEnum<AttrId>::IntType>(i));
	}
}

void axRenderVertexDescData::addAttr(AttrId attrId, axInt offset, DataType dataType_) {
	if (dataType_ == DataType::Unknown) {
		throw axError_Undefined(AX_LOC);
	}

	auto& a = allAttrs[ax_enum_int(attrId)];
	if (a.active) {
		throw axError_Undefined(AX_LOC);
		return;
	}

	ax_safe_assign(a.offset, offset);
	a.sizeInBytes = axRenderDataTypeUtil::sizeInBytes(dataType_);
	a.dataType = dataType_;
	a.active   = true;
	activeAttrs.append(&a);
}

void axRenderVertexDescData::onFormat(axFormat& f) const {
	f.format("vertexDesc {?}", name);
	f.format("\n  strideBytes={?}", strideBytes);
	for (auto* attr : activeAttrs) {
		f << "\n   " << *attr;
	}
}

void axRenderVertexDescData::Attr::onFormat(axFormat& f) const {
	f.format("attrId={?} offset={?} dataType={?}", attrId, offset, dataType);
}
