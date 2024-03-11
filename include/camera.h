#pragma once
#include "math.h"

class Camera
{
public:
    enum class Type
    {
        FREE,
        LOOKAT
    };

    Camera(float fovRad, float aspect, Type type);

    void set(const PVector3& center, const PVector3& rotateOrCenter);

    PMatrix4 getMatrix() const;
    PVector3 getPosition() const;
    PVector3 getCenterOrRotation() const;


private:
    PVector3 position;
    PVector3 centerLock;
    float fov;
    float aspect;
    Type type;
};
