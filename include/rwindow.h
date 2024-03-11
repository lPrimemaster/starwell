#pragma once
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/gl.h>
#include <glfw3.h>

#include "math.h"
#include "camera.h"

class RenderWindow
{
public:
    RenderWindow(const std::string& name, bool maximized = true);
    RenderWindow(const RenderWindow& window) = delete;
    RenderWindow(RenderWindow&& window) = delete;

    ~RenderWindow();

    bool initOK() const;
    bool windowOpen() const;
    void render(Camera& camera, bool* repopulate);
    void clearBuffer();
    void swapBuffers();

private:
    bool glfwOK;
    GLFWwindow* window;
    int width;
    int height;
};

