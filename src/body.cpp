#include "../include/body.h"

Body::Body(const PVector3& position, const PVector3& velocity) : force(nullptr), mass(1.0f)
{
    if(PositionPool.empty())
    {
        // Make an initial allocation
        std::cout << "Creating PositionPool with 1'000'000 capacity." << std::endl;
        PositionPool.reserve(1'000'000); // TODO: (César) : Remove these magic numbers
        VelocityPool.reserve(1'000'000); // TODO: (César) : Remove these magic numbers
        ForcePool.reserve(1'000'000); // TODO: (César) : Remove these magic numbers
    }

    if(PositionPool.size() == PositionPool.capacity())
    {
        std::cerr << "Body PositionPool reached max capacity. Oops!" << std::endl;
    }

    PositionPool.push_back(position);
    VelocityPool.push_back(velocity);
    ForcePool.push_back(PVector3{0.0f, 0.0f, 0.0f});
    this->position = &PositionPool.back();
    this->velocity = &VelocityPool.back();
    this->force = &ForcePool.back();
}

void Body::move(const PVector3& field)
{
    *force = mass * field;
    *velocity += 0.01f * 2 * (*force);
    *position += 0.01f * (*velocity);
}

float Body::getMass() const
{
    return mass;
}

PVector3 Body::getPosition() const
{
    return *position;
}

std::vector<PVector3>* Body::GetLinearPositionPool()
{
    return &PositionPool;
}

std::vector<PVector3>* Body::GetLinearVelocityPool()
{
    return &VelocityPool;
}

std::vector<PVector3>* Body::GetLinearForcePool()
{
    return &ForcePool;
}

void Body::ResetPools()
{
    PositionPool.clear();
    VelocityPool.clear();
    ForcePool.clear();

    PositionPool.shrink_to_fit();
    VelocityPool.shrink_to_fit();
    ForcePool.shrink_to_fit();
}
