#pragma once

#if AX_USE_OPENGL

#include "../../buffer/axRenderMultiGpuBuffer.h"
#include "axGLRenderer.h"

class axGLGpuBuffer : public axRenderGpuBuffer {
	AX_RTTI_CLASS(axGLGpuBuffer, axRenderGpuBuffer)
public:
	using Util = axGLUtil;

	~axGLGpuBuffer() { destroy(); }

	void bind	() { glBindBuffer(_target, _handle); }
	void unbind	() { glBindBuffer(_target,  0); }

	virtual void onCreate(const CreateDesc& desc, axInt bufferSize) final;
	virtual void onUploadToGpu(axInt offset, axByteSpan data) final;

	bool isValid() const { return _handle != 0; }
	GLuint handle() { return _handle; }

	void* map(axInt offset, axInt size, GLbitfield access);
	void unmap();

private:
	void destroy();

	GLenum _target = 0;
	GLuint _handle = 0;
};

#endif //AX_USE_OPENGL
