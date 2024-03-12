#include "window.h"

class SettingsWindow : public GenWindow
{
public:
    SettingsWindow(RenderWindow* parent);
    ~SettingsWindow();

protected:
    void internalDraw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader) override;

private:
    void drawMetrics(Camera& camera);
    void drawSceneControl(PythonScene& scene);
    void drawAnalysis(Camera& camera, InstanceState& pstate);

private:
    int particleFocus = -1;
};
