#include "Application.hpp"

#include <iostream>
#include <random>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "Geometry"
#include <texture.hpp>

using namespace std;

void Application::Init() {
    // Create shader program
    program = make_unique<ShaderProgram>("shaders/main.vert", "shaders/main.frag");

    // Get locations of vertex attributes position and normal
    const GLint positionLoc = program->GetAttributeLocation("position");
    const GLint normalLoc = program->GetAttributeLocation("normal");
    const GLint texCoordLoc = program->GetAttributeLocation("tex_coord");

    // Get locations of uniforms for positioning and projecting object
    modelMatrixLoc = program->GetUniformLocation("model_matrix");
    viewMatrixLoc = program->GetUniformLocation("view_matrix");
    projectionMatrixLoc = program->GetUniformLocation("projection_matrix");

    lightPositionLoc = program->GetUniformLocation("light_position");
    eyePositionLoc = program->GetUniformLocation("eye_position");
    lightAmbientColorLoc = program->GetUniformLocation("light_ambient_color");
    lightDiffuseColorLoc = program->GetUniformLocation("light_diffuse_color");
    lightSpecularColorLoc = program->GetUniformLocation("light_specular_color");

    materialAmbientColorLoc = program->GetUniformLocation("material_ambient_color");
    materialDiffuseColorLoc = program->GetUniformLocation("material_diffuse_color");
    materialSpecularColorLoc = program->GetUniformLocation("material_specular_color");
    materialShininessLoc = program->GetUniformLocation("material_shininess");

    objTextureLoc = program->GetUniformLocation("obj_texture");

    const auto loadAndSetTexture = [](const std::string& name, GLuint& pos)
    {
        pos = Texture::LoadTexture2D("images/" + name + ".png");
        glBindTexture(GL_TEXTURE_2D, pos);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    };

    loadAndSetTexture("bricks", tBricks);
    loadAndSetTexture("glass", tGlass);
    loadAndSetTexture("roof", tRoof);
    loadAndSetTexture("rune", tRune);
    loadAndSetTexture("wood", tWood);
    glBindTexture(GL_TEXTURE_2D, 0);

    cube.CreateVao(positionLoc, normalLoc, texCoordLoc);
    sphere.CreateVao(positionLoc, normalLoc, texCoordLoc);
    ground.CreateVao(positionLoc, normalLoc, texCoordLoc);
    pad.CreateVao(positionLoc, normalLoc, texCoordLoc);
    brick.CreateVao(positionLoc, normalLoc, texCoordLoc);

    cube.SetTexture(tRoof);
    sphere.SetTexture(tGlass);
    ground.SetTexture(tBricks);
    pad.SetTexture(tWood);
    brick.SetTexture(tRune);

    // Nuklear
    ctx = nk_glfw3_init(Window.GetWindow(), NK_GLFW3_INSTALL_CALLBACKS);
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();
    
    // Objects
    SpawnBalls();

    pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 0.f);
    // pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 2.f * Geometry::pi / 3.f);
    // pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 4.f * Geometry::pi / 3.f);

    for (float i = 0; i < 1.f; ++i) {
        float offset = i * 1.9f;
        float height = i * BRICK_HEIGHT;
        for (int j = 0; j < 8; ++j) {
            bricks.emplace_back(BRICK_DISTANCE, BRICK_SEGMENTS, 2.f * j * Geometry::pi / 8.f + offset, height);
        }
    }
}

