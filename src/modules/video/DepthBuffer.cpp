/**
 * @file
 */

#include "DepthBuffer.h"
#include "GLFunc.h"
#include "ScopedFrameBuffer.h"
#include "Texture.h"

#include <cstddef>
#include "core/Common.h"

namespace video {

DepthBuffer::DepthBuffer() :
		_depthTexture(TextureType::Texture2DArray, TextureFormat::D24S8, "**depthmap**", 1, 1, 1, TextureWrap::ClampToEdge) {
}

DepthBuffer::~DepthBuffer() {
	core_assert_msg(_fbo == 0u, "Depthbuffer was not properly shut down");
	shutdown();
}

void DepthBuffer::shutdown() {
	if (_fbo != 0) {
		glDeleteFramebuffers(1, &_fbo);
		_fbo = 0;
	}

	_depthTexture.shutdown();

	if (_rbo != 0) {
		glDeleteRenderbuffers(1, &_rbo);
		_rbo = 0;
	}

	core_assert(_oldFramebuffer == -1);
}

bool DepthBuffer::init(const glm::ivec2& dimension, DepthBufferMode mode, int textureCount) {
	if (textureCount > 4 || textureCount < 1) {
		Log::error("Invalid texture count for depthbuffer: %i", textureCount);
		return false;
	}
	_mode = mode;

	TextureFormat format;
	if (depthAttachment()) {
		if (_mode == DepthBufferMode::DEPTH_CMP) {
			const TextureType type = textureType();
			const GLenum glType = std::enum_value(type);
			glBindTexture(glType, _depthTexture);
			glTexParameteri(glType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(glType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
		format = TextureFormat::D24S8;
	} else {
		format = TextureFormat::RGBA;
	}
	_depthTexture.upload(format, dimension.x, dimension.y, nullptr, textureCount);

	glGenFramebuffers(1, &_fbo);
	GL_setName(GL_FRAMEBUFFER, _fbo, "depthfbo");
	ScopedFrameBuffer scopedFrameBuffer(_fbo);
	if (depthAttachment()) {
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	GL_checkError();

	if (!depthAttachment()) {
#if 0
		glGenRenderbuffers(1, &_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, dimension.x, dimension.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);
#endif

		const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(SDL_arraysize(drawBuffers), drawBuffers);
	}

#if 0
	const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			Log::error("FB error, incomplete attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			Log::error("FB error, incomplete missing attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			Log::error("FB error, incomplete draw buffer");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			Log::error("FB error, incomplete read buffer");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			Log::error("FB error, framebuffer unsupported");
			break;
		default:
			Log::error("FB error, status: %i", (int)status);
			break;
		}
		return false;
	}
#endif
	return true;
}

bool DepthBuffer::bind() {
	core_assert(_oldFramebuffer == -1);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFramebuffer);
	GL_checkError();

	glGetIntegerv(GL_VIEWPORT, _oldViewport);
	const glm::ivec2& dim = dimension();
	glViewport(0, 0, dim.x, dim.y);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	return true;
}

bool DepthBuffer::bindTexture(bool read, int textureIndex) {
	core_assert(textureIndex >= 0 && textureIndex < 4);
	if (textureIndex < 0 || textureIndex >= 4) {
		return false;
	}
	if (depthAttachment()) {
		if (!read) {
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexture, 0, textureIndex);
		}
		glClear(GL_DEPTH_BUFFER_BIT);
	} else {
		if (!read) {
			glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _depthTexture, 0, textureIndex);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	GL_checkError();
	return true;
}

void DepthBuffer::unbind() {
	glViewport(_oldViewport[0], _oldViewport[1], (GLsizei)_oldViewport[2], (GLsizei)_oldViewport[3]);
	core_assert(_oldFramebuffer != -1);
	glBindFramebuffer(GL_FRAMEBUFFER, _oldFramebuffer);
	_oldFramebuffer = -1;
}

}
