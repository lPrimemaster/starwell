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

    struct LockSentinel
    {
        LockSentinel(bool inert = false) { if(inert) return; _lock = gLockID++; }


        LockSentinel(const LockSentinel&) = delete;
        LockSentinel(LockSentinel&& other) = delete;
        LockSentinel& operator=(const LockSentinel& other) { _lock = other._lock; return *this; }

        long _lock = -1;
        static inline long gLockID = 0;
    };

    Camera(float fovRad, float aspect, Type type);

    void set(const PVector3& center, const PVector3& rotateOrCenter);
    void lookat(const PVector3& center);
    void translate(const PVector3& v);

    PMatrix4 getMatrix() const;
    PVector3 getPosition() const;
    PVector3 getCenterOrRotation() const;
    PVector3 getHeading() const;
    
    float getScrollSensitivity() const;

    void lock(const LockSentinel& ls);
    void unlock(const LockSentinel& ls);

private:
    PVector3 position;
    PVector3 centerLock;
    float fov;
    float aspect;
    float scrollSensitivity;
    Type type;
    LockSentinel lockSentinel;
};

bool operator==(const Camera::LockSentinel& lhs, const Camera::LockSentinel& rhs);
