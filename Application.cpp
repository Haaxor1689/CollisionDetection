#include "Application.hpp"

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

    loadAndSetTexture("default", tDefault);
    loadAndSetTexture("stripes", tStripes);
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
    sphere.SetTexture(tDefault);
    ground.SetTexture(tStripes);
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

    if (bricks.size() == 0) {
        WinGame();
        return;
    }

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
                score += brickValue;
            }
        }
        for (auto& otherBall : balls) {
            ball.Collision(otherBall);
        }
    }
}

void Application::Render() {
    nk_glfw3_new_frame();
    glClearColor(0.8f, 0.85f, 0.9f, 0.0f);
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

    // Set camera's projection and view matrices and eye location
    auto projectionMatrix = Geometry::Perspective(45.0f, aspectRatio, 1.f, 200.f);
    Geometry::Vector<3> eye;
    Geometry::Vector<3> center = {};
    Geometry::Vector<3> up = { 0.0f, 1.0f, 0.0f };
    switch (cameraMode) {
    case CameraMode::Perspective:
        eye = { 0.f, 80.f, -60.f };
        center = { 0.f, -10.f, 0.f };
        break;
    case CameraMode::Top:
        projectionMatrix = Geometry::Ortho(-RADIUS * aspectRatio, RADIUS * aspectRatio, -RADIUS, RADIUS, 1.f, 200.f);
        eye = { 0.f, 100.f, 0.f };
        up = { 0.0f, 0.0f, 1.0f };
        break;
    case CameraMode::Ball:
        eye = Geometry::Vector<3>(balls[selectedBall].Position()).Translate({ 0.f, 5.f, 0.f }) * 2.5f;
        // center = balls[selectedBall].Position();
        break;
    }
    const auto viewMatrix = LookAt(eye, center, up);

    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, &projectionMatrix);
    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix);
    glUniform3fv(eyePositionLoc, 1, &eye);

    // Set light
    auto lightPos = Geometry::Vector<3>{ 0.5f, 0.5f, 0.f };
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
    const auto mainMenuWIdth = 300;
    if (isInMenu) {
        // Main menu window
        const auto windowHeight = 140;
        if (nk_begin(ctx, "Debug", nk_rect((Window.GetWidth() - mainMenuWIdth) / 2.f, (Window.GetHeight() - windowHeight) / 2.f, mainMenuWIdth, windowHeight), NK_WINDOW_BORDER)) {
            const auto menuItemWidth = mainMenuWIdth - 26;
            nk_layout_row_static(ctx, 52, menuItemWidth, 1);
            nk_label(ctx, "3D breakout", NK_TEXT_CENTERED);

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            if (nk_button_label(ctx, "Play")) {
                RestartGame();
            }

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            if (nk_button_label(ctx, "Exit")) {
                Window.Close();
            }
        }
        nk_end(ctx);
        return;
    }

    if (hasWon) {
        // You won window
        const auto windowHeight = 200;
        if (nk_begin(ctx, "Debug", nk_rect((Window.GetWidth() - mainMenuWIdth) / 2.f, (Window.GetHeight() - windowHeight) / 2.f, mainMenuWIdth, windowHeight), NK_WINDOW_BORDER)) {
            const auto menuItemWidth = mainMenuWIdth - 26;
            nk_layout_row_static(ctx, 52, menuItemWidth, 1);
            nk_label(ctx, "You won", NK_TEXT_CENTERED);

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            nk_label(ctx, "Final score:", NK_TEXT_CENTERED);

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            nk_label(ctx, std::to_string(score).c_str(), NK_TEXT_CENTERED);

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            if (nk_button_label(ctx, "Restart")) {
                RestartGame();
                hasWon = false;
            }

            nk_layout_row_static(ctx, 26, menuItemWidth, 1);
            if (nk_button_label(ctx, "Exit")) {
                Window.Close();
            }
        }
        nk_end(ctx);
        return;
    }

    unsigned nextWindowY = 10;
    // Debug window
    if (nk_begin(ctx, "Debug", nk_rect(10, nextWindowY, 270, 180), NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE)) {
        nextWindowY += 150;

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
    }
    nk_end(ctx);

    nextWindowY += 40;

    // Game rules window
    if (nk_begin(ctx, "Game rules", nk_rect(10, nextWindowY, 270, 200), NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE)) {
        nextWindowY += 170;

        const auto biColumnSize = 120;

        // Rules
        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Balls(" + std::to_string(ballCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 1, &ballCount, 10, 1);

        if (nk_button_label(ctx, "Respawn balls")) {
            SpawnBalls();
        }

        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Brick rows(" + std::to_string(brickRowCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 1, &brickRowCount, 10, 1);

        nk_layout_row_static(ctx, 26, biColumnSize, 2);
        nk_label(ctx, ("Brick columns(" + std::to_string(brickColumnCount) + ")").c_str(), NK_TEXT_LEFT);
        nk_slider_int(ctx, 7, &brickColumnCount, 11, 1);

        if (nk_button_label(ctx, "Respawn bricks")) {
            SpawnBricks();
        }
    }
    nk_end(ctx);

    nextWindowY += 40;

    // Controlls window
    if (nk_begin(ctx, "Controlls", nk_rect(10, nextWindowY, 270, 170), NK_WINDOW_TITLE | NK_WINDOW_BORDER | NK_WINDOW_MINIMIZABLE)) {
        const auto labelSize = 140;
        const auto keySize = 100;

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Rotate pads:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, "A/D", NK_TEXT_LEFT);

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Play/Pause:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, "Space", NK_TEXT_LEFT);

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Restart:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, "R", NK_TEXT_LEFT);

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Change camera:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, "C", NK_TEXT_LEFT);

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Menu:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, "Esc", NK_TEXT_LEFT);
    }
    nk_end(ctx);

    // Score window
    const auto windowWidth = 270;
    if (nk_begin(ctx, "Score", nk_rect(Window.GetWidth() - windowWidth - 10, 10, windowWidth, 80), NK_WINDOW_BORDER)) {
        const auto labelSize = 140;
        const auto keySize = 100;

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Score:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, std::to_string(score).c_str(), NK_TEXT_LEFT);

        nk_layout_row_begin(ctx, NK_STATIC, 26, 2);
        nk_layout_row_push(ctx, labelSize);
        nk_label(ctx, "Remaining bricks:", NK_TEXT_LEFT);
        nk_layout_row_push(ctx, keySize);
        nk_label(ctx, std::to_string(bricks.size()).c_str(), NK_TEXT_LEFT);
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
    balls.clear();
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
    bricks.clear();
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

