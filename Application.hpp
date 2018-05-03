#pragma once

#include <glad/glad.h>

#include <string>

#include "mesh.hpp"
#include "program.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "font.hpp"

#include "Collisions"

class Application {
public:
    Application(int initial_window_width, int initial_window_height, const std::string& title)
        : window(initial_window_width, initial_window_height, title) {
        // Set callbacks
        window.set_user_pointer(this);
        window.set_key_callback(on_key);
        window.set_size_callback(on_resize);
        window.set_mouse_button_callback(on_mouse_button);
        window.set_mouse_position_callback(on_mouse_position);
    }

    void init();
    void step();
    void render();
    void gui();

    void on_key(int key, int scancode, int actions, int mods);
    void on_mouse_position(double x, double y);
    void on_mouse_button(int button, int action, int mods);
    void on_resize(int width, int height);

    Window window;

private:
    std::unique_ptr<ShaderProgram> program;

    Camera camera;

    // Attr and uniform locations
    GLint color_loc = -1;
    GLint model_matrix_loc = -1;
    GLint view_matrix_loc = -1;
    GLint projection_matrix_loc = -1;
    GLint PVM_matrix_loc = -1;
    GLint normal_matrix_loc = -1;

    GLint material_ambient_color_loc = -1;
    GLint material_diffuse_color_loc = -1;
    GLint material_specular_color_loc = -1;
    GLint material_shininess_loc = -1;

    GLint obj_texture_loc = -1;

    GLint light_position_loc = -1;
    GLint light_diffuse_color_loc = -1;
    GLint light_ambient_color_loc = -1;
    GLint light_specular_color_loc = -1;

    GLint eye_position_loc = -1;

    // Textures
    GLuint t_bricks = 0;
    GLuint t_glass = 0;
    GLuint t_rune = 0;
    GLuint t_roof = 0;
    GLuint t_wood = 0;

    // Nuklear
    struct nk_context* ctx;
    struct nk_font_atlas* atlas;

    // Meshes
    Mesh cube = Mesh::cube();
    Mesh sphere = Mesh::sphere();
    Mesh ground = Mesh::ground();
    Mesh pad = Mesh::pad();
    Mesh brick = Mesh::brick();

    // Colliders
    std::vector<Collisions::BallCollider> balls;
    std::vector<Collisions::BrickCollider> pads;
    std::vector<Collisions::BrickCollider> bricks;
    Collisions::BoundsCollider bounds = { radius };

    // Player input
    float movement = 0.f;
    float movement_speed = 0.05f;

    void draw_object(const Mesh& mesh, const Collisions::Collider& collider);

    static void on_key(GLFWwindow* window, int key, int scancode, int actions, int mods) {
        Application* this_pointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_pointer->on_key(key, scancode, actions, mods);
    }

    static void on_mouse_position(GLFWwindow* window, double x, double y) {
        Application* this_pointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_pointer->on_mouse_position(x, y);
    }

    static void on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
        Application* this_pointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_pointer->on_mouse_button(button, action, mods);
    }

    static void on_resize(GLFWwindow* window, int width, int height) {
        Application* this_pointer = static_cast<Application*>(glfwGetWindowUserPointer(window));
        this_pointer->on_resize(width, height);
    }
};
