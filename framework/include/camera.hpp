#pragma once

#include "Geometry"

// This is a VERY SIMPLE class that allows to very simply move with the camera.
//
// Use left mouse button to change the point of view.
// Use right mouse button to zoom in and zoom out.
class Camera {
    // Constants that defines the behaviour of the camera
    //		- Minimum elevation in radians
    static const float minElevation;
    //		- Maximum elevation in radians
    static const float maxElevation;
    //		- Minimum distance from the point of interest
    static const float minDistance;
    //		- Sensitivity of the mouse when changing elevation or direction angles
    static const float angleSensitivity;
    //		- Sensitivity of the mouse when changing zoom
    static const float zoomSensitivity;

    // angle_direction is an angle in which determines into which direction in xz
    // plane I look.
    //		- 0 degrees .. I look in -z direction
    //		- 90 degrees .. I look in -x direction
    //		- 180 degrees .. I look in +z direction
    //		- 270 degrees .. I look in +x direction
    float angleDirection;

    // angle_direction is an angle in which determines from which "height" I
    // look.
    //		- positive elevation .. I look from above the xz plane
    //		- negative elevation .. I look from below the xz plane
    float angleElevation;

    // Distance from (0,0,0), the point at which I look
    float distance;

    // Final position of the eye in world space coordinates, for LookAt or shaders
    Geometry::Vector<3> eyePosition;

    // Last X and Y coordinates of the mouse cursor
    int lastX;
    int lastY;

    // True or false if moused buttons are pressed and the user rotates/zooms the camera
    bool isRotating;
    bool isZooming;

    // Recomputes 'eye_position' from 'angle_direction', 'angle_elevation', and 'distance'
    void UpdateEyePos();

public:
    Camera();

    // Call when the user presses or releases a mouse button (see glfwSetMouseButtonCallback)
    void OnMouseButton(int button, int action, int mods);

    // Call when the user moves with the mouse cursor (see glfwSetCursorPosCallback)
    void OnMouseMove(double x, double y);

    // Returns the position of the eye in world space coordinates
    Geometry::Vector<3> GetEyePosition() const;
};
