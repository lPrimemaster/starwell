#pragma once
#include <unordered_map>
#include <vector>
#include <iostream>
#include <glad/gl.h>

#include "math.h"
// #include "rwindow.h"

class GenShader
{
public:
    GenShader();
    ~GenShader();

    bool swap(const std::string& name) const;
    void setRenderTarget();

    template<typename T>
    bool load(const std::string& name, const T& value) const
    {
        GLint location = glGetUniformLocation(activeProgram, name.c_str());

        if(location == -1)
        {
            std::cerr << "Uniform location for name '" << name << "' not found." << std::endl;
            return false;
        }

        if constexpr(std::is_same_v<T, float>)
        {
            glUniform1f(location, value);
            return true;
        }
        else if constexpr(std::is_same_v<T, int>)
        {
            glUniform1i(location, value);
            return true;
        }
        else if constexpr(std::is_same_v<T, PVector3>)
        {
            glUniform3fv(location, 1, value.data);
            return true;
        }
        else if constexpr(std::is_same_v<T, PVector4>)
        {
            glUniform4fv(location, 1, value.data);
            return true;
        }
        else if constexpr(std::is_same_v<T, PMatrix4>)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat*)value.data);
            return true;
        }
        else
        {
            std::cerr << "Uniform with type '" << typeid(value).name() << "' not suppported." << std::endl;
            return false;
        }
    }

private:
    void loadShaderCache();
    
private:
    std::unordered_map<std::string, GLuint> shaderCache;
    mutable GLuint activeProgram;
};

class InstanceState
{
public:
    InstanceState();
    InstanceState(const InstanceState& s) = delete;
    InstanceState(InstanceState&& s) = delete;
    ~InstanceState();
    
    void updatePositions(const std::vector<PVector3>* p);
    void updateColors(const std::vector<UVector4>* c);
    void draw() const;

private:
    void setAttribute(int location, GLuint buffer, int size, GLenum type, bool perInstance) const;

private:
    GLuint vao;
    GLuint billboard;
    GLuint position;
    GLuint color;
    unsigned long long particleCount = 0;
    static inline constexpr unsigned long long MAX_INSTANCE_PARTICLES = 100'000'000;
};
