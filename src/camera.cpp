#include "../include/camera.h"
#include <cmath>

Camera::Camera(float fovRad, float aspect, Type type)
    : fov(fovRad), aspect(aspect), scrollSensitivity(1.0f), type(type), lockSentinel(true) {  }

void Camera::set(const PVector3& center, const PVector3& rotateOrCenter)
{
    centerLock = rotateOrCenter;

    // Locked camera is not modifiable
    if(lockSentinel._lock >= 0) return;

    position = center;
}

void Camera::lookat(const PVector3& center)
{
    if(lockSentinel._lock >= 0) return;

    centerLock = center;
}

void Camera::translate(const PVector3& v)
{
    // Locked camera positon is modifiable
    if(lockSentinel._lock >= 0) return;
    position += v;
}

PMatrix4 Camera::getMatrix() const
{
    switch (type)
    {
        case Type::FREE:
            return PMatrix4::Perspective(fov, aspect, 0.01f, 10000.0f) * 
                PMatrix4::Translate(position) * 
                PMatrix4::Rotate(centerLock.x, {1.0f, 0.0f, 0.0f}) *
                PMatrix4::Rotate(centerLock.y, {0.0f, 1.0f, 0.0f}) *
                PMatrix4::Rotate(centerLock.z, {0.0f, 0.0f, 1.0f});
        case Type::LOOKAT:
            return PMatrix4::Perspective(fov, aspect, 0.01f, 10000.0f) * PMatrix4::LookAt(position, centerLock);
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

PVector3 Camera::getHeading() const
{
    switch (type)
    {
        case Type::FREE:
            return PVector3{}; // TODO: (César): Missing implementation
        case Type::LOOKAT:
            return PVector3::Normalize(centerLock - position);
        default:
            return PVector3{};
    }
}

float Camera::getScrollSensitivity() const
{
    return scrollSensitivity;
}

void Camera::lock(const LockSentinel& ls)
{
    // Take ownership
    if(lockSentinel._lock == -1) lockSentinel = ls;
}

void Camera::unlock(const LockSentinel& ls)
{
    if(lockSentinel._lock == -1) return;
    if(lockSentinel == ls) lockSentinel = LockSentinel(true);
}

bool operator==(const Camera::LockSentinel& lhs, const Camera::LockSentinel& rhs)
{
    return lhs._lock == rhs._lock;
}
