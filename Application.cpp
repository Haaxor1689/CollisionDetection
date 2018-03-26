#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;

void Application::init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);

    // Create shader program
    program = make_unique<ShaderProgram>("shaders/main.vert", "shaders/main.frag");

    // Get locations of vertex attributes position and normal
    GLint position_loc = program->get_attribute_location("position");
    GLint normal_loc = program->get_attribute_location("normal");
    GLint tex_coord_loc = program->get_attribute_location("tex_coord");

    // Get locations of uniforms for positioning and projecting object
    model_matrix_loc = program->get_uniform_location("model_matrix");
    view_matrix_loc = program->get_uniform_location("view_matrix");
    projection_matrix_loc = program->get_uniform_location("projection_matrix");

    light_position_loc = program->get_uniform_location("light_position");
    eye_position_loc = program->get_uniform_location("eye_position");
    light_ambient_color_loc = program->get_uniform_location("light_ambient_color");
    light_diffuse_color_loc = program->get_uniform_location("light_diffuse_color");
    light_specular_color_loc = program->get_uniform_location("light_specular_color");

    material_ambient_color_loc = program->get_uniform_location("material_ambient_color");
    material_diffuse_color_loc = program->get_uniform_location("material_diffuse_color");
    material_specular_color_loc = program->get_uniform_location("material_specular_color");
    material_shininess_loc = program->get_uniform_location("material_shininess");

    obj_texture_loc = program->get_uniform_location("obj_texture");

    cube.create_vao(position_loc, normal_loc, tex_coord_loc);
    sphere.create_vao(position_loc, normal_loc, tex_coord_loc);
    teapot.create_vao(position_loc, normal_loc, tex_coord_loc);

    texture_01 = load_texture_2d("images/rocks.png");
    glBindTexture(GL_TEXTURE_2D, texture_01);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::render() {
    // Get the current time
    float app_time = float(glfwGetTime());

    // Get the aspect ratio of window size
    float width = float(window.get_width());
    float height = float(window.get_height());
    float aspect_ratio = width / height;

    // Clear screen, both color and depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind(use) our program
    program->use();

    // Set cameras projection and view matrices and eye location
    glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 100.0f);
    glm::mat4 view_matrix = glm::lookAt(camera.get_eye_position(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniform3fv(eye_position_loc, 1, glm::value_ptr(camera.get_eye_position()));

    // Set light
    glm::vec3 light_pos = glm::rotate(glm::vec3(0.f, std::sin(app_time * 2), 2.f), app_time, glm::vec3(0.f, 1.f, 0.f));
    // Position
    // W = 0.0 for directional, W = 1.0 for point
    glUniform4f(light_position_loc, light_pos.x, light_pos.y, light_pos.z, 1.0f);

    // Colors
    glUniform3f(light_ambient_color_loc, 0.05f, 0.05f, 0.05f);
    glUniform3f(light_diffuse_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(light_specular_color_loc, 1.0f, 1.0f, 1.0f);

    sphere.bind_vao();

    // Model matrix
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, glm::value_ptr(model_matrix));

    // Materials
    glUniform3f(material_ambient_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(material_diffuse_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(material_specular_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform1f(material_shininess_loc, 200.0f);

    // Texture
    glUniform1i(obj_texture_loc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_01);

    sphere.draw();
}

void Application::on_mouse_position(double x, double y) {
    camera.on_mouse_move(x, y);
}
void Application::on_mouse_button(int button, int action, int mods) {
    camera.on_mouse_button(button, action, mods);
}

void Application::on_key(int key, int scancode, int actions, int mods) {
    switch (key) {
    case GLFW_KEY_L:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case GLFW_KEY_F:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }
}

void Application::on_resize(int width, int height) {
    window.resize(width, height);

    // Set the area into which we render
    glViewport(0, 0, width, height);
}
