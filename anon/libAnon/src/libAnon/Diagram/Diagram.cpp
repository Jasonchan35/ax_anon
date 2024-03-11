#include "Diagram.h"
#include "DiaSystem.h"
#include "DiaConnection.h"

namespace Anon {

Diagram::Diagram(IDiaSystem* system, DiaGroup* parent)
	: Base(system, parent)
{
	_diagram = this;
}

void Diagram::onSetComputeNeeded() {
	Base::onSetComputeNeeded();
	if (_owner) _owner->setComputeNeeded();
}

bool Diagram::onReadJsonProp(axJsonReader& rd) {
	bool ret = Base::onReadJsonProp(rd);

	if (rd.beginArray("connections")) {
		while( !rd.endArray()) {

			axTempString from, to;

			rd.beginObject();
			while (!rd.endObject()) {
				if (rd.readMember("from", from)) continue;
				if (rd.readMember("to", to)) continue;
			}

			auto src = from.splitByChar('.');
			auto dst = to.splitByChar('.');

			onAcceptConnection(src.first, src.second, dst.first, dst.second);
		}
		ret = true;
	}
	return ret;
}

void Diagram::onWriteJsonProp(axJsonWriter& wr) const {
	Base::onWriteJsonProp(wr);

	{
		auto connections_scope = wr.array("connections");

		for (auto& node : nodes()) {
			for (auto& inPlug : node.connNode.inSock.plugs()) {
				auto& outPlug = inPlug.outPlug();

				auto obj_scope = wr.object();

				axTempString from, to;

				inPlug.getString(to);
				outPlug.getString(from);

				wr.writeMember("from", from);
				wr.writeMember("to",   to);
			}
		}
	}
}

DiaConnection* Diagram::onAcceptConnection(axStrView srcNode_, axStrView srcField_, axStrView dstNode_, axStrView dstField_) {
//	AX_LOG("connect {?}.{?} -> {?}.{?}", srcNode_, srcField_, dstNode_, dstField_);

	auto* srcNode = findNode(srcNode_);
	if (!srcNode) return nullptr;

	auto* dstNode = findNode(dstNode_);
	if (!dstNode) return nullptr;

	auto srcPath = srcNode->getFieldPath(srcField_);
	if (!srcPath) return nullptr;

	auto dstPath = dstNode->getFieldPath(dstField_);
	if (!dstPath) return nullptr;

	dstNode->connNode.inSock.disconnect(dstPath);

	auto conn = axUPtr_new(DiaConnection);
	conn->outPlug.connect(&srcNode->connNode.outSock, srcPath);
	conn->inPlug.connect( &dstNode->connNode.inSock,  dstPath);

	dstNode->setComputeNeeded();

	return conn.detach();
}


} //namespace