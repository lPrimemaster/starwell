#pragma once
#include <string>
#include <functional>
#include <imgui.h>

#include "../camera.h"
#include "../draw.h"
#include "../scene.h"

class RenderWindow;

class GenWindow
{
public:
    GenWindow(const std::string& name, RenderWindow* parent, bool hidden = false);
    virtual ~GenWindow();

    GenWindow(const GenWindow&) = delete;
    GenWindow(GenWindow&&) = delete;
    GenWindow& operator=(const GenWindow&) = delete;
    GenWindow& operator=(GenWindow&&) = delete;

    void draw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader);

protected:
    virtual void internalDraw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader) = 0;

protected:
    std::function<void()> setup;
    ImGuiWindowFlags wflags;
    const std::string name;
    bool hidden;
    RenderWindow* parent;

private:
    bool firstRun;
};
