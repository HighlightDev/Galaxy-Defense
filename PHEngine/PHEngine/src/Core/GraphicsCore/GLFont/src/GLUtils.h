#pragma once

#include <gl/glew.h>
#include <string>

class GLUtils {
public:
    GLUtils();
    ~GLUtils();

    static void loadShader(const std::string& shaderSource, GLenum shaderType, GLuint& programId);
};
