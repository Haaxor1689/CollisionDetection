#include "Application.hpp"

#include <iostream>
#include <random>

#include "Geometry"

using namespace std;

void Application::init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Create shader program
    program = make_unique<ShaderProgram>("shaders/main.vert", "shaders/main.frag");
    program->use();

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

    auto load_and_set_texture = [](const std::string& name, GLuint& pos) {
        pos = load_texture_2d("images/" + name + ".png");
        glBindTexture(GL_TEXTURE_2D, pos);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    };

    load_and_set_texture("bricks", t_bricks);
    load_and_set_texture("glass", t_glass);
    load_and_set_texture("roof", t_roof);
    load_and_set_texture("rune", t_rune);
    load_and_set_texture("wood", t_wood);

    // Materials
    glUniform3f(material_ambient_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(material_diffuse_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(material_specular_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform1f(material_shininess_loc, 200.0f);

    // Texture
    glUniform1i(obj_texture_loc, 0);
    glActiveTexture(GL_TEXTURE0);

    cube.create_vao(position_loc, normal_loc, tex_coord_loc);
    sphere.create_vao(position_loc, normal_loc, tex_coord_loc);
    ground.create_vao(position_loc, normal_loc, tex_coord_loc);
    pad.create_vao(position_loc, normal_loc, tex_coord_loc);
    brick.create_vao(position_loc, normal_loc, tex_coord_loc);

    cube.set_texture(t_roof);
    sphere.set_texture(t_glass);
    ground.set_texture(t_bricks);
    pad.set_texture(t_wood);
    brick.set_texture(t_rune);

    // Objects
    std::random_device r;
    std::default_random_engine e(r());
    std::uniform_real_distribution<float> pos(-15.f, 15.f);
    std::uniform_real_distribution<float> vel(-0.5f, 0.5f);

    for (unsigned i = 0; i < 1; ++i) {
        balls.emplace_back(Geometry::Vector<3>{ pos(e), 1.f, pos(e) }, Geometry::Vector<3>{ vel(e), 0.f, vel(e) }, 1.f);
    }

    pads.emplace_back(radius * 3.f / 4.f, segments / 5.f, 0.f);
    // pads.emplace_back(radius * 3.f / 4.f, segments / 5.f, 2.f * Geometry::Pi() / 3.f);
    // pads.emplace_back(radius * 3.f / 4.f, segments / 5.f, 4.f * Geometry::Pi() / 3.f);

    // for (float i = 0; i < 4.f; ++i) {
    //     float offset = i * 1.9f;
    //     float height = i * brick_height;
    //     bricks.emplace_back(radius / 4.f, segments / 6.f, 0.f + offset, height);
    //     bricks.emplace_back(radius / 4.f, segments / 6.f, 2.f * Geometry::Pi() / 5.f + offset, height);
    //     bricks.emplace_back(radius / 4.f, segments / 6.f, 4.f * Geometry::Pi() / 5.f + offset, height);
    //     bricks.emplace_back(radius / 4.f, segments / 6.f, 6.f * Geometry::Pi() / 5.f + offset, height);
    //     bricks.emplace_back(radius / 4.f, segments / 6.f, 8.f * Geometry::Pi() / 5.f + offset, height);
    // }
}

void Application::step() {
    for (auto& ball : balls) {
        ball.Step();
    }

    for (auto& ball : balls) {
        ball.Collision(bounds);
        for (auto& pad : pads) {
            ball.Collision(pad);
        }
        for (auto& brick : bricks) {
            ball.Collision(brick);
        }
        for (auto& otherBall : balls) {
            ball.Collision(otherBall);
        }
    }

    for (auto& pad : pads) {
        pad.Rotate(movement);
    }
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

    // Set cameras projection and view matrices and eye location
    auto projection_matrix = Geometry::Perspective(45.0f, aspect_ratio, 0.1f, 100.0f);
    auto view_matrix = Geometry::LookAt(camera.get_eye_position(), Geometry::Vector<3>(), { 0.0f, 1.0f, 0.0f });

    glUniformMatrix4fv(projection_matrix_loc, 1, GL_FALSE, &projection_matrix);
    glUniformMatrix4fv(view_matrix_loc, 1, GL_FALSE, &view_matrix);
    glUniform3fv(eye_position_loc, 1, &camera.get_eye_position());

    // Set light
    auto light_pos = Geometry::Vector<3>{ 5.f / 4.f + 0.75f, 1.f, 0.f }.Rotate(app_time, { 0.f, 1.f, 0.f });
    glUniform4f(light_position_loc, light_pos.x(), light_pos.y(), light_pos.z(), 0.f);

    // Colors
    glUniform3f(light_ambient_color_loc, 0.1f, 0.1f, 0.1f);
    glUniform3f(light_diffuse_color_loc, 1.0f, 1.0f, 1.0f);
    glUniform3f(light_specular_color_loc, 1.0f, 1.0f, 1.0f);

    // Model matrix
    auto model_matrix = Geometry::Matrix<4>::Identity();
    glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, &model_matrix);

    ground.draw();

    for (auto& ball : balls) {
        draw_object(sphere, ball);
    }

    for (auto& pad_col : pads) {
        draw_object(pad, pad_col);
    }

    for (auto& brick_col : bricks) {
        draw_object(brick, brick_col);
    }
}

void Application::draw_object(const Mesh& mesh, const Collisions::Collider& collider) {
    auto model_matrix = Geometry::Matrix<4>::Identity();
    class Visitor : public Collisions::ConstColliderVisitor {
    public:
        explicit Visitor(Geometry::Matrix<4>& model_matrix)
            : model_matrix(model_matrix) {}

        void operator()(const Collisions::BallCollider& col) override {
            model_matrix.Translate(col.Position());
        }

        void operator()(const Collisions::BrickCollider& col) override {
            model_matrix
                .Rotate(col.AngleStart(), Geometry::Vector<3>{ 0.f, 1.f, 0.f })
                .Translate(Geometry::Vector<3>{ 0.f, col.Height(), 0.f });
        }

        Geometry::Matrix<4>& model_matrix;
        bool stopThere = false;
    } visitor{ model_matrix };
    collider.visit(visitor);

    glUniformMatrix4fv(model_matrix_loc, 1, GL_FALSE, &model_matrix);
    mesh.draw();
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
        return;
    case GLFW_KEY_F:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        return;
    case GLFW_KEY_A:
        if (actions == GLFW_PRESS) {
            movement -= movement_speed;
        } else if (actions == GLFW_RELEASE) {
            movement += movement_speed;
        }
        return;
    case GLFW_KEY_D:
        if (actions == GLFW_PRESS) {
            movement += movement_speed;
        } else if (actions == GLFW_RELEASE) {
            movement -= movement_speed;
        }
        return;
    }
}

void Application::on_resize(int width, int height) {
    window.resize(width, height);
    glViewport(0, 0, width, height);
}
