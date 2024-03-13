//
// Created by ana on 5.3.24..
//

#include "rg/Error.h"

namespace rg {

    void clearAllOpenGlErrors() {
        while (glGetError() != GL_NO_ERROR) {
            ;
        }
    }
    const char* openGLErrorToString(GLenum error) {
        switch(error) {
            case GL_NO_ERROR: return "GL_NO_ERROR";
            case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
            case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
            case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
            case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        }
        ASSERT(false, "Passed something that is not an error code");
        return "THIS_SHOULD_NEVER_HAPPEN";
    }
    bool wasPreviousOpenGLCallSuccessful(const char* file, int line, const char* call) {
        bool success = true;
        while (GLenum error = glGetError()) {
            std::cerr << "[OpenGL error] " << error << " " << openGLErrorToString(error)
                      << "\nFile: " << file
                      << "\nLine: " << line
                      << "\nCall: " << call
                      << "\n\n";
            success = false;
        }
        return success;
    }

}