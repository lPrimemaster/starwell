#include "../include/rwindow.h"
#include "../include/windows/settings.h"
#include "imgui.h"
#include <GLFW/glfw3.h>

// #include <memory>

static void InternalScrollCalback(GLFWwindow* window, double xoff, double yoff)
{
    (void)xoff;
    RenderWindow* rwindow = reinterpret_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
    if(rwindow)
    {
        rwindow->runScrollCallbacks(yoff);
    }
}

RenderWindow::RenderWindow(const std::string& name, bool maximized)
    : glfwOK(false), window(nullptr), width(1920), height(1080)
{
    if(!glfwInit())
    {
        std::cerr << "Failed to init glfw." << std::endl;
        return;
    }

    glfwOK = true;

    glfwSetErrorCallback([](int error, const char* msg) {
        std::cerr << "glfw Error : " << error << " - " << msg << std::endl;
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);

    if(!window)
    {
        std::cerr << "Failed to create window." << std::endl;
        return;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    if(maximized)
    {
        glfwMaximizeWindow(window);
    }

    registerWindows();

    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, InternalScrollCalback);
}

RenderWindow::~RenderWindow()
{
    if(window)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
    }

    if(glfwOK)
    {
        glfwTerminate();
    }
}

bool RenderWindow::initOK() const
{
    return (window != nullptr) && glfwOK;
}

bool RenderWindow::windowOpen() const
{
    return !glfwWindowShouldClose(window);
}

void RenderWindow::render(Camera& camera, InstanceState& pstate, PythonScene& scene, GenShader& shader)
{
    glfwPollEvents();

    // Update
    pstate.updatePositions(Body::GetLinearPositionPool());
    pstate.updateColors(Body::GetColorPool());
    shader.load("MVP", camera.getMatrix());

    // Draw
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render pstate first
    pstate.draw();

    ImGui::ShowDemoWindow();

    // Render GUI windows here
    for(auto& window : windows)
    {
        window->draw(camera, pstate, scene, shader);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void RenderWindow::clearBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderWindow::swapBuffers()
{
    glfwSwapBuffers(window);
}

int RenderWindow::registerScrollCallback(std::function<void(double)> func)
{
    scrollCallbacks.push_back(func);
    return static_cast<int>(scrollCallbacks.size()) - 1;
}

void RenderWindow::removeScrollCallback(int id)
{
    if(id < static_cast<int>(scrollCallbacks.size()))
    {
        scrollCallbacks.erase(std::next(scrollCallbacks.begin(), id));
    }
}

void RenderWindow::runScrollCallbacks(double yoff)
{
    for(auto& sfun : scrollCallbacks)
    {
        sfun(yoff);
    }
}

void RenderWindow::registerWindows()
{
    windows.push_back(std::make_unique<SettingsWindow>(this));
}
