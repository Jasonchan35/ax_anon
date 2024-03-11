#pragma once

#include "axRenderDrawCall.h"
#include "axRenderComputeCall.h"
#include "mesh/axRenderMesh.h"
#include "buffer/axRenderMultiGpuBuffer.h"
#include "axRenderer.h"
#include "axRenderBuiltInAssets.h"

class axRenderContext;
class axRenderBuiltInAssets;

#define axRenderCommandType_EnumList(E) \
	E(Unknown,) \
	E(ClearColorAndDepthBuffers,) \
	E(SetViewport,) \
	E(SwapBuffers,) \
	E(DrawCall,) \
	E(ComputeCall,) \
	E(CopyToStageBuffer,) \
//----------
ax_ENUM_CLASS(axRenderCommandType, axInt32)

class axRenderCommand : public axSListNode<axRenderCommand>, public axRenderCommonBase {
public:
	virtual ~axRenderCommand() {}

	using Type = axRenderCommandType;

	axRenderCommand(Type type) : _type(type) {}
	Type type() const { return _type; }

private:
	Type	_type;
};

class axRenderCommand_Unknown : public axRenderCommand {
public:
	axRenderCommand_Unknown() : axRenderCommand(Type::Unknown) {}
};

class axRenderCommand_ClearColorAndDepthBuffers : public axRenderCommand {
public:
	axRenderCommand_ClearColorAndDepthBuffers() : axRenderCommand(Type::ClearColorAndDepthBuffers) {}
	axColor	color {1,0,1,1};
	float depth {0};
};

class axRenderCommand_SetViewport : public axRenderCommand {
public:
	axRenderCommand_SetViewport() : axRenderCommand(Type::SetViewport) {}
	axRect2f	viewport {0,0,0,0};
};

class axRenderCommand_SwapBuffers : public axRenderCommand {
public:
	axRenderCommand_SwapBuffers() : axRenderCommand(Type::SwapBuffers) {}
};

class axRenderCommand_DrawCall : public axRenderCommand {
public:
	axRenderCommand_DrawCall() : axRenderCommand(Type::DrawCall) {}
	axRenderDrawCall drawCall;
};

class axRenderCommand_ComputeCall : public axRenderCommand {
public:
	axRenderCommand_ComputeCall() : axRenderCommand(Type::ComputeCall) {}
	axRenderComputeCall computeCall;
};

class axRenderCommand_CopyToStageBuffer : public axRenderCommand {
public:
	axRenderCommand_CopyToStageBuffer() : axRenderCommand(Type::CopyToStageBuffer) {}
	axSPtr<StageBuffer>		dst;
	axSPtr<StorageBuffer>	src;
};
