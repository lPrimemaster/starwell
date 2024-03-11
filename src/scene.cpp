#include "../include/scene.h"
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

// Set a global python interpreter to live the whole time
// Initialization order does not matter for this
// Unless we are planning on using python out of this class
static pybind11::scoped_interpreter interp{};

PythonScene::PythonScene(const std::string& name) : module(pybind11::module_::import(name.c_str()))
{
    reload();
}

void PythonScene::reload()
{
    module.reload();
    populateBodiesFromScript();
}

std::vector<Body>* PythonScene::getBodies()
{
    return &bodies;
}

std::optional<std::pair<std::vector<PVector3>, std::vector<PVector3>>> PythonScene::parsePythonBodyPos(const pybind11::tuple& input)
{
    if(input.size() != 2)
    {
        std::cerr << "Failed to parse python script : Must return a pair (positions, velocities)." << std::endl;
        return {};
    }

    auto pos = input[0];
    auto vel = input[1];
    
    std::vector<PVector3> positions;
    for(const auto& v : pos)
    {
        auto it = v.begin();
        float x = it->cast<float>();
        float y = (++it)->cast<float>();
        float z = (++it)->cast<float>();
        positions.push_back({x, y, z});
    }

    std::vector<PVector3> velocities;
    for(const auto& v : vel)
    {
        auto it = v.begin();
        float x = it->cast<float>();
        float y = (++it)->cast<float>();
        float z = (++it)->cast<float>();
        velocities.push_back({x, y, z});
    }

    return std::make_pair(positions, velocities);
}

void PythonScene::populateBodiesFromScript()
{
    Body::ResetPools();
    bodies.clear();
    pybind11::tuple data = module.attr("main")();

    auto nativeData = parsePythonBodyPos(data);
    if(nativeData)
    {
        auto [positions, velocities] = *nativeData;
        for(std::size_t i = 0; i < positions.size(); i++)
        {
            bodies.push_back(Body(positions[i], velocities[i]));
        }
    }
}

