#include "../../include/windows/settings.h"
#include "../../include/rwindow.h"
#include "imgui.h"
#include <cmath>
#include <mutex>
#include <stdio.h>

SettingsWindow::SettingsWindow(RenderWindow* parent) : GenWindow("Settings", parent)
{
    wflags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    setup = []() -> void {
        const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(0, mainViewport->Pos.y), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(0.2f * mainViewport->Size.x, mainViewport->Size.y), ImGuiCond_Once);
    };
}

SettingsWindow::~SettingsWindow()
{

}

void SettingsWindow::internalDraw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader)
{
    (void)pstate;
    (void)shader;

    if(ImGui::CollapsingHeader("Metrics"))
    {
        drawMetrics(camera);
    }

    if(ImGui::CollapsingHeader("Particle Analysis"))
    {
        drawAnalysis(camera, pstate);
    }

    if(ImGui::CollapsingHeader("Scene"))
    {
        drawSceneControl(scene);
    }
}

void SettingsWindow::drawMetrics(Camera& camera)
{
    ImGui::BeginDisabled();
    float delta = ImGui::GetIO().DeltaTime * 1000.0f;
    ImGui::InputFloat("Frametime", &delta);

    int particleCount = static_cast<int>(Body::GetLinearPositionPool()->size());
    ImGui::DragInt("Particle Count", &particleCount);

    PVector3 cameraPos = camera.getPosition();
    PVector3 cameraRot = camera.getCenterOrRotation();
    ImGui::DragFloat3("Cam Position", cameraPos.data);
    ImGui::DragFloat3("Cam Rotation", cameraRot.data);
    ImGui::EndDisabled();
}

void SettingsWindow::drawSceneControl(PythonScene& scene)
{
    if(ImGui::Button("Reload"))
    {
        scene.reload();
    }
}

void SettingsWindow::drawAnalysis(Camera& camera, InstanceState& pstate)
{
    (void)pstate;
    static Camera::LockSentinel sentinel;
    static bool lockOnCamera = false;
    static PVector3 lastParticlePos;
    static int lockScrollID;

    std::vector<PVector3>* bodyPositions = Body::GetLinearPositionPool();
    std::vector<PVector3>* bodyVelocities = Body::GetLinearVelocityPool();
    std::vector<PVector3>* bodyForces = Body::GetLinearForcePool();

    ImGui::BeginDisabled(particleFocus == -1);
    if(ImGui::Checkbox("Lock-on camera", &lockOnCamera))
    {
        if(lockOnCamera)
        {
            PVector3 cameraLookAt = bodyPositions->at(particleFocus);
            PVector3 cameraPos = cameraLookAt + PVector3{ 10.0f, 10.0f, 10.0f };
            lastParticlePos = cameraLookAt;
            camera.unlock(sentinel);
            camera.set(cameraPos, cameraLookAt);
            camera.lock(sentinel);
            
            lockScrollID = parent->registerScrollCallback([&camera](double yoff) -> void {
                camera.unlock(sentinel);
                camera.translate(camera.getScrollSensitivity() * yoff * camera.getHeading());
                camera.lock(sentinel);
            });
        }
        else if(lockScrollID >= 0)
        {
            parent->removeScrollCallback(lockScrollID);
        }
    }
    ImGui::EndDisabled();
    
    ImGui::DragInt("Particle", &particleFocus, 1.0f, -1, static_cast<int>(bodyPositions->size()));
    if(particleFocus >= 0)
    {
        if(lockOnCamera)
        {
            PVector3 cameraLookAt = bodyPositions->at(particleFocus);

            camera.unlock(sentinel);
            camera.lookat(cameraLookAt);
            camera.translate(cameraLookAt - lastParticlePos);
            camera.lock(sentinel);

            lastParticlePos = cameraLookAt;
        }

        ImGui::BeginDisabled();
        ImGui::InputFloat3("Position", bodyPositions->at(particleFocus).data);
        ImGui::InputFloat3("Velocity", bodyVelocities->at(particleFocus).data);
        ImGui::InputFloat3("Force", bodyForces->at(particleFocus).data);
        ImGui::EndDisabled();
    }
    else
    {
        lockOnCamera = false;
    }
}
