#pragma once

#include <glad/glad.h>

#include <string>

#include "mesh.hpp"
#include "program.hpp"
#include "window.hpp"
#include "camera.hpp"

#include "Collisions"

class Application {
public:
    Application(int initialWindowWidth, int initialWindowHeight, const std::string& title)
        : Window(initialWindowWidth, initialWindowHeight, title) {
        // Set callbacks
        Window.SetUserPointer(this);
        Window.SetKeyCallback(OnKey);
        Window.SetSizeCallback(OnResize);
        Window.SetMouseButtonCallback(OnMouseButton);
        Window.SetMousePositionCallback(OnMousePosition);
    }

    void Init();
    void Step();
    void Render();
    void Gui();

    void OnKey(int key, int scancode, int actions, int mods);
    void OnMousePosition(double x, double y);
    void OnMouseButton(int button, int action, int mods);
    void OnResize(int width, int height);

    Window Window;

private:
    std::unique_ptr<ShaderProgram> program{};

    Camera camera;

    // Attr and uniform locations
    GLint modelMatrixLoc = -1;
    GLint viewMatrixLoc = -1;
    GLint projectionMatrixLoc = -1;

    GLint materialAmbientColorLoc = -1;
    GLint materialDiffuseColorLoc = -1;
    GLint materialSpecularColorLoc = -1;
    GLint materialShininessLoc = -1;

    GLint objTextureLoc = -1;

    GLint lightPositionLoc = -1;
    GLint lightDiffuseColorLoc = -1;
    GLint lightAmbientColorLoc = -1;
    GLint lightSpecularColorLoc = -1;

    GLint eyePositionLoc = -1;

    // Textures
    GLuint tBricks = 0;
    GLuint tGlass = 0;
    GLuint tRune = 0;
    GLuint tRoof = 0;
    GLuint tWood = 0;

    // Nuklear
    struct nk_context* ctx = nullptr;
    struct nk_font_atlas* atlas = nullptr;

    // Meshes
    Mesh cube = Mesh::Cube();
    Mesh sphere = Mesh::Sphere();
    Mesh ground = Mesh::Ground();
    Mesh pad = Mesh::Pad();
    Mesh brick = Mesh::Brick();

    // Colliders
    std::vector<Collisions::BallCollider> balls{};
    std::vector<Collisions::BrickCollider> pads{};
    std::vector<Collisions::BrickCollider> bricks{};
    Collisions::BoundsCollider bounds = { RADIUS };

    // Player input
    float movement = 0.f;
    float movementSpeed = 0.05f;

    void DrawObject(const Mesh& mesh, const Collisions::Collider& collider) const;

    static void OnKey(GLFWwindow* window, int key, int scancode, int actions, int mods) {
        auto thisPointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        thisPointer->OnKey(key, scancode, actions, mods);
    }

    static void OnMousePosition(GLFWwindow* window, double x, double y) {
        auto thisPointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        thisPointer->OnMousePosition(x, y);
    }

    static void OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
        auto thisPointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        thisPointer->OnMouseButton(button, action, mods);
    }

    static void OnResize(GLFWwindow* window, int width, int height) {
        auto thisPointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        thisPointer->OnResize(width, height);
    }
};
