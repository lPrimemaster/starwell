#pragma once
#include <pybind11/embed.h>

#include "math.h"
#include "body.h"

class PythonScene
{
public:
    explicit PythonScene(const std::string& name);
    ~PythonScene() = default;
    void reload();

    std::vector<Body>* getBodies(); // TODO: (César): Prefer a different approach

private:
    std::optional<std::tuple<std::vector<PVector3>, std::vector<PVector3>, std::vector<UVector4>>> parsePythonBodyPos(const pybind11::tuple& input);
    void populateBodiesFromScript();

private:
    pybind11::module_ module;
    std::vector<Body> bodies;
};
