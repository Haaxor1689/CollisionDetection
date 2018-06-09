#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(int initialWidth, int initialHeight, const std::string& title);
    ~Window();

    void SetUserPointer(void* pointer) const;

    void SetPosCallback(GLFWwindowposfun function) const;
    void SetSizeCallback(GLFWwindowsizefun function) const;
    void SetCloseCallback(GLFWwindowclosefun function) const;

    void SetKeyCallback(GLFWkeyfun function) const;
    void SetMousePositionCallback(GLFWcursorposfun function) const;
    void SetMouseButtonCallback(GLFWmousebuttonfun function) const;

    bool ShouldClose() const;
    void SwapBuffers() const;
    void PollEvents();

    void Resize(int width, int height);

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    GLFWwindow* GetWindow() const { return window; }

private:
    int width;
    int height;

    GLFWwindow* window;
};
