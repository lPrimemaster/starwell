#include "../include/rwindow.h"

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
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    if(maximized)
    {
        glfwMaximizeWindow(window);
    }
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

void RenderWindow::render(Camera& camera, bool* repopulate)
{
    glfwPollEvents();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render GUI windows here
    ImGui::Begin("Test window");
    ImGui::Text("%3.0f", ImGui::GetIO().Framerate);

    PVector3 cameraPos = camera.getPosition();
    PVector3 cameraRot = camera.getCenterOrRotation();
    ImGui::DragFloat3("Camera position", cameraPos.data);
    ImGui::DragFloat3("Camera rotation", cameraRot.data);
    camera.set(cameraPos, cameraRot);

    *repopulate = ImGui::Button("Restart script");
    
    ImGui::End();

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