void Application::Step() {
    if (isPaused) {
        return;
    }
    if (isStepping) {
        isPaused = true;
    }

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

void Application::Render() {
    nk_glfw3_new_frame();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    program->Use();

    // Get the current time
    const auto appTime = float(glfwGetTime());

    // Get the aspect ratio of window size
    const auto width = float(Window.GetWidth());
    const auto height = float(Window.GetHeight());
    const auto aspectRatio = width / height;

    // Clear screen, both color and depth
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set cameras projection and view matrices and eye location
    const auto projectionMatrix = Geometry::Perspective(45.0f, aspectRatio, 1.f, 200.0f);
    const auto viewMatrix = LookAt(camera.GetEyePosition(), Geometry::Vector<3>(), { 0.0f, 1.0f, 0.0f });

    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, &projectionMatrix);
    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix);
    glUniform3fv(eyePositionLoc, 1, &camera.GetEyePosition());

    // Set light
    auto lightPos = Geometry::Vector<3>{ 5.f / 4.f + 0.75f, 1.f, 0.f };//.Rotate(appTime, { 0.f, 1.f, 0.f });
    glUniform4f(lightPositionLoc, lightPos.X(), lightPos.Y(), lightPos.Z(), 0.f);

    // Colors
    glUniform3f(lightAmbientColorLoc, 0.1f, 0.1f, 0.1f);
    glUniform3f(lightDiffuseColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(lightSpecularColorLoc, 1.0f, 1.0f, 1.0f);

    // Materials
    glUniform3f(materialAmbientColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(materialDiffuseColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(materialSpecularColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform1f(materialShininessLoc, 200.0f);

    // Texture
    glUniform1i(objTextureLoc, 0);
    glActiveTexture(GL_TEXTURE0);

    // Model matrix
    const auto modelMatrix = Geometry::Matrix<4>::Identity();
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix);

    ground.Draw();

    for (auto& ball : balls) {
        DrawObject(sphere, ball);
    }

    for (auto& padCol : pads) {
        DrawObject(pad, padCol);
    }

    for (auto& brickCol : bricks) {
        DrawObject(brick, brickCol);
    }

    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
}

void Application::Gui() {
    if (nk_begin(ctx, "Debug", nk_rect(10, 10, 230, 400), NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
        nk_layout_row_dynamic(ctx, 32, 1);
        nk_label(ctx, "Ball", NK_TEXT_LEFT);
        auto ballPosition = balls[0].Position();
        const auto ballAngle = Geometry::Degrees(std::atan2(ballPosition.Z(), ballPosition.X()));

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Position: ", NK_TEXT_LEFT);
        nk_label(ctx, ("[" + Geometry::ToString(ballPosition.X(), 2) + ", " + Geometry::ToString(ballPosition.Z(), 2) + "]").c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Velocity: ", NK_TEXT_LEFT);
        nk_label(ctx, ("[" + Geometry::ToString(balls[0].Velocity().X(), 2) + ", " + Geometry::ToString(balls[0].Velocity().Z(), 2) + "]").c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Distance: ", NK_TEXT_LEFT);
        nk_label(ctx, std::to_string(ballPosition.Magnitude()).c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Angle: ", NK_TEXT_LEFT);
        nk_label(ctx, std::to_string(ballAngle).c_str(), NK_TEXT_LEFT);

        nk_layout_row_dynamic(ctx, 32, 1);
        nk_label(ctx, "Pad", NK_TEXT_LEFT);
        const auto startAngle = Geometry::Degrees(pads[0].AngleStart());
        const auto endAngle = Geometry::Degrees(pads[0].AngleEnd());

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Distance: ", NK_TEXT_LEFT);
        nk_label(ctx, std::to_string(pads[0].MiddleRadius()).c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "Start Angle: ", NK_TEXT_LEFT);
        nk_label(ctx, std::to_string(startAngle).c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        nk_label(ctx, "End Angle: ", NK_TEXT_LEFT);
        nk_label(ctx, std::to_string(endAngle).c_str(), NK_TEXT_LEFT);

        nk_layout_row_static(ctx, 26, 100, 2);
        if (nk_button_label(ctx, isPaused ? "Play" : "Pause")) {
            isPaused = !isPaused;
        }
        if (nk_button_label(ctx, isStepping ? "Step On" : "Step Off")) {
            isStepping = !isStepping;
        }
        
        if (nk_button_label(ctx, "Respawn balls")) {
            balls.clear();
            SpawnBalls();
        }
    }
    nk_end(ctx);
}

void Application::DrawObject(const Mesh& mesh, const Collisions::Collider& collider) const {
    auto modelMatrix = Geometry::Matrix<4>::Identity();
    class Visitor : public Collisions::ConstColliderVisitor {
    public:
        explicit Visitor(Geometry::Matrix<4>& modelMatrix)
            : ModelMatrix(modelMatrix) {}

        void operator()(const Collisions::BallCollider& col) override {
            ModelMatrix.Translate(col.Position());
        }

        void operator()(const Collisions::BrickCollider& col) override {
            ModelMatrix
                    .Rotate(-col.AngleStart(), Geometry::Vector<3>{ 0.f, 1.f, 0.f })
                    .Translate(Geometry::Vector<3>{ 0.f, col.Height(), 0.f });
        }

        Geometry::Matrix<4>& ModelMatrix;
        bool StopThere = false;
    } visitor{ modelMatrix };
    collider.Visit(visitor);

    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix);
    mesh.Draw();
}

void Application::SpawnBalls() {
    std::random_device r;
    std::default_random_engine e(r());
    const std::uniform_real_distribution<float> pos(-5.f, 5.f);
    const std::uniform_real_distribution<float> vel(-0.5f, 0.5f);
    for (unsigned i = 0; i < 1; ++i) {
        balls.emplace_back(Geometry::Vector<3>{ pos(e), 1.f, pos(e) }, Geometry::Vector<3>{ vel(e), 0.f, vel(e) }, 1.f);
    }
}

void Application::OnMousePosition(double x, double y) {
    camera.OnMouseMove(x, y);
}

void Application::OnMouseButton(int button, int action, int mods) {
    camera.OnMouseButton(button, action, mods);
}

void Application::OnKey(int key, int scancode, int actions, int mods) {
    switch (key) {
    case GLFW_KEY_L:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        return;
    case GLFW_KEY_F:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        return;
    case GLFW_KEY_A:
        if (actions == GLFW_PRESS) {
            movement -= movementSpeed;
        } else if (actions == GLFW_RELEASE) {
            movement += movementSpeed;
        }
        return;
    case GLFW_KEY_D:
        if (actions == GLFW_PRESS) {
            movement += movementSpeed;
        } else if (actions == GLFW_RELEASE) {
            movement -= movementSpeed;
        }
    default:
        break;
    }
}

void Application::OnResize(int width, int height) {
    Window.Resize(width, height);
    glViewport(0, 0, width, height);
}
