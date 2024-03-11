#include "window.h"

class SettingsWindow : public GenWindow
{
public:
    SettingsWindow();
    ~SettingsWindow();

protected:
    void internalDraw(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader) override;
};
