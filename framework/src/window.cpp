#include "window.hpp"

Window::Window(int initialWidth, int initialHeight, const std::string& title)
    : width(initialWidth), height(initialHeight) {
    // Before creating the Window, request OpenGL 3.3 Core context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    window = glfwCreateWindow(initialWidth, initialHeight, title.c_str(), nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Could not create window!");
    }

    // Make window's context current
    glfwMakeContextCurrent(window);

    // Load OpenGL functions
    if (!gladLoadGL()) {
        throw std::runtime_error("Could not initialize OpenGL functions!");
    }
}

void Window::SetUserPointer(void* pointer) const {
    glfwSetWindowUserPointer(window, pointer);
}

void Window::SetPosCallback(GLFWwindowposfun function) const {
    glfwSetWindowPosCallback(window, function);
}

void Window::SetSizeCallback(GLFWwindowsizefun function) const {
    glfwSetWindowSizeCallback(window, function);
}

void Window::SetCloseCallback(GLFWwindowclosefun function) const {
    glfwSetWindowCloseCallback(window, function);
}

void Window::SetKeyCallback(GLFWkeyfun function) const {
    glfwSetKeyCallback(window, function);
}
void Window::SetMousePositionCallback(GLFWcursorposfun function) const {
    glfwSetCursorPosCallback(window, function);
}
void Window::SetMouseButtonCallback(GLFWmousebuttonfun function) const {
    glfwSetMouseButtonCallback(window, function);
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(window) != 0;
}

void Window::Close() const {
    glfwSetWindowShouldClose(window, GL_TRUE);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::Resize(int width, int height) {
    width = width;
    height = height;
}

Window::~Window() {
    glfwDestroyWindow(window);
}
