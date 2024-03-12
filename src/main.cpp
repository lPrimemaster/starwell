// This is a very simple attempt to start some simulation code using the Barnes-Hut algorithm for n-body interactions
// It features a O(n log n) complexity

#include <stperf.h>
#include <config.h>

#include "../include/math.h"
#include "../include/camera.h"
#include "../include/body.h"
#include "../include/bhtree.h"
#include "../include/rwindow.h"
#include "../include/draw.h"
#include "../include/scene.h"

int main(void)
{
    // Init window
    RenderWindow rwindow("starwell v" STARWELL_VERSION);

    // Init shader(s)
    GenShader shader;

    // Start with the classic shader ON
    shader.swap("classic");

    // Init instances memory on GPU
    InstanceState pstate;

    // Init camera
    Camera camera(PRadians(90.0f), (16.0f / 9.0f), Camera::Type::LOOKAT);
    camera.set({0.0f, -20.0f, -500.0f}, {0.0f, 0.0f, 0.0f});
    shader.load("sf", 5.0f);

    // Scroll default behaviour
    rwindow.registerScrollCallback([&camera](double yoff) -> void {
        camera.translate(camera.getScrollSensitivity() * yoff * camera.getHeading());
    });
    
    // Init BH tree
    BHTree tree;

    // Populate the space with the selected script
    PythonScene scene("scenes.galaxies");

    if(rwindow.initOK())
    {
        while(rwindow.windowOpen())
        {
            // Compute BHTree
            for(auto& body : *scene.getBodies())
            {
                tree.insertBody(&body);
            }
            
            // Render
            rwindow.clearBuffer();
            rwindow.render(camera, pstate, scene, shader);
            rwindow.swapBuffers();
            
            // Calculate field from BHTree and displace bodies
            for(auto& body : *scene.getBodies())
            {
                PVector3 field = tree.calculateFieldOnPoint(body.getPosition(), 0.5f);
                body.move(field);
            }
            tree.reset();
        }
    }
    return 0;
}
