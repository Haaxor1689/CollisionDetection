#include "camera.hpp"

#include <GLFW/glfw3.h>

const float Camera::minElevation = 0.1f;
const float Camera::maxElevation = 1.5f;
const float Camera::minDistance = 1.0f;
const float Camera::angleSensitivity = 0.008f;
const float Camera::zoomSensitivity = 0.003f;

Camera::Camera()
    : angleDirection(Geometry::pi / 2.f), angleElevation(1.2f), distance(90.0f), lastX(0), lastY(0), isRotating(false),
      isZooming(false) {
    UpdateEyePos();
}

void Camera::UpdateEyePos() {
    eyePosition.X() = distance * cosf(angleElevation) * -sinf(angleDirection);
    eyePosition.Y() = distance * sinf(angleElevation);
    eyePosition.Z() = distance * cosf(angleElevation) * cosf(angleDirection);
}

void Camera::OnMouseButton(int button, int action, int mods) {
    // Left mouse button affects the angles
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isRotating = true;
        } else {
            isRotating = false;
        }
    }
    // Right mouse button affects the zoom
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            isZooming = true;
        } else {
            isZooming = false;
        }
    }
}

void Camera::OnMouseMove(double x, double y) {
    auto dx = float(x - lastX);
    auto dy = float(y - lastY);
    lastX = static_cast<int>(x);
    lastY = static_cast<int>(y);

    if (isRotating) {
        angleDirection += dx * angleSensitivity;
        angleElevation += dy * angleSensitivity;

        // Clamp the results
        if (angleElevation > maxElevation)
            angleElevation = maxElevation;
        if (angleElevation < minElevation)
            angleElevation = minElevation;
    }
    if (isZooming) {
        distance *= 1.0f + dy * zoomSensitivity;

        // Clamp the results
        if (distance < minDistance)
            distance = minDistance;
    }
    UpdateEyePos();
}

Geometry::Vector<3> Camera::GetEyePosition() const {
    return eyePosition;
}
