#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>

#include <cstddef>

using GlGenBuffersProc = void (APIENTRY*)(GLsizei, GLuint*);
using GlBindBufferProc = void (APIENTRY*)(GLenum, GLuint);
using GlBufferDataProc = void (APIENTRY*)(GLenum, ptrdiff_t, const void*, GLenum);
using GlDeleteBuffersProc = void (APIENTRY*)(GLsizei, const GLuint*);

#ifndef GL_ARRAY_BUFFER
constexpr GLenum GL_ARRAY_BUFFER = 0x8892;
#endif

#ifndef GL_STATIC_DRAW
constexpr GLenum GL_STATIC_DRAW = 0x88E4;
#endif
