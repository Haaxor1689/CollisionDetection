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
    SpawnBricks();

    pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 0.f);
    pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 2.f * Geometry::pi / 3.f);
    pads.emplace_back(PAD_DISTANCE, PAD_SEGMENTS, 4.f * Geometry::pi / 3.f);

}

void Application::Step() {
    if (isPaused) {
        return;
    }

    if (isStepping) {
        isPaused = true;
    }

    for (auto& brick : bricks) {
        // Drop down hnging bricks
        brick->Drop();
    }

    // Remove destroyed bricks
    bricks.erase(std::remove_if(bricks.begin(), bricks.end(), [](const auto& brick) { return brick->ShouldBeDeleted; }), bricks.end());

    // Move all balls
    for (auto& ball : balls) {
        ball.Step();
    }

    // Move all pads
    for (auto& pad : pads) {
        pad.Rotate(movement);
    }

    // Check for collisions
    for (auto& ball : balls) {
        ball.Collision(bounds);
        for (auto& pad : pads) {
            ball.Collision(pad);
        }
        for (auto& brick : bricks) {
            // Mark brick for removal if collision occurs
            if (ball.Collision(*brick)) {
                brick->ShouldBeDeleted = true;
            }
        }
        for (auto& otherBall : balls) {
            ball.Collision(otherBall);
        }
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
    auto lightPos = Geometry::Vector<3>{ 5.f / 4.f + 0.75f, 1.f, 0.f }; //.Rotate(appTime, { 0.f, 1.f, 0.f });
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
        DrawObject(brick, *brickCol);
    }

    nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
}

