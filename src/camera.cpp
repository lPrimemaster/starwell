#include "../include/camera.h"

Camera::Camera(float fovRad, float aspect, Type type)
    : fov(fovRad), aspect(aspect), type(type) {  }

void Camera::set(const PVector3& center, const PVector3& rotateOrCenter)
{
    position = center;
    centerLock = rotateOrCenter;
}

PMatrix4 Camera::getMatrix() const
{
    switch (type)
    {
        case Type::FREE:
            return PMatrix4::Perspective(fov, aspect, 0.01f, 1000.0f) * 
                PMatrix4::Translate(position) * 
                PMatrix4::Rotate(centerLock.x, {1.0f, 0.0f, 0.0f}) *
                PMatrix4::Rotate(centerLock.y, {0.0f, 1.0f, 0.0f}) *
                PMatrix4::Rotate(centerLock.z, {0.0f, 0.0f, 1.0f});
        case Type::LOOKAT:
            return PMatrix4::Perspective(fov, aspect, 0.01f, 1000.0f) * PMatrix4::LookAt(position, centerLock);
        default:
            return PMatrix4::Identity();
    }
}

PVector3 Camera::getPosition() const
{
    return position;
}

PVector3 Camera::getCenterOrRotation() const
{
    return centerLock;
}
