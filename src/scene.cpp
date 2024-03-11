#include "../include/scene.h"
#include <cmath>
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

std::optional<std::tuple<std::vector<PVector3>, std::vector<PVector3>, std::vector<UVector4>>> PythonScene::parsePythonBodyPos(const pybind11::tuple& input)
{
    if(input.size() != 3)
    {
        std::cerr << "Failed to parse python script : Must return a tuple (positions, velocities, colors)." << std::endl;
        return {};
    }

    auto pos = input[0];
    auto vel = input[1];
    auto col = input[2];
    
    std::vector<PVector3> positions;
    for(const auto& p : pos)
    {
        auto it = p.begin();
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

    std::vector<UVector4> colors;
    for(const auto& c : col)
    {
        auto it = c.begin();
        unsigned char r = it->cast<unsigned char>();
        unsigned char g = (++it)->cast<unsigned char>();
        unsigned char b = (++it)->cast<unsigned char>();
        unsigned char a = (++it)->cast<unsigned char>();
        colors.push_back({r, g, b, a});
    }

    return std::make_tuple(positions, velocities, colors);
}

void PythonScene::populateBodiesFromScript()
{
    Body::ResetPools();
    bodies.clear();
    pybind11::tuple data = module.attr("main")();

    auto nativeData = parsePythonBodyPos(data);
    if(nativeData)
    {
        auto [positions, velocities, colors] = *nativeData;
        for(std::size_t i = 0; i < positions.size(); i++)
        {
            bodies.push_back(Body(positions[i], velocities[i], colors[i]));
        }
    }
}