void Application::Gui() {

    if (isDebug && nk_begin(ctx, "Debug", nk_rect(10, 10, 270, 520), NK_WINDOW_TITLE | NK_WINDOW_BORDER)) {
        const auto infoLabelSize = 70;
        const auto infoSliderSize = 135;
        const auto infoButtonSize = 30;
        const auto biColumnSize = 120;

        // Balls
        nk_layout_row_begin(ctx, NK_STATIC, 30, 3);
        nk_layout_row_push(ctx, infoLabelSize);
        nk_label(ctx, ("Ball(" + std::to_string(selectedBall) + ")").c_str(), NK_TEXT_LEFT);
        nk_layout_row_push(ctx, infoSliderSize);
        nk_slider_int(ctx, 0, &selectedBall, balls.size() - 1, 1);
        nk_layout_row_push(ctx, infoButtonSize);
        if (nk_button_label(ctx, ballInfo ? "-" : "+")) {
            ballInfo = !ballInfo;
        }

        if (ballInfo) {
            auto ballPosition = balls[selectedBall].Position();
            const auto ballAngle = Geometry::Degrees(std::atan2(ballPosition.Z(), ballPosition.X()));

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Position: ", NK_TEXT_LEFT);
            nk_label(ctx, ("[" + Geometry::ToString(ballPosition.X(), 2) + ", " + Geometry::ToString(ballPosition.Z(), 2) + "]").c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Velocity: ", NK_TEXT_LEFT);
            nk_label(ctx, ("[" + Geometry::ToString(balls[selectedBall].Velocity().X(), 2) + ", " + Geometry::ToString(balls[selectedBall].Velocity().Z(), 2) + "]").c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Distance: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(ballPosition.Magnitude()).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Angle: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(ballAngle).c_str(), NK_TEXT_LEFT);
        }

        // Pads
        nk_layout_row_begin(ctx, NK_STATIC, 30, 3);
        nk_layout_row_push(ctx, infoLabelSize);
        nk_label(ctx, ("Pad(" + std::to_string(selectedPad) + ")").c_str(), NK_TEXT_LEFT);
        nk_layout_row_push(ctx, infoSliderSize);
        nk_slider_int(ctx, 0, &selectedPad, pads.size() - 1, 1);
        nk_layout_row_push(ctx, infoButtonSize);
        if (nk_button_label(ctx, padInfo ? "-" : "+")) {
            padInfo = !padInfo;
        }

        if (padInfo) {
            const auto startAngle = Geometry::Degrees(pads[selectedPad].AngleStart());
            const auto endAngle = Geometry::Degrees(pads[selectedPad].AngleEnd());

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Distance: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(pads[selectedPad].MiddleRadius()).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Start Angle: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(startAngle).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "End Angle: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(endAngle).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Movement: ", NK_TEXT_LEFT);
            nk_label(ctx, Geometry::ToString(movement, 2).c_str(), NK_TEXT_LEFT);

        }

        // Bricks
        nk_layout_row_begin(ctx, NK_STATIC, 30, 3);
        nk_layout_row_push(ctx, infoLabelSize);
        nk_label(ctx, ("Brick(" + std::to_string(selectedBrick) + ")").c_str(), NK_TEXT_LEFT);
        nk_layout_row_push(ctx, infoSliderSize);
        nk_slider_int(ctx, 0, &selectedBrick, bricks.size() - 1, 1);
        nk_layout_row_push(ctx, infoButtonSize);
        if (nk_button_label(ctx, brickInfo ? "-" : "+")) {
            brickInfo = !brickInfo;
        }

        if (brickInfo) {
            const auto startAngle = Geometry::Degrees(bricks[selectedBrick]->AngleStart());
            const auto endAngle = Geometry::Degrees(bricks[selectedBrick]->AngleEnd());

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Distance: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(bricks[selectedBrick]->MiddleRadius()).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Start Angle: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(startAngle).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "End Angle: ", NK_TEXT_LEFT);
            nk_label(ctx, std::to_string(endAngle).c_str(), NK_TEXT_LEFT);

            nk_layout_row_static(ctx, 26, biColumnSize, 2);
            nk_label(ctx, "Height: ", NK_TEXT_LEFT);
            nk_label(ctx, Geometry::ToString(bricks[selectedBrick]->Height(), 2).c_str(), NK_TEXT_LEFT);

        }

        // Game control
        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        if (nk_button_label(ctx, isPaused ? "Play" : "Pause")) {
            isPaused = !isPaused;
        }
        if (nk_button_label(ctx, isStepping ? "Step On" : "Step Off")) {
            isStepping = !isStepping;
        }

        // Rules
        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Balls(" + std::to_string(ballCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 1, &ballCount, 10, 1);

        if (nk_button_label(ctx, "Respawn balls")) {
            balls.clear();
            SpawnBalls();
        }

        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Brick rows(" + std::to_string(brickRowCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 1, &brickRowCount, 10, 1);

        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Brick columns(" + std::to_string(brickColumnCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 7, &brickColumnCount, 11, 1);

        if (nk_button_label(ctx, "Respawn bricks")) {
            bricks.clear();
            SpawnBricks();
        }
        nk_end(ctx);
    }
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
    const auto ballRadius = 1.f;
    const auto maxVelocity = 1.f;
    std::random_device r;
    std::default_random_engine e(r());
    const std::uniform_real_distribution<float> dist(BRICK_DISTANCE + BRICK_WIDTH + ballRadius * 2, PAD_DISTANCE - ballRadius * 2);
    const std::uniform_real_distribution<float> angle(-Geometry::pi, Geometry::pi);
    const std::uniform_real_distribution<float> vel(0.5f, 1.0f);

    balls.reserve(ballCount);
    for (auto i = 0; i < ballCount; ++i) {
        auto position = Geometry::Vector<3>{ dist(e), 1.f, 0.f }.Rotate(angle(e), { 0.f, 1.f, 0.f });
        auto velocity = Geometry::Vector<3>::Normalized(position) * maxVelocity * vel(e);
        balls.emplace_back(std::move(position), std::move(velocity), ballRadius, maxVelocity);
    }
}

void Application::SpawnBricks() {
    bricks.reserve(brickRowCount * brickColumnCount);
    auto index = 0;
    for (float i = 0; i < brickRowCount; ++i) {
        const auto offset = i * 0.4f;
        const auto height = i * BRICK_HEIGHT;
        for (auto j = 0; j < brickColumnCount; ++j) {
            bricks.emplace_back(std::make_unique<Collisions::BrickCollider>(BRICK_DISTANCE, BRICK_SEGMENTS, 2.f * j * Geometry::pi / brickColumnCount + offset, height));
            if (index >= brickColumnCount) {
                const auto first = index - brickColumnCount;
                const auto second = index - (index % brickColumnCount == brickColumnCount - 1 ? 2 * brickColumnCount - 1 : brickColumnCount - 1);
                bricks[index]->SetParents(bricks[first].get(), bricks[second].get());
            }
            ++index;
        }
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
        return;
    case GLFW_KEY_F1:
        if (actions == GLFW_PRESS) {
            isDebug = !isDebug;
        }
        break;
    case GLFW_KEY_SPACE:
        if (actions == GLFW_PRESS) {
            isPaused = !isPaused;
        }
        break;
    default:
        break;
    }
}

void Application::OnResize(int width, int height) {
    Window.Resize(width, height);
    glViewport(0, 0, width, height);
}
