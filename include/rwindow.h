#pragma once
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/gl.h>
#include <glfw3.h>
#include <vector>

#include "math.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "windows/window.h"

class RenderWindow
{
public:
    RenderWindow(const std::string& name, bool maximized = true);
    RenderWindow(const RenderWindow& window) = delete;
    RenderWindow(RenderWindow&& window) = delete;

    ~RenderWindow();

    bool initOK() const;
    bool windowOpen() const;
    void render(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader);
    void clearBuffer();
    void swapBuffers();

    int registerScrollCallback(std::function<void(double)> func);
    void removeScrollCallback(int id);
    void runScrollCallbacks(double yoff);

private:
    void registerWindows();

private:
    bool glfwOK;
    GLFWwindow* window;
    int width;
    int height;
    std::vector<std::unique_ptr<GenWindow>> windows;
    std::vector<std::function<void(double)>> scrollCallbacks;
};

