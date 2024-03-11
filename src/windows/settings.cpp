#include "../../include/windows/settings.h"
#include "imgui.h"

SettingsWindow::SettingsWindow() : GenWindow("Settings")
{

}

SettingsWindow::~SettingsWindow()
{

}

void SettingsWindow::internalDraw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader)
{
    (void)pstate;
    (void)shader;

    ImGui::Text("%3.0f", ImGui::GetIO().Framerate);
    PVector3 cameraPos = camera.getPosition();
    PVector3 cameraRot = camera.getCenterOrRotation();
    ImGui::DragFloat3("Camera position", cameraPos.data);
    ImGui::DragFloat3("Camera rotation", cameraRot.data);
    camera.set(cameraPos, cameraRot);
    if(ImGui::Button("Reload scene"))
    {
        scene.reload();
    }
}
