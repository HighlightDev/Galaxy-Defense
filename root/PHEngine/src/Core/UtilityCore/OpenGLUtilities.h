#pragma once

#include <gl/glew.h>

#include <iostream>

void CheckGLError(const char* stmt, const char* fname, int line)
{
    GLenum err = glGetError();
    while (err != GL_NO_ERROR) {
        const char* error;
        switch (err) {
        case GL_INVALID_ENUM:
            error = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "GL_INVALID_OPERATION";
            break;
        case GL_STACK_OVERFLOW:
            error = "GL_STACK_OVERFLOW";
            break;
        case GL_STACK_UNDERFLOW:
            error = "GL_STACK_UNDERFLOW";
            break;
        case GL_OUT_OF_MEMORY:
            error = "GL_OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        default:
            error = "Unknown Error";
            break;
        }
        std::cerr << "OpenGL error " << error << " (" << err << "), at " << fname << ":" << line << " - for " << stmt
                  << std::endl;
        err = glGetError();
    }
}

#define GL_CHECK(stmt)                                                                                                           \
    do {                                                                                                                         \
        stmt;                                                                                                                    \
        CheckGLError(#stmt, __FILE__, __LINE__);                                                                                 \
    } while (0)