void Application::RestartGame() {
    SpawnBalls();
    SpawnBricks();
    isPaused = false;
    isInMenu = false;
    hasWon = false;
    score = 0;
}

void Application::WinGame() {
    hasWon = true;
    isPaused = true;
}

void Application::NextCameraMode() {
    switch (cameraMode) {
    case CameraMode::Perspective:
        cameraMode = CameraMode::Top;
        break;
    case CameraMode::Top:
        cameraMode = CameraMode::Ball;
        break;
    case CameraMode::Ball:
        cameraMode = CameraMode::Perspective;
        break;
    }
}

void Application::OnKey(int key, int scancode, int actions, int mods) {
    switch (key) {
    case GLFW_KEY_L:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        return;
    case GLFW_KEY_F:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        return;
    case GLFW_KEY_D:
        if (actions == GLFW_PRESS) {
            movement -= movementSpeed;
        } else if (actions == GLFW_RELEASE) {
            movement += movementSpeed;
        }
        return;
    case GLFW_KEY_A:
        if (actions == GLFW_PRESS) {
            movement += movementSpeed;
        } else if (actions == GLFW_RELEASE) {
            movement -= movementSpeed;
        }
        return;
    case GLFW_KEY_R:
        if (actions == GLFW_PRESS) {
            RestartGame();
        }
        return;
    case GLFW_KEY_SPACE:
        if (actions == GLFW_PRESS && !isInMenu) {
            isPaused = !isPaused;
        }
        return;
    case GLFW_KEY_C:
        if (actions == GLFW_PRESS) {
            NextCameraMode();
        }
        return;
    case GLFW_KEY_ESCAPE:
        if (actions == GLFW_PRESS) {
            if (hasWon) {
                RestartGame();
            }
            isInMenu = !isInMenu;
            isPaused = isInMenu;
        }
    default:
        break;
    }
}

void Application::OnResize(int width, int height) {
    Window.Resize(width, height);
    glViewport(0, 0, width, height);
}
