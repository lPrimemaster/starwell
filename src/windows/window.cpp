#include "../../include/windows/window.h"
#include "../../include/rwindow.h"
#include "imgui.h"


GenWindow::GenWindow(const std::string& name, RenderWindow* parent, bool hidden)
    : wflags(0), name(name), hidden(hidden), parent(parent), firstRun(true)
{

}

GenWindow::~GenWindow()
{

}

void GenWindow::draw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader)
{
    if(hidden) return;
    
    if(firstRun)
    {
        setup();
        firstRun = false;
    }

    ImGui::Begin(name.c_str(), nullptr, wflags);
    internalDraw(camera, pstate, scene, shader);
    ImGui::End();
}
