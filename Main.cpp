#include <fstream>
#include <iostream>
#include <memory>

#include "Application.hpp"

// Callback function to be called when OpenGL emits an error
void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                    const char* message, const void* user_parameter);

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    std::unique_ptr<Application> application = std::make_unique<Application>(1280, 760, "Seminar 03 Final");

    if (GLAD_GL_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(opengl_debug_callback, nullptr);
    }

    try {
        application->Init();
    } catch (std::string error) {
        std::cout << error << std::endl;

        application.reset();
        glfwTerminate();
        return -1;
    }

    // Loop
    while (!application->Window.ShouldClose()) {
        application->Step();
        application->Gui();
        application->Render();
        application->Window.SwapBuffers();
        application->Window.PollEvents();
    }

    // Free the entire application before terminating glfw. If this were done in the wrong order
    // application may crash on calling OpenGL (Delete*) calls after destruction of a context
    application.reset();

    // Free every allocated resource associated with GLFW
    glfwTerminate();

    return 0;
}

void APIENTRY opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                    const char* message, const void* user_parameter) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << message << std::endl;
        return;
    default:
        return;
    }
}
