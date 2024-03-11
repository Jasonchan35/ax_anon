#pragma once

namespace Anon { namespace Editor {

class EdUIDiaConnection : public axUIDiaConnection {
	AX_RTTI_CLASS(EdUIDiaConnection, axUIDiaConnection);
public:
	EdUIDiaConnection(axUIDiaSocket* parent, axUIDiaSocket* target);

	virtual void onRender(axRenderRequest& req) override;

	DiaConnection* diaConnection() { return linkedObject<DiaConnection>(); }
};

}}