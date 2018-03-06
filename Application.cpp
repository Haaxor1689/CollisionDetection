#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

using namespace std;

void Application::init() {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0);

  // TASK 5: Enable depth test
  // glEnable(GL_DEPTH_TEST);

  // Create shader program
  program = make_unique<ShaderProgram>("shaders/main.vert", "shaders/main.frag");

  // Get locations of vertex attributes position and normal
  GLint position_loc = program->get_attribute_location("position");
  GLint normal_loc = program->get_attribute_location("normal");

  // Get locations of uniforms for positioning and projecting object
  model_matrix_loc = program->get_uniform_location("model_matrix");
  view_matrix_loc = program->get_uniform_location("view_matrix");
  projection_matrix_loc = program->get_uniform_location("projection_matrix");

  // Get location of color uniform
  color_loc = program->get_uniform_location("color");

  cube.create_vao(position_loc, normal_loc);
  sphere.create_vao(position_loc, normal_loc);
  teapot.create_vao(position_loc, normal_loc);
}

void Application::render() {
  // Get the current time
  float time = float(glfwGetTime());

  // Get the aspect ratio of window size
  float width = float(window.get_width());
  float height = float(window.get_height());
  float aspect_ratio = width / height;

  // Clear screen, both color and depth
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Bind(use) our program
  program->use();

  // TASK 1: create and set projection matrix
  //          use glm::perspective(field of view, aspect, near, far)
  //          field of view - half angle of camera view in radians (about 45 degrees is fine)
  //          aspect - ratio of screen size: width / height
  //          near - distance from camera from where we should see the scene (for example 0.1f)
  //          far - distance from camera where we should not see anything (for example 100.0f)
  //          use glm::radians(degrees) to convert degrees to radians
  // glm::mat4 projection_matrix = ...;

  // TASK 1: Upload projection matrix to the shader
  //         use glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, glm::value_ptr(matrix))
  //         go to shader main.vert and finish the task there
  // ...

  // TASK 2: create and set view matrix
  //         use glm::lookAt(eye position, center of view, up vector of space) function
  //         choose point of view such that you can see entire cube
  //         use glm::vec3(0.0f, 0.0f, 0.0f) as a center for simplicity
  //         use glm::vec3(0.0f, 1.0f, 0.0f) - Y axis pointing up - as up vector
  //         go to main.vert again and add multiplication using view matrix
  // ...

  // Draw cube
  cube.bind_vao();
  glUniform3f(color_loc, 1.0f, 1.0f, 1.0f);

  // TASK 3: rotate cube using glm::rotate(existing transformation matrix,
  //                                       rotation in angles,
  //                                       vector to rotate around)
  //         you can simply use time as an angle
  // glm::mat4 cube_model_matrix = glm::mat4(1.0);
  // ...

  cube.draw();

  // TASK 4: draw another object, for example teapot or sphere
  // TASK 5: observe overlapping of erroneous overlapping of objects polygons
  //         you should fix this by enabling depth buffer usage in init function

  // (BONUS)
  // - draw more objects(using for loop)
  // - animate color uniform
  // - animate different transformations
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
