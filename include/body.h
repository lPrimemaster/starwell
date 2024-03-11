#pragma once
#include "math.h"
#include <vector>

class Body
{
public:
    Body(const PVector3& position, const PVector3& velocity = {0, 0, 0}, const UVector4& color = {255, 255, 255, 255});


    Body(const Body& body) = default;
    Body(Body&& body) = default;
    ~Body() = default;

    void move(const PVector3& field);

    float getMass() const;
    PVector3 getPosition() const;


    static std::vector<PVector3>* GetLinearPositionPool();
    static std::vector<PVector3>* GetLinearVelocityPool();
    static std::vector<PVector3>* GetLinearForcePool();
    static std::vector<UVector4>* GetColorPool();
    static void ResetPools();


private:
    static inline std::vector<PVector3> PositionPool;
    static inline std::vector<PVector3> VelocityPool;
    static inline std::vector<PVector3> ForcePool;
    static inline std::vector<UVector4> ColorPool;
    PVector3* force;
    PVector3* position;
    PVector3* velocity;
    UVector4* color;
    float mass;
};
