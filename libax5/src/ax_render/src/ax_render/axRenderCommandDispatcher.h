#pragma once

#include "axRenderCommand.h"
#include "axRenderDrawCall.h"
#include "axRenderContext.h"

class axRenderCommandDispatcher : public axTypedBase, public axNonCopyable, public axRenderCommonBase {
	AX_RTTI_CLASS(axRenderCommandDispatcher, axTypedBase)
public:
	using Rect2			= axRect2f;
	using Vec2			= axVec2f;

	axRenderCommandDispatcher(axRenderContext* renderContext, axRenderRequest* renderReq_);
	virtual ~axRenderCommandDispatcher();

	void uploadTexture2D(axTexture2D_UploadRequest& req) { onUploadTexture2D(req); }
	virtual void onUploadTexture2D(axTexture2D_UploadRequest& req) = 0;

	virtual void onGpuFrameBegin();
	virtual void onGpuFrameEnd();

	void cmdUnknown			(axRenderCommand_Unknown& cmd) { AX_ASSERT(false); }

	axRenderContext*	renderContextBase = nullptr;
	axRenderer*			renderer = nullptr;

	axInt64				renderSeqId() const { return _renderSeqId; }

protected:
	axInt64		_renderSeqId = 0;
	axRenderRequest* _renderReqBase = nullptr;
	bool		_onGpuFrameBeginCalled : 1;
	bool		_onGpuFrameEndCalled : 1;
};

template<class IMPL>
class axRenderCommandDispatcherImpl : public axRenderCommandDispatcher {
	AX_RTTI_CLASS(axRenderCommandDispatcherImpl<IMPL>, axRenderCommandDispatcher)
public:
	axRenderCommandDispatcherImpl(axRenderContext* renderContext, axRenderRequest* renderReq) 
		: Base(renderContext, renderReq)
	{
	}

	void dispatch() {
		onGpuFrameBegin();
		_dispatch(_renderReqBase->computeCommandQueue());
		_dispatch(_renderReqBase->commandQueue());
		onGpuFrameEnd();
	}

private:
	void _dispatch(axSList<axRenderCommand>& commandQueue) {
		AX_ZoneScoped;

		using Type = axRenderCommandType;
		IMPL* impl = static_cast<IMPL*>(this);

		for (auto& cmd : commandQueue) {
			switch (cmd.type()) {
				#define E(CMD, ...) \
					case Type::CMD: { \
						AX_ZoneScopedN(#CMD); \
						impl->cmd##CMD(*static_cast<axRenderCommand_##CMD*>(&cmd)); \
					} break; \
				//--------
					axRenderCommandType_EnumList(E)
				#undef E

				default: {
					AX_ASSERT_MSG(false, "unhandled render command");
				}break;
			}
		}
	}
};

#define axRenderGpuRequestImpl_COMMAND_FUNCTIONS \
	void cmdSwapBuffers					(axRenderCommand_SwapBuffers&				cmd); \
	void cmdClearColorAndDepthBuffers	(axRenderCommand_ClearColorAndDepthBuffers&	cmd); \
	void cmdSetViewport					(axRenderCommand_SetViewport&				cmd); \
	void cmdDrawCall					(axRenderCommand_DrawCall&					cmd); \
	void cmdComputeCall					(axRenderCommand_ComputeCall&				cmd); \
	void cmdCopyToStageBuffer			(axRenderCommand_CopyToStageBuffer&			cmd); \
//--------
