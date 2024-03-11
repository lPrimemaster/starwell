// This is a very simple attempt to start some simulation code using the Barnes-Hut algorithm for n-body interactions
// It features a O(n log n) complexity

#include <algorithm>
#include <array>
#include <complex>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <math.h>
#include <new>
#include <numeric>
#include <pybind11/cast.h>
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <random>
#include <string>
#include <typeinfo>
#include <utility>
#include <vector>
#include <cmath>
#include <iostream>
#include <regex>
#include <typeinfo>

#include <stperf.h>

#include <config.h>

#include "../include/math.h"
#include "../include/camera.h"
#include "../include/body.h"
#include "../include/bhtree.h"
#include "../include/rwindow.h"
#include "../include/draw.h"
#include "../include/scene.h"

// template<typename T>
// class LPool
// {
// public:
//     LPool(std::size_t maxInstances) : N(maxInstances)
//     {
//         buffer = new unsigned char[sizeof(T) * N];
//         state = new bool[N]{ false };
//     }
//
//     ~LPool() { if(buffer) delete[] buffer; }
//
//     void* alloc()
//     {
//         for(std::size_t i = 0; i < N; i++)
//         {
//             if(!state[i])
//             {
//                 state[i] = true;
//                 return &buffer[sizeof(T) * i];
//             }
//         }
//         throw std::bad_alloc();
//     }
//
//     void free(void* p)
//     {
//         for(std::size_t i = 0; i < N; i++)
//         {
//             if(&buffer[sizeof(T) * i] == p)
//             {
//                 state[i] = false;
//                 return;
//             }
//         }
//     }
//
// private:
//     unsigned char* buffer = nullptr;
//     bool* state = nullptr;
//     const std::size_t N;
// };

static std::vector<GLubyte> ColorMapTest(const std::vector<Body>* velocities)
{
    std::vector<GLubyte> out;
    out.reserve(4 * velocities->size());
    
    int i = velocities->size();
    while(i--)
    {   
        if(i >= 1000)
        {
            out.push_back(255);
            out.push_back(0);
        }
        else
        {
            out.push_back(0);
            out.push_back(255);
        }
        
        out.push_back(0);
        out.push_back(255);
    }

    return out;
}

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
    
    // Init BH tree
    BHTree tree;

    // Populate the space with the selected script
    PythonScene scene("scenes.galaxies");

    if(rwindow.initOK())
    {
        while(rwindow.windowOpen())
        {
            for(auto& body : *scene.getBodies())
            {
                tree.insertBody(&body);
            }
            
            rwindow.clearBuffer();
            pstate.updatePositions(Body::GetLinearPositionPool());
            auto colorMap = ColorMapTest(scene.getBodies());
            pstate.updateColors(&colorMap);
            shader.load("MVP", camera.getMatrix());
            pstate.draw();
            static bool repopulate = false;
            rwindow.render(camera, &repopulate);
            rwindow.swapBuffers();

            for(auto& body : *scene.getBodies())
            {
                PVector3 field = tree.calculateFieldOnPoint(body.getPosition(), 0.5f);
                body.move(field);
            }
            tree.reset();

            if(repopulate)
            {
                scene.reload();
            }
        }
    }
    return 0;
}
