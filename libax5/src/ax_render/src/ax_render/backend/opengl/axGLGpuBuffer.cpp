#if AX_USE_OPENGL

#include "axGLGpuBuffer.h"
#include "axGLRenderContext.h"
#include "axGLRenderer.h"

void axGLGpuBuffer::onCreate(const CreateDesc& desc, axInt bufferSize) {
	destroy();

	GLenum usage = GL_DYNAMIC_DRAW;

	switch (desc.type) {
		case Type::Vertex:	_target = GL_ARRAY_BUFFER;			break;
		case Type::Index:	_target = GL_ELEMENT_ARRAY_BUFFER;	break;
		case Type::Const:	_target = GL_UNIFORM_BUFFER;		break;
		case Type::Storage:	{
			#if 0 // require GL4.5
				glCreateBuffers(1, &_handle);
				glNamedBufferStorage(_handle, bufferSize, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
				return;
			#else
				_target = GL_ARRAY_BUFFER;
			#endif
		}break;

		case Type::Stage: {
			#if 0 // require GL4.5
				glCreateBuffers(1, &_handle);
				glNamedBufferStorage(_handle, bufferSize, nullptr, GL_MAP_READ_BIT);
				return;
			#else
				_target = GL_UNIFORM_BUFFER;
				usage = GL_DYNAMIC_READ;
			#endif
		}break;

		default: throw axError_Undefined(AX_LOC);
	}

	glGenBuffers(1, &_handle);
	if (!_handle) throw axError_Undefined(AX_LOC); //error create

	bind();
	glBufferData(_target, bufferSize, nullptr, usage);
	unbind();
}

void* axGLGpuBuffer::map(axInt offset, axInt size, GLbitfield access) {
	bind();
	return glMapBufferRange(_target, offset, size, access);
}

void axGLGpuBuffer::unmap() {
	glUnmapBuffer(_target);
	unbind();
}

void axGLGpuBuffer::onUploadToGpu(axInt offset, axByteSpan data) {
//	glBufferSubData(_target, offset, data.sizeInBytes(), data.data());
	auto* p = map(offset, data.sizeInBytes(), GL_MAP_WRITE_BIT /* | GL_MAP_INVALIDATE_BUFFER_BIT */);
	if (!p) {
		Util::checkGLError(AX_LOC);
		return;
	}

	axByte* dst = reinterpret_cast<axByte*>(p);
	ax_memcpy(dst, data.data(), data.sizeInBytes());
	unmap();
}

void axGLGpuBuffer::destroy() {
	if (_handle) {
		glDeleteBuffers(1, &_handle);
		_handle = 0;
	}
}

#endif //AX_USE_OPENGL
