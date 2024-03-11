#include "../../include/windows/window.h"
#include "imgui.h"

GenWindow::GenWindow(const std::string& name, bool hidden)
    : name(name), hidden(hidden)
{

}

GenWindow::~GenWindow()
{

}

void GenWindow::draw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader)
{
    if(hidden) return;

    ImGui::Begin(name.c_str(), nullptr);
    internalDraw(camera, pstate, scene, shader);
    ImGui::End();
}
